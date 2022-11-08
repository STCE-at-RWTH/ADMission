# admission User Guide {#mainpage}

This tool for __A__ lgorithmic __D__ ifferentiation __Mission__ Planning allows the user
to find optimal (minimal in cost) orders of tangent or adjoint model executions and matrix-matrix products
resulting in the accumulation of the Jacobian of codes consisting of lage, independent elemental code blocks.
Essentially, admission finds (near) optimal solutions of the _Face Elimination_ Problem on the call graph (_DAG_)
of the code.
Depending on the computational costs of the tangent and adjoint models
available for the elemental code blocks and the sizes of input and output vectors,
admission finds a sequence of
accumulations of _elemental Jacobian_ matrices, matrix-matrix products and propagations
of elemental Jacobians through tangent and adjoint implementations of the elemental code blocks.
The result is intended as a reference for the efficient implementation of the actual
Jacobian accumulation of the code.


## Prequisites for building admission

* A c++ 17 and OpenMP able compiler (e.g. gcc/9).
* A recent version of the boost library.

## Building admission

1. Create a build directory. This can be inside the admission source directory, which we call  
`adm` here. You may end up with a build directory called `adm/build`.
2. Install boost into `adm/extern/boost` or alternatively configure the environment variables `BOOST_INC` and `BOOST_LIB` to point to your installed boost version. Versions boost_1_72_0 and boost_1_76_0 were tested. Recent versions of boost generate multiple warnings when compiling admission, caused by boost using deprecated parts of itself. Don't worry about them.
3. run `cmake path/to/project/dir -DCMAKE_BUILD_TYPE=Release` from inside the build dir
   This may amount to calling `cmake .. -DCMAKE_BUILD_TYPE=Release` if the build dir is `adm/build`.
   Add `-DUNITTESTS=ON` to build the unit test binaries `adm_unit_test` and `adm_branch_and_bound_test`.
   Add `-DDOXYGEN=ON` to set up doxygen for building the documentation.
4. run `cmake --build . --target admission` to build
   the admission executable.
   run `cmake --build . --target doc` to build the doxyden documentaion. You can open `html/index.html` from within the build dir.
   You can also manually build the doxygen documentation by invoking `doxygen Doxyfile.AdmissionDoc` from inside the build dir.
  `cmake --build . --target name` can be used to build any executable that is part of admission.
  `cmake --build .` will build all executables that are part of admission.
   Add `-j4` to speed up the build process by using 4 threads. You may use larger numbers if your machine has more than 4 CPUs.
   `make test` will build and run all unit tests, including `adm_branch_and_bound_test` which may take a very long time to complete.
5. The executables can be found in `adm/bin` which is independent of the location of your build dir.

## Using admission

The program solving the Face Elimination problem (FE) on (matrix-free)(vector) face DAGs
is simply called `admission`. Running `./admission` will provide you with a set of parameters you can specify using a configuration file.
All parameters are specified by the parameter name folloewd by a whitespace and the value. Boolean values must be specified as 0 or 1.
All given parameters will be parsed and set. If any parameter value appears to be invalid during set-up of the problem, the program will explicitly ask for it on the console. If an unknown parameter tag is passed, the program will stop.
The list of possible parameters is:
* _graph\_path_: A relative path to the DAG.
* _optimizer\_name_: Identifier of the optimizer to solve FE. _BranchAndBound_, _GreedyOptimizer_ or _MinFillInOptimizer_.
* _thread\_spawn\_depth_: Max depth of the search tree in which searching of branches is delegated to OpenMP tasks.
* _n\_threads_: Max number of threads to use in a parallel optimizer and parallel sparsity pattern computation.
* _print\_diagnostics_: Diagnostic output of the branch and bound algorithm will be written to disk.
* _preaccumulate\_all_: Will treat the input DAG as if no tan/adj models were available and all elemental Jacobians were preaccumulated.

### Interpreting the output
While solving, `admission` will provide you with single-line update of the solution process, which includes the number of performed modifications to the DAG,
the cost of the current optimal solution and a few more values.
Once the final solution is obtained, `admission` will present you with the total time required for solving, the number of eliminations and accumulations of the optimal solution sequence,
its cost and a list that describes the single eliminations and accumulations.
It is to be interpreted as follows:
* __PRE__: This step is the pre-accumulation of an elemental Jacobian.
  * __ADJ__: The preaccumulation is done in adjoint (reverse) mode.
  * __TAN__: The preaccumulation is done in tangent (forward) mode.
  * (i,j) : Index pair identifying the preaccumulated elemental Jacobian F'\_ji.
* __ELI__: This step is the elimination of an intermediate edge, e.g. the computation of a Jacobian-Jacobian product in some way.
  * __ADJ__: The product is computed by propagation of a Jacobian through an adjoint model.
  * __TAN__: The product is computed by propagation of a Jacobian through a tangent model.
  * __MUL__: The product is computed as an explicit product of preaccumulated Jacobians.
  * (i,j,k) : Index triplet identifying the multiplied Jacobians F'\_kj and F'\_ji.

