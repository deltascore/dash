DASH
====

A C++ Template Library for Distributed Data Structures with Support
for Hierarchical Locality for HPC and Data-Driven Science

Build and Installation
----------------------

To build the DASH project using CMake, run:

  (dash/)$ cmake --build .

Or, specify a new directory used for the build:

  (dash/)$ mkdir build && cd ./build

For a list of available CMake parameters:

  (build/)$ cmake . -L

Parameters can be set using -D flags. As an example, these parameters
will configure the build process to use clang instead of the default
compiler:

  (build/)$ cmake -DCMAKE_CXX_COMPILER=clang++ \
                       -DCMAKE_C_COMPILER=clang ..

Optionally, configure build parameters using ccmake:

  (build/)$ ccmake .

### 1. Choosing a DASH runtime (DART)

DASH provides the following variants:

  - MPI: the Message Passing Interface
  - CUDA: nNvidia's Compute Unified Device Architecture
  - SHMEM: Symmetric Hierarchical Memory access

The build process creates the following static libraries:

  - libdart-mpi
  - libdart-cuda
  - libdart-shmem

By default, DASH is configured to build all variants of the runtime.
You can define which implementation of DART to build using the cmake
parameter

  (build/)$ cmake -DDART_IMPLEMENTATION=mpi|cuda|shmem ...

Programs using DASH select a runtime implementation by linking against the
respective library.

### 2. Developer Settings

To activate a previous version of the DART interface, specify the
cmake parameter

  (build/)$ cmake -DDART_INTERFACE_VERSION=x.y ...

### 3. Examples and Unit Tests

Source code of usage examples of DASH are located in dash/examples/.
Examples each consist of a single executable and are built by default.
Binaries from examples and unit tests are deployed to the build direcory,
but will not be installed.
To disable building of examples, specify the cmake parameter

  (build/)$ cmake -DBUILD_EXAMPLES=OFF ...

To disable building of unit tests, specify the cmake parameter

  (build/)$ cmake -DBUILD_TESTS=OFF ...

The example applications are located in the bin/ folder in the build
directory.

### 4. Installation

The default installation path is /usr/local/
To specify a different installation path, use

  (build/)$ cmake -DINSTALL_PREFIX=/your/install/path ../

The option "-DINSTALL_PREFIX=YourCustomPath" can also be given in Step 1.

The installation process copies the 'bin', 'lib', and 'include' directories
in the build directory to the specified installation path.

  (dash/)$ cmake --build . --target install

Or manually using make:

  (build/)$ cmake <build options> ../
  (build/)$ make
  (build/)$ make install

Running DASH Applications
-------------------------

With the MPI variant, applications are spawn by MPI:

  $ mpirun <app>-mpi

For CUDA and SHMEM, use

  $ dartrun-cuda <app>-cuda

and respectively

  $ dartrun-shmem <app>-shmem

Profiling DASH Applications using IPM
-------------------------------------

Compile IPM as described in ipm/INSTALL. Then, use `LD_PRELOAD` to run a
DASH application built with the DART-MPI backend:

  $ LD_PRELOAD=/path/to/ipm/lib/libipm.so mpirun -n <nproc> <DASH executable>

Available options for IPM are documented in the
[IPM user guide](http://ipm-hpc.org/docs/user.php).

Links
-----

The DART project homepage: http://www.dart-project.org
The Munich Network Management homepage: http://www.mnm-team.org

Developer Notes
===============

This section is relevant if you intend to contribute code to the DASH project.

Code Style
----------

We follow the Google C++ Style Guide which is widely accepted in prevalent
open source projects:

http://google.github.io/styleguide/cppguide.html

The standards defined by the LLVM team are worth mentioning, too:

http://llvm.org/docs/CodingStandards.html

Contributing Code
-----------------

1. Create a new, dedicated branch for any task. We follow the naming
   convention:

    - *feat-<shortname>* for implementing and testing features
    - *bug-<shortname>* for bugfix branches
    - *sup-<shortname>* for support tasks (e.g. build system, documentation)

   For example, when fixing a bug in team allocation, you could name your
   branch *bug-team-alloc*.

2. Create a ticket in Redmine

    - State the name of your branch in the ticket description.
    - Set the ticket type (Support, Bug, or Feature)
    - Assign the ticket to yourself

   There is no need for time tracking, we use Redmine to maintain an overview
   of who is working on which component, and what branches are active.

3. For features and bugfixes, implement unit tests

4. Once you are finished with your implementation and unit tests:

    - Clone branch master into a new working copy:

          $ git clone git@git.dash-project.org:dash -b master ./dash-master

    - In the master working copy, pull from your branch:

          (dash-master)$ git pull origin feat-myfeature

    - Run continuous integration suite in the updated master working copy:

          (dash-master)$ ./dash/scripts/dash-ci.sh

    - If continuous integration passed, push to master:

          (dash-master)$ git push origin master

5. Reviewing

   After you merged your changes to master, choose a reviewer:

    - Edit the ticket in Redmine
    - Set state to "Resolved"
    - Set "Assigned to" to the team member that will review your code

   For now, we chose to merge to master before reviewing so everyone can
   contribute to master without depending on other team members.

6. Closing a ticket

   Tickets are closed by reviewers once the code changes in the ticket's
   branch passed review.

   Branches only be deleted when their ticket is closed.


Running Tests
-------------

Launch the DASH unit test suite using <code>dash-test-shmem</code> or
<code>dash-test-mpi</code>:

  (bin/dash/test/shmem)$ dartrun-shmem <dartrun options> dash-test-shmem <gtest options>

or

  (bin/dash/test/mpi)$ mpirun <MPI options> dash-test-mpi <gtest options>

For example, you would all unit tests of matrix data structures on 4 units
using the MPI runtime with:

  (bin/dash/test/mpi)$ mpirun -n 4 dash-test-mpi --gtest_filter=Matrix*

