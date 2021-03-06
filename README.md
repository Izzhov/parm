ParM
====

#### The PARticle Molecular dynamics library

Purpose
----

The purpose of this library is to provide methods to produce *any* MD 
simulation, with any type of integrator, any type of particles, and any 
type of interaction in 2D or 3D.

It is currently single-core only, and provides objects for many 
different integrators, and many different forces, which can be composed 
together for simulations from a few particles bouncing around in a box 
to a full protein simulation.

## Basic Concepts

 * `Vec`: this is a "vector" in the physics sense, having either 2 or 3 
 dimensions.

 *  `atom`: an `atom` is the basic unit of the simulation; it 
 represents a particle with mass,
    position, velocity, etc.

 *  `atomgroup`: an `atomgroup` is a set of atoms, grouped together for 
 the sake of utility.
    * Note that `atomvec` is a concrete type, and `atomgroup` is an 
    abstract class

 *  `interaction`: an `interaction` is a definition of a force and 
 energy, such as a Lennard-Jones potential (use 
 `NListed<LJAttractRepulseAtom, LJAttractRepulsePair>`), springs for 
 bonds (`bondpairs`), etc.

    * Note that the neighbor list has been "abstracted" to work with 
    many potentials; to use it, you create a `neighborlist`, then use 
    `NListed<FooAtom, FooPair>` as the interaction

*  `Box`: a box is either infinite (`InfiniteBox`) or periodic 
(`OriginBox`), and takes care of the boundary conditions

*  `collection`: a grouping together of a `Box`, `atomgroup`, and 
`interaction`s, with an integrator (such as velocity Verlet, 
`collectionVerlet`, or browian motion, `CollectionSol`).

Standard Steps
----

1.  Make a `Box`
2.  Make an `atomvec`, setting masses, positions, and velocities to 0
3.  Make `interaction`s. For neighborlisted interactions, make 
`neighborlist` first, then make interactions.
    1.   Add atoms / pairs to interaction
4.  Make a `collection`. Note that the neighborlist has to be added to 
`trackers`
5.  Run `collection.timestep()` many, many times
    1.   Use methods such as `collection.kinetic()` or 
    `collection.temp()` to get statistics
    2. Or use `tracker`s like `RsqTracker` to track running statistics
6.  Write output to files

Dependencies
----

 - STL
 - Boost: primarily for random numbers, also a few odds and ends
 - (optional) SWIG: for generating Python bindings
 - (optional) Python: for generating Python bindings
    - Known to compile for python 3.2-3.4, and probably with 2.6-2.7

Python
----

This library includes a `sim.i` file for use with SWIG for generating 
Python bindings.

#### To Generate Python module

Run `make 2d` or `make 3d` to generate bindings for a 2D or 3D library;
run `make pyparm` to generate bindings for both.

#### Using the Python module

Use `import pyparm.d2 as sim` or `import pyparm.d3 as sim` to import
the module. Then use it freely.


Examples
----

#### C++

See `src/hardspheres.cpp` or `src/LJatoms.cpp` for examples of C++ 
programs. Both are well documented, much more so than the rest of this 
code, unfortunately.

#### Python

See `pyparm/examples/LJ.py` for an example of a simple Lennard-Jones 
simulation, with data analysis included.