Each entry is followed by the cost of the individual elimination or accumulation in terms of _fused multiply-adds_ (`fma`).
`admission` additionally computes the cost of a homogenous global tangent and adjoint approach.

### A sample session of admission

We would like to solve the dense matrix-free Jacobian accumulation problem on a version of the Lion DAG, which was presented in research on Jacobian accumulation
in various papers. To solve the matrix-free problem, we write a file `admission.in` and call `./admission admission.in`.

`admission.in` could look like

    graph_path ../graphs/paper/lion_n1_c1.xml
    optimizer_name BranchAndBound

Some of the parameters were left out and will be set to somewhat useful default values.
After a while `admission` tells us:

    Printing DAG to TikZ for you...
    Planning AD Mission...
    Curr. Opt.: 13  Search space: 42961250 Cuts: 36925166 Branches : 6036084 Leaves: 7
    Done after 1.7187s

    Op |Dir|Where|c(fma)
    PRE ADJ 1 2 1
    ELI TAN 1 2 5 1
    PRE ADJ 1 5 1
    ELI ADJ 0 1 2 2
    ELI ADJ 0 1 5 2
    ELI TAN 0 2 3 4
    ELI TAN 0 2 4 2

    Statistics:
      Search space: 42987687
      Cuts: 36947536
      Branches: 6040151
      Leaves: 7
    Global tangent cost: 16fma
    Global adjoint cost: 32fma
    Optimized cost: 13fma
    Operations: 7


We see, the solution contains of 7 steps and has a cost of 13fma.
The list of all steps is printed above.
The statistics tell us that 42961250 eliminations were performed.
6036084 resulted in a face DAG that could still yield an optimal solution.
36925166 branches were cut.


Consider, we want to solve matmul JA on the R2 DAG using 6 threads. The configuration file could look like.

    graph_path ../graphs/paper/lion_n1_c1.xml
    optimizer_name BranchAndBound
    n_threads 6
    preaccumulate_all 1

Because for this kind of problem no special lower bound is available, the solution takes a bit longer
and provides us with some information about the solution process:

    Curr. Opt.: 769044  Search space: 165622 Cuts: 114670 Branches : 50952 Leaves: 11

After a second (on the authors rather old machine), the program prints the usual output:

    Done after 0.365209s

    Op |Dir|Where|c(fma)
    ELI MUL 2 3 4 30400
    ELI MUL 0 2 4 46208
    ELI MUL 0 1 4 20672
    ELI MUL 0 4 5 52288
    ELI MUL 0 5 6 372552
    ELI MUL 0 6 7 246924

    Statistics:
      Search space: 193510
      Cuts: 133288
      Branches: 60222
      Leaves: 11
    Local Preaccumulation cost: 792641fma
    Global tangent cost: 2143732fma
    Global adjoint cost: 1607799fma
    Optimized cost: 769044fma
    Operations: 6


## Generators for matrix-free Jacoian chain products and random DAGs.

`admission` comes with `gdjcpb_generate`, a _Generalised Dense Jacobian Chain ProBlem generator_ from prior research
and a customized DAG generator `adm_generate_dag`.
Call `./gsjcpb_generate 10 100 1` to generate a matrix chain of length 10 with a maximum matrix dimension of 100.

Calling `./adm_generate_dag` without any arguments will give you a list of parameters:

                 num_vertices: Number of vertices in the graph.
    num_intermediate_vertices: Number of intermediate vertices in the graph. Must be smaller than NumVertices-2.
                  coeff_const: Parameter of the probability density function of the distance spanned by an edge. Const distribution.
              coeff_ascending: Parameter of the probability density function of the distance spanned by an edge. Ascending probability.
             coeff_descending: Parameter of the probability density function of the distance spanned by an edge. Descending probability.
                    coeff_sin: Parameter of the probability density function of the distance spanned by an edge. Modified sine wave probability.
              max_vertex_size: Max vector size of a vertex. Actual size is randomly chosen in [1, MaxVertexSize].
                   max_in_out: Maximal number of in-edges and out-edges of any vertex.
                 cost_scaling: Scale the max value for the random cost generator relatively to vertex sizes.

Use `./adm_generate_dag adm_generate_dag.in` with `adm_generate_dag.in` looking like

    num_vertices 20
    num_intermediate_vertices 15
    coeff_const 0.0
    coeff_ascending 0.0
    coeff_descending 1.0
    coeff_sin 0.0
    max_vertex_size 50
    max_in_out 2
    cost_scaling 10

to generate a DAG with 20 vertices, 15 of which are intermediate and 5 either minimal or maximal vertices.
The probability function is chosen such that it is more likely that vertices close to each other (in terms of
the partial ordering of the DAG) are connected by an edge.
The maximum size of a vertex is 50. The desired number of in- and out-edges of each vertex is 2, meaning that
each vertex has a number of desired in- and out- edges which is at max 2, but can actually have more
predecessors or successors than that. Play with the numbers until you find a setup that fits you.

`adm_generate_dag` prints the generated DAG as GRAPHML as well as a TikZ image in a standalone latex file.
        
# admission Tech Guide
    
