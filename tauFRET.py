from numpy import sqrt, mean, std, array

Forsterdist = 54

ijs = [ (291, 322), (291, 354), (354, 433), (103, 184),
        ( 17, 103), (184, 291), (244, 354), (322, 433), 
        (291, 433), (103, 291), ( 17, 291), ( 17, 433)]

ETeffs = { (291, 322) : 0.81, (291, 354) : 0.61, (354, 433) : 0.62, 
        (103, 184) : 0.70, ( 17, 103) : 0.17, (184, 291) : 0.36,
        (244, 354) : 0.37, (322, 433) : 0.51, (291, 433) : 0.42, 
        (103, 291) : 0.33, ( 17, 291) : 0.42, ( 17, 433) : 0.22 }
        
ETerrs = {( 17, 433) : 0.04,  ( 17, 291) : 0.062, ( 17, 103) : 0.032,
          (103, 184) : 0.067, (103, 291) : 0.055, (291, 433) : 0.062,
          (322, 433) : 0.067, (354, 433) : 0.068, (291, 322) : 0.054,
          (291, 354) : 0.069, (244, 354) : 0.058, (184, 291) : 0.057 }

ETs = [ETeffs[ij] for ij in ijs]
ETerrlst = [ETerrs[ij] for ij in ijs]

def ETeffrsq(d):
    dists = [(d[ij] - ETs[ij])**2 for ij in ijs]
    return sqrt(mean(dists))
    
def ETeffrsqerrs(simETs, simETerrs=None):
    # \Delta=\sqrt{\left\langle \left(x_i-y_i\right)^2\right\rangle}
    # \sigma_{\Delta}^2=\sum_{i=1}^{N}\left(\frac{\partial\Delta}{\partial x_i}\sigma_{x_i}\right)^2+\left(\frac{\partial\Delta}{\partial y_i\sigma_{y_i}\right)^2
    # \frac{\partial\Delta}{\partial x_i} = \frac{x_i-y_i}{N\Delta}
    dxs = array([(simETs[ij] - ETeffs[ij]) for ij in ijs])
    sigxs = array([simETerrs[ij] for ij in ijs]) if simETerrs else array([0.0 for ij in ijs])
    sigys = array([ETerrs[ij] for ij in ijs])
    N = len(ETeffs)
    
    D = sqrt(mean(dxs**2))
    
    dDdxi = dxs/D/N
    Derr = sqrt(sum((dDdxi * sigys)**2 + (dDdxi * sigxs)**2))
    
    return D, Derr
    
def ETeffrsqerrs_old(simETs, simETerrs):
    dxs = array([(simETs[ij] - ETs[ij]) for ij in ijs])
    sigys = array([simETerrs[ij] for ij in ijs])
    N = len(simETs)
    
    D = sqrt(mean(dxs**2))
    
    dDdxi = dxs/D/N
    Derr = sqrt(sum((dDdxi * sigys)**2))
    
    return D, Derr