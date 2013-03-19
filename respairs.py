#!/usr/bin/env python
# encoding: UTF-8

from optparse import OptionParser
from itertools import count
import sys, datetime, collections
import yaml

from simw import *
import simpdb, xyzfile

def errprint(*args, **kw):
    kwargs = {'file':sys.stderr}
    kwargs.update(kw)
    print(*args, **kwargs)
    kwargs['file'].flush()

parser = OptionParser("Runs a simple simulation.")
parser.add_option('-T', '--temperature', type=float, dest='T', default=1.0)
parser.add_option('-D', '--damping', type=float, default=.001)
parser.add_option('-t', '--time', type=float, default=1.0)
parser.add_option('-d', '--dt', type=float, default=.01)
parser.add_option('--LJcutoff', type=float, default=2.5, help='Ratio to σ')

parser.add_option('-s', '--spacing', type=float, default=20.0,
                help='Initial space between residues')
parser.add_option('-k', '--springconstant', type=float, default=10.0,
                help='Spring constant for spring between Cα-Cα')
parser.add_option('-X', '--springdist', type=float, default=-1.0,
                help='Spring equilibrium distance between Cα-Cα (default: spacing)')

parser.add_option('--rampsteps', type=int, default=0)
parser.add_option('--rampdamp', type=float, default=.5)
parser.add_option('--rampdt', type=float, default=0.0)
                
parser.add_option('-S', '--showcount', type=int, default=0, 
                    help='Number of output lines (overrides showsteps)')
parser.add_option('-Z', '--showsteps', type=float, default=.25,
                    help='Time between output lines (in thousands)')
parser.add_option('-g', '--statfile', dest='statfile', 
            default=None)
parser.add_option('-x', '--xyzfile', dest='xyzfile', 
            default=None)
parser.add_option('--fixedrepulsion', action='store_true')
parser.add_option('--eisenbergmclachlan', action='store_true')

errprint(" ".join(sys.argv))
opts, args = parser.parse_args()
steps = int(opts.time * 1000 / opts.dt + .5)
showtime = opts.time * 1000 / opts.showcount if opts.showcount else 1000*opts.showsteps
showsteps = int(showtime / opts.dt + .5)

def errprint(*args, **kw):
    kwargs = {'file':sys.stderr}
    kwargs.update(kw)
    print(*args, **kwargs)
    kwargs['file'].flush()

assert len(args) == 2
#~ args = [a.upper() for a in args]
#~ for arg in args:
    #~ assert (arg in simpdb.res31 or arg in simpdb.res13), "Residue %s not understood" % arg

parameters = []
for arg in args:
    with open(arg,'r') as f:
        resparams = yaml.load(f)
    parameters.append(resparams)

trackers = []
interactions = {}
constraints = []

########################################################################
## Make atoms & L-J
sigmas = [s for rp in parameters
            for resdict in rp['atoms']
            for m,s,i in resdict['sizes'].values()
         ]
maxsigma = max(sigmas)

innerradius = (maxsigma * opts.LJcutoff
                    if not opts.eisenbergmclachlan 
                    else maxsigma*2 + 2.8) # two diameters + 1 water diameter
outerradius = innerradius * 2.5
#~ neighbors = neighborlist(innerradius, outerradius)
neighbors = neighborlist(innerradius, outerradius)
trackers.append(neighbors)

if 'hydrophobicities' in parameters[0] and not opts.fixedrepulsion:
    interactions['LJ'] = LJ = LJAttractRepulse(neighbors)
    htable = parameters[0]['hydrophobicities']
    def hadd(hindex, sigma, atom, cutoff):
        LJ.add(LJAttractRepulseAtom(
                htable[hindex], sigma, hindex, atom, cutoff
            ))

elif opts.eisenbergmclachlan:
    interactions['LJ'] = LJ = LJgroup(neighbors)
    interactions['Eis'] = Eis = EisMclachlan(neighbors)
    def hadd(H, sigma, atom, cutoff=0):
        LJ.add(LJatom(1, sigma, atom))
        Eis.add(EisMclachlanAtom(sigma + 1.4, H, atom))
    
elif opts.fixedrepulsion:
    interactions['LJ'] = LJ = LJAttractFixedRepulse(neighbors)
    htable = parameters[0]['hydrophobicities']
    def hadd(hindex, sigma, atom, cutoff):
        LJ.add(LJAttractFixedRepulseAtom(
            htable[hindex], 1, sigma, hindex, atom, cutoff
        ))
else:
    interactions['LJ'] = LJ = LJDouble(neighbors)
    def hadd(hindex, sigma, atom, cutoff):
        LJ.add(LJDoubleAtom(
            hindex, 1, sigma, atom, cutoff
        ))

rvecs = []
startvec = Vec(0,0,0)
nvec = Vec(opts.spacing,0,0)
for n,rp in enumerate(parameters):
    resdict, = rp['atoms']
    anames, masses_indices = zip(*sorted(resdict['sizes'].items()))
    masses, radii, indices = zip(*masses_indices)
    res = Res(anames, masses)
    
    for atom in res:
        m, sigma, hindex = resdict['sizes'][atom.name]
        x,y,z = resdict['xyz'][atom.name]
        atom.x = Vec(x,y,z) + (nvec * n)
        hadd(hindex, sigma, atom, opts.LJcutoff)
        #~ errprint("Adding", name, "mass %.2f" % m, "width %.2f" % sigma);
    
    rvecs.append(res)

#-----------------------------------------------------------------------
## Make bonds and angles
interactions['bond'] = bonds = bondpairs()
for n,rp in enumerate(parameters):
    for n1,aname1,n2,aname2,x0,k in rp['bonds']:
        a1,a2 = rvecs[n+n1][aname1], rvecs[n+n2][aname2]
        newk = max((k, 200.0))
        bonds.add(newk,x0,a1,a2)
        neighbors.ignore(a1,a2)
        #~ errprint("Connecting %s-%s (%d-%d)" % (aname1, aname2, n+n1, n+n2), newk, x0)

