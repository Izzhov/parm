# A simple script for running a Lennard-Jones simulation.

# Statistics such as energy, (instantaneous) temperature, and (instantaneous) pressure are
# output to "LJ-example.npz", and coordinates are output to "LJ-example.xyz".
# These xyz files can be read by VMD; a file named "LJ-example.tcl" is also created for use with vmd.
# You can then run 'vmd -e LJ-example.tcl LJ-example.xyz' to see the simulation.

# This was written for Python 3.4+; it may take some small effort to use a lower version of Python.

#---------------------------------------------------------------------------------------------------
from math import pi, sqrt
import numpy as np
import pyparm.d3 as sim # or d2 for 2D simulation
import pyparm.util as util

#---------------------------------------------------------------------------------------------------
# Parameters

# filenames
data_file = 'LJ-example.npz'
tcl_file = 'LJ-example.tcl'
xyz_file = 'LJ-example.xyz'

# system parameters
N = 40
N1, N2 = (N//2), (N - (N//2))
T0 = 1
phi = 0.3 # "packing fraction", i.e. (volume of spheres) / (volume of box)
sigmas = ([1] * N1) + ([1.4] * N2)

# simulation parameters
dt = 0.001
total_time = 100
data_n = 1000
xyz_n = 1000
print_n = 20

total_steps = int(total_time / dt + 0.5)
xyz_dt = float(total_time) / xyz_n
data_dt = float(total_time) / data_n
print_dt = float(total_time) / print_n

# purely calculated
d = sim.NDIM # number of dimensions
volumes = [s**d * pi/(2*d) for s in sigmas]
masses = [v / min(volumes) for v in volumes] # mass proportional to volume, but 1 for the smallest
L = (sum(volumes) / phi) ** (1.0/d) # length of each side of the box

#---------------------------------------------------------------------------------------------------
# Setting up the simulation
box = sim.OriginBox(L)
atoms = sim.atomvec(masses)
neighbors = sim.neighborlist(box, atoms, 0.4) # the neighborlist, for keeping track of what atoms are near what other atoms
LJ = sim.LJgroup(atoms, neighbors)
collec = sim.collectionVerlet(box, atoms, dt, [LJ], [neighbors]) # the integrator
# We use a simple velocity-verlet integrator, which is time-reversible and NVE ensemble
# i.e., it preserves number of atoms, volume of box, and energy

#---------------------------------------------------------------------------------------------------
# Initial Conditions
# Now we have created our interaction, but we need to add our atoms to it. We do that in a way that prevents overlap

E0 = 0
for a,s in zip(atoms, sigmas):
    E = E0 + 10
    a.v = sim.randVec() # from a gaussian distribution
    LJ.add(sim.LJatom(1, s, a))
    while E > E0 + 0.1:
        a.x = box.randLoc()
        neighbors.update_list()
        E = LJ.energy(box)
    E0 = E

collec.resetcomv() # subtract center-of-mass velocity from all particles
collec.scaleVelocitiesT(T0) # scale all velocities to get an instantaneous temperature T = T0, at least at the beginning

####################################################################################################
# Data Analysis

data_functions = {
    'E' : collec.energy,
    'T' : collec.temp,
    'U' : lambda: LJ.energy(box),
    'P' : collec.pressure
    }

data_arrays = {k : np.zeros((data_n,)) for k in data_functions}
data_arrays['t'] = np.zeros((data_n,))

def take_data(idx, time):
    """Take each measurement in data_functions at time 'time', and store it in data_arrays at index idx"""
    data_arrays['t'][idx] = time
    for k, f in data_functions.items():
        data_arrays[k][idx] = f()

def write_data(idx):
    delim = '\t'
    keys = sorted(data_arrays.keys())
    header = delim.join(keys)
    data = {k:data_arrays[k][:idx+1] for k in keys}
    np.savez_compressed(data_file, **data)

#---------------------------------------------------------------------------------------------------
# XYZ file

element_names = ['C']*N1 + ['O']*N2
def write_xyz(time):
    # print the current line to file
    with open(xyz_file, 'a') as f:
        print(N, file=f)
        print(time, file=f) # xyz format for VMD requires a line here, and ignores it; I put the time here.
        for e,a in zip(element_names, atoms):
            x = box.diff(a.x, sim.Vec())
            print(e, *x, file=f)

# empty out the file
with open(xyz_file, 'w') as f:
    f.truncate()

#---------------------------------------------------------------------------------------------------
# TCL file
# this is just to tell VMD to show the box and show the atoms as the right size.
# Not necessary if you're not using VMD for visualization.

tcl_str = """\
mol modstyle 0 0 VDW 1 32
mol rep VDW 1 20

set cell [pbc set {{{L} {L} {L}}} -all];
pbc box -toggle -center origin -color red;
set natoms [atomselect 0 "name C";];
$natoms set radius {r1};
set natoms [atomselect 0 "name O";];
$natoms set radius {r2};""".format(L=L, r1=sigmas[0]/2.0, r2=sigmas[-1]/2.0)

with open(tcl_file, 'w') as f:
    print(tcl_str, file=f)

xyz_m = 0
data_m = 0
print_m = 1

progress = util.Progress(total_time) # for tracking and printing our progress

for step in range(total_steps+1):
    if step > 0: collec.timestep()
    time = step*dt
    
    if time > xyz_m * xyz_dt:
        write_xyz(time)
        xyz_m += 1        
        
    if time > data_m * data_dt:
        take_data(data_m, time)
        write_data(data_m)
        data_m += 1
    
    if time > print_m * print_dt:
        print(progress.eta_str(time))
        print_m += 1
    
print("Done.")
