# ADMission User Guide

This tool for __A__ lgorithmic __D__ ifferentiation __Mission__ Planning allows the user
to find optimal (minimal in cost) orders of tangent or adjoint model executions and matrix-matrix products
resulting in the accumulation of the Jacobian of codes consisting of lage, independent elemental code blocks.
Essentially, ADMission finds optimal solutions of the _Face Elimination_ Problem on the directed acyclic graph (_dag_)
of the code, which is assumed to be known.
Depending on the computational costs of the tangent and adjoint models
available for the elemental code blocks as well as the sizes of input and output vectors,
ADMission finds a sequence of
accumulations of _elemental Jacobian_ matrices, matrix-matrix products and propagations
of elemental Jacobians through tangent and adjoint implementations of the elemental code blocks.
The result serves as a reference for the acual implementation of the
Jacobian accumulation of the code.


## Prequisites for building ADMission

* A C++17 compiler with OpenMP support (e.g. `gcc 9.4.0`, `10.1.0`, `11.2.0` or `12.2.0`).
* A recent version of the boost library (e.g. `boost 1.52`, `1.53`, `1.69`, `1.73`, `1.79` or `1.80`).
  - Be aware that some versions of boost are incompatible, such as `boost 1.70`!
  - If you use the `libboost-dev` debian package you'll need the `libboost-graph-dev` package as well!


## Building ADMission

The CMake files delivered with ADMission are able to install most of ADMission's dependencies.
Admission uses the following external projects:
* googletest: A suite for automating software tests by google.
* drag: A library for drawing DAGs in a layered mode.

1. Create a build directory.
2. Configure the project with cmake or ccmake. You have several build types (`Debug, Release, MinSizeRel, RelWithDebInfo`) to choose from.
4. Build the ADMission binary by running `make -j<N> admission`, where `<N>` is the number of threads to use.
5. You can build the entire ADMission project including tests and documentation by running `make -j<N>`.
6. The executables are stored in the 'bin' subdirectory of the build dir.

## Using admission

The program solving the Face Elimination problem (FE) on face DAGs is simply called `admission` and expects you to provide a
configuration file. You can find examples in the `examples` folder. Alternatively, running 'admission' without any arguments
will print a list of parameters you can specify in the config file:
* _dag_: Path to the DAG.
* _method_: Name of the optimization method.

### Interpreting the output
While solving, `admission` will provide you with single-line updates of the solution process, which includes the number of performed modifications to the DAG,
and the cost of the current optimal solution.
Once the final solution is obtained, `admission` presents you with the elapsed time and the optimized solution sequence.
It is to be interpreted as follows:
* __ACC__: This step is the pre-accumulation of an elemental Jacobian.
  * __ADJ__: The preaccumulation is done in adjoint (reverse) mode.
  * __TAN__: The preaccumulation is done in tangent (forward) mode.
  * (i,j) : Index pair identifying the preaccumulated elemental Jacobian F'\_ji.
* __ELI__: This step is the elimination of an intermediate edge, e.g. the computation of a Jacobian-Jacobian product in some way.
  * __ADJ__: The product is computed by propagation of a Jacobian through an adjoint model.
  * __TAN__: The product is computed by propagation of a Jacobian through a tangent model.
  * __MUL__: The product is computed as an explicit product of preaccumulated Jacobians.
  * (i,j,k) : Index triplet identifying the multiplied Jacobians F'\_kj and F'\_ji.

Each entry is followed by the cost of the individual elimination or accumulation. `admission` additionally computes the cost of a global tangent and adjoint approach.

### A sample session of ADMission

We would like to solve the dense matrix-free Jacobian accumulation problem on a version of the Lion DAG, which was presented in research on Jacobian accumulation
in various papers. To solve the matrix-free problem, we write a file `admission.in` next to the DAG xml file. Then we call `<build folder>/bin/admission admission.in`.

`admission.in` could look like

    dag lion.xml
    method BranchAndBound

After a while `admission` tells us:
```
elapsed time: 0.197694s

elimination sequence
  (operation mode target cost):
  ACC TAN (0 1) 4
  ELI TAN (0 1 2) 2
  ELI TAN (0 1 4) 2
  ELI TAN (0 2 3) 6
  ELI TAN (0 2 4) 2

dense tangent cost: 16
dense adjoint cost: 64
optimized cost: 16

branch and bound statistics:
  number of discovered branches: 416
  number of explored branches: 190
```

We see, the solution contains of 5 steps and has a cost of 16.
The statistics tell us that of 416 discovered branches, 190 were explored.

### Examples

All examples are also available as build targets, e.g. running
```
make lion
```
will execute the lion example for you. All example files are copies into the
`<build folder>/bin/` folder where the `admission` executable is. Here you can
also edit the config files and DAGs and run `./admission <config file>` to
execute the examples manually.

To run all examples, use:
```
make examples
```