errprint("Made", sum([len(rp['bonds']) for rp in parameters]), "bonds,", end=' ')

interactions['angles'] = angles = angletriples()
for n,rp in enumerate(parameters):
    for n1,aname1,n2,aname2,n3,aname3,q0,k in rp['angles']:
        a1,a2,a3 = rvecs[n+n1][aname1], rvecs[n+n2][aname2], rvecs[n+n3][aname3]
        angles.add(k,x0,a1,a2,a3)
        neighbors.ignore(a1,a3)
        #~ errprint("Connecting %s-%s-%s (%d-%d-%d)" % (aname1, aname2, aname3, n+n1, n+n2, n+n3))

errprint(sum([len(rp['angles']) for rp in parameters]), "angles,", end=' ')

#-----------------------------------------------------------------------
## Make basic spring
interactions['spring'] = resspring = bondpairs()
a1,a2 = rvecs[0]['CA'], rvecs[1]['CA']
CAx0 = opts.springdist if opts.springdist >= 0 else opts.spacing
resspring.add(opts.springconstant, CAx0, a1,a2)

#-----------------------------------------------------------------------
## Make constraints
constraints = [
    coordConstraint(rvecs[0]['CA'], False, True, True, Vec(0,0,0)),
    coordConstraint(rvecs[1]['CA'], False, True, True, Vec(0,0,0)),
    relativeConstraint(rvecs[0]['C'], rvecs[0]['N'], True, False, False, Vec(0,0,0)),
    relativeConstraint(rvecs[1]['C'], rvecs[1]['N'], True, False, False, Vec(0,0,0))
    ]

#-----------------------------------------------------------------------
## Create simulator
errprint("Ignoring", neighbors.ignore_size(), "pairs.")
atomgroups = [r.atomvec for r in rvecs]
collec = collectionSol(opts.dt, opts.damping, opts.T,
                            atomgroups, list(interactions.values()),
                            trackers, constraints)
collec.seed()
collec.setForces()

########################################################################
## Run simulation

def statistics(t, c=collec):
    d = dict(
        time=t,
        E=collec.energy(),
        KE=collec.kinetic(),
        T=collec.temp(),
        Distance=(rvecs[0]['CA'].x - rvecs[1]['CA'].x).mag() - CAx0
        )
    for name, i in interactions.items():
        d[name + 'E'] = i.energy()
    return d

#-----------------------------------------------------------------------
# Ramping
if opts.rampsteps > 0:
    collec.resetcomv()
    collec.resetL()
    errprint("Damping for", opts.rampsteps, "steps:\n    E: ", end='')
    collec.changeT(opts.rampdt or opts.dt, opts.rampdamp, opts.T)
    for i in range(opts.rampsteps):
        collec.timestep()
        if i % (opts.rampsteps / 5) == 0:
            errprint("%.3f.." % collec.energy(), end='')
    collec.changeT(opts.dt, opts.damping, opts.T)
    collec.resetcomv()
    collec.resetL()
    errprint("Done.")

startt = t = 0
curlim = t + showsteps
#errprint('Starting', t, curlim, 'E: %8.3f' % collec.energy())
starttime = datetime.datetime.now()
valtracker = collections.defaultdict(list)

if opts.xyzfile:
    global xyz
    print(' '.join(rvecs[0].names), '::', ' ' .join(rvecs[1].names))
    xyz = xyzfile.XYZwriter(open(opts.xyzfile, 'w'), usevels=False, printnames=True)

def statprintline(stats):
    global opts, xyz, collec
    if opts.statfile:
        ks, vs = zip(*sorted(stats.items()))
        with open(opts.statfile, 'a') as f:
            errprint(*['%.6g' % v for v in vs], file=f, sep='\t')
    if opts.xyzfile:
        xyz.writeframe(rvecs, com=collec.com(), **stats)

if opts.statfile:
    stats = statistics(0)
    with open(opts.statfile, 'w') as f:
        ks, vs = zip(*sorted(stats.items()))
        errprint(*ks, file=f, sep='\t')
    statprintline(stats)

#-----------------------------------------------------------------------
## Actual run
try:
    while t < steps:
        while t < curlim:
            collec.timestep()
            t+=1
        curlim += showsteps
        c = collec.com()
        #~ values = xyz.writefull(int(t * opts.dt+.5), avecs, collec)
        
        stats = statistics(t*opts.dt)
        statprintline(stats)
        #~ table.update(int(t * opts.dt+.5), write=True)
        
        endtime, interval = simpdb.get_eta(steps-t, t-startt, starttime)
        days, hr, mn, sec = interval
        
        errprint('------ ', int(t*opts.dt+.5), 
            ' Ends in (', (('%dd ' % days) if days else ''),
                        '%d:%02d:%02d)'% (hr, mn, sec), ' on ', 
                                endtime.strftime('(%a %b %d, %H:%M:%S)'),
            sep = '')
        for k,v in sorted(stats.items()):
            valtracker[k].append(v)
            if k is 'time': continue
            simpdb.printlist(valtracker[k], k, file=sys.stderr)
        sys.stderr.flush()
except KeyboardInterrupt:
    errprint('Keyboard Interrupt, stopped.')

#-----------------------------------------------------------------------
## Finish
tottime = datetime.datetime.now() - starttime
totsecs = 24*60*60 * tottime.days + tottime.seconds + (tottime.microseconds / 1000000.)

errprint("%s, %.3f fps" % (tottime, (t-startt) / totsecs))

