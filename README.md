 Constrained RAndom Verification Enviroment - CRAVE
====================================================

For feedback and bug reports: crave@informatik.uni-bremen.de

 Preparations
--------------

When installing directly from the git repository some additional steps are
required. These are not necessary when using the packaged source code. Both
require some packages to be installed on the system.

To get CRAVE from the git repository execute the following commands:

    git clone --recursive https://github.com/agra-uni-bremen/crave-bundle.git

This will download crave and its direct dependencies. External dependencies
will be downloaded by the bootstrap script when executing make.

Please install the following packages to ensure the external packages will
build correctly:

* zlib and bzip2 development libraries (e.g. zlib1g-dev, libbz2-dev)
* CMake
* Python development libraries


 Editing CRAVE's Makefile
-------------------------

For now, the following line from Makefile is the only one you need to pay attention to:

    ./bootstrap.sh -d ${SRCDIR}/deps --systemc -b cudd -b sword -b z3 -b boolector ${BUILD} ${BOOSTRAP_ARGS}

The editable options are '--systemc', '-b cudd', '-b sword', '-b z3' and '-b boolector', which will be
described in the following.


 SystemC Support
-----------------

You need to set the environment variable SYSTEMC_HOME to your SystemC installation 
directory (which also must contains a static library). Otherwise, CRAVE will automatically 
download and build systemc-2.3 from https://github.com/systemc/systemc-2.3.
It is also possible to build CRAVE without SystemC support by removing the option '--systemc',
but this is not recommended.


 Other external dependencies
-----------------------

CRAVE uses the following external dependencies:

* [Boost] - you need at least version 1.50.0, set the environment variable BOOST_ROOT to your 
boost installation directory (e.g. /usr) or CRAVE will automatically download and build boost-1.50.0. 
For faster build, it is recommended to choose the first option.
* [Glog] - similarly, set GLOG_ROOT or CRAVE will automatically download and build glog.
* [CUDD] - CUDD can be disabled by removing the option '-b cudd', but this is not recommended.
* [SWORD] - SWORD can be disabled by removing the option '-b sword'.
* [Z3] - Z3 can be disabled by removing the option '-b z3'. Building Z3 takes considerable time so
you might want to disable Z3 for faster build.
* [Boolector] - Boolector can be disabled by removing the option '-b boolector'.

Please make sure that at least one SMT backend ([SWORD], [Z3] or [Boolector]) is enabled.


 Build and install CRAVE
-------------------------

1. Call 'make' to build CRAVE, the dependencies should
   automatically download and build.
2. Call 'make test', all tests should pass (with very few execeptions).
3. Call 'make install'.

To clean CRAVE you can delete the build directory and call 'make' again.

 Examples
----------

You will find several examples in the examples/ directory. These will
demonstrate how to build applications that use CRAVE as well as some core
features of CRAVE as listed below:

  * ex1_seed_dist              : global seed and simple distribution.
  * ex2_constr_ref             : constraints and references.
  * ex3_inh_soft_next          : constraint inheritance, soft constraints and custom next().
  * ex4_constr_mng             : enable/disable constraints.
  * ex5_vec_constr             : vector constraints.
  * ex6_inline_gen             : inline generator.
  * ex7_randv_enum             : constraints on enumeration types.
  * ex8_make_expression        : expression layer usage.
  * ex9_experimental           : experimental features.

to execute these examples look into the build/examples directory.
See also the test cases in crave/tests which cover all implemented features.


 Configuration file
-----------------------

CRAVE will try to find and if not found, create a configuration file crave.cfg in 
the executing directory. The default configuration is shown in the following:

    <crave>
    	<backend>auto</backend>
    	<seed>0</seed>
    </crave>
    <logger>
    	<filename>crave</filename>
    	<directory>./logs</directory>
    	<level>0</level>
    	<filesize>100</filesize>
    </logger>

The value 'auto' means that CRAVE will automatically select a backend among 
the available SMT solvers. Other possible values are 'boolector', 'z3' and 'sword'.
Change the seed to a positive integer, if you want to use this integer as the fixed seed.


 Known issues
--------------

In the default configuration CRAVE will use SWORD as one of its SMT backends.
SWORD is provided as binary program for Linux x86 and x86_64 platforms. However,
this version of SWORD is incompatible with RHEL 5 or derived operating systems (e.g. 
CentOS 5). On such systems, SWORD must be disabled before building CRAVE by editing
'-b sword' out of the Makefile.

[Boost]: http://www.boost.org
[CUDD]:  http://vlsi.colorado.edu/~fabio/CUDD/
[SWORD]: http://www.informatik.uni-bremen.de/agra/eng/sword.php
[Z3]: https://github.com/Z3Prover/z3
[Boolector]: http://fmv.jku.at/boolector/
[Glog]: https://code.google.com/p/google-glog/