The admission framework allows to represent vector (face) DAGs and to implement the algorithms presented in the bachelor thesis 
"Extending the optimal Jacobian
accumulation problem to vector-valued
functions" by Erik Schneidereit.
The development of admission included finding efficient ways to represent vector DAGs in a computer program,
enabling future research on this topic to use this representation.
Furthermore, admission was used to compare the results of the branch and bound algorithm
with a gerenalized critical degree lower bound to results obtained with an exhaustive search
and to test different greedy strategies. 
Testing different algorithms can enable important insight into how these algorithms perform on actual graphs
and how they can be enhanced.
    

The list of basic user requirements for admission is:
   * Representation of a (matrix-free) vector (face) DAG.
   * Import of matrix-free vector DAGs and matrix-free vector face DAGs that are stored in the GRAPHML format.
   * Construction of a face DAG from a DAG.
   * The ability to apply face-eliminations to vector face DAGs.
   * A greedy-heuristic to find a near optimal elimination sequence.
   * A fast (parallel) branch and bound algorithm to find an optimal elimination sequence.
   * A lower bound for matrix-free vector Jacobian accumulation.

Apart from these basic requirements, a set of unit tests is implemented.
It is used for testing the correctness of basic algorithms
like the construction of absorption-free sub graphs,
the generation of random DAGs,
the construction of face DAGs from DAGs,
the application of basic eliminations
and the performance of the lower bound, the greedy optimizer
and the branch and bound.

## Choice of the programming language and used libraries

Requirements 1 and 6 suggest that the representation of a DAG needs to be modified
efficiently to be able perform face-eliminations in (if possible) constant time.
The BGL library, which is a sub library of Boost, appears to be a suited candidate for this task.
Boost is used together with the C++17 standard.

googletest is used to automate the generation of unit-tests, allowing to simply specify 
the solver and its settings, based upon which a set of unit tests is generated automatically.
The speedup of the branch and bound algorithm is realised with shared-memory parellelisation
using openMP.


# Structure of the admission framework

The admission framework is class based, meaning that its main functionalities
an be instantiated as objects and then be applied to a face DAG.
This is especially important for algorithms that have the same interface and
basically solve the same task, like a set of optimisation algorithms or a set of lower
bounds.

A large part of the functionality used by 
these main algorithms is implemented as free-standing functions,
such as the `make_face_dag()`, `longest_path()`, `global_tangent_cost()`
and many more. All free standing functions and classes are grouped and
can be found in the `Modules` tab of the DOXYGEN documentation.


## The DAG and FaceDAG classes

admission operates on two base classes for DAGs.
Both are based on the
`boost::adjacency_list<...>` class and differ only in the properties
that are attached to their vertices and edges.
They use the `VecS` selector for the vertex storage,
meaning that the vertices of a graph are stored in a `std::vector<...>`
allowing insertion of vertices by resizing.
We don not delete vertices from `DAG`s and `FaceDAG`s but rather isolate them
(i.e. delete all their edges). Using `VecS` as a storage allows to directly
address vertices by their index and faster copy of `DAG`s and `FaceDAG`s.

The `DAG` and `FaceDAG` use the `BidirectionalS` selector, 
forcing the storage of in- and out-edges
of each vertex (rather than only the out-edges), allowing fast access of the predecessor
sets of vertices required for face elimination.
They also use the `VecS` selector for storing in- and out-edges
of a vertex as a vector and allowing insertions and deletions in linear time.

Both graph classes also store the optimal (or best known) number of `fma` required to
solve the FE on them,
as well as an array of identifiers for vertices associated with
a prior elimination.
The `FaceDAG` class stores an edge index, representing the index of the corresponding `DAG` vertex.
Edge indices in the `FaceDAG` are not unique.

## The Optimizer class

The `Optimizer` class is the central class providing functionality for
finding a (near) optimal elimination sequence a `FaceDAG`.
It provides the `virtual OpSequence solve(const Graph& g)` function
that returns an `OpSequence`.
An `OpSequence` wraps around
a `std::list` of `OpCont`, which stores either a preaccumulation of an elimination.
`OpCont` possesses a pointer to an 
`AccOp` (accumulation Operation) and
`MultOp` (an operation that performs a multiplication, either explicit or by tangent/adjoint propagation),
which defines a set of functions using the data in `OpCont`.
This allows to store both kinds of operation in a vector.
An `OpSequence` can be applied to a `FaceDAG`.

The `Optimizer` class
does not implement the solve function,
but leaves the implementation to its
derived classes,
using different approaches for obtaining a solution.
Instead, it provides a set of functions to search a `FaceDAG` for different types of 
eliminations.

The `Optimizer` class is specialised by the `GreedyOptimizer`, 
`BranchAndBound` and `MinFillInOptimizer` classes.
Both implement the respective ansatz for solving FE using the set of functions
available from their base class.

## The LowerBound class

The `LowerBound` base class is used by the `BranchAndBound` class
to decide whether to explore a branch or not.

The base class `LowerBound` provides the function
`flop_t operator()(const FaceDAG&)`,
and implements the trivial lower bound that just computes 0.
Other lower bounds inherit from LowerBound and override `operator()`.
The only other lower bound implemented in this version of admission
is the `SimpleMinAccCostBound`.
