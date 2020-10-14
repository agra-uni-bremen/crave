# CRAVE2UVM prototype

This projects contains a prototypical integration of CRAVE with UVM-SystemC to provide access to constrained randomization and coverage.
For demonstration, we ported the SystemVerilog example UBUS to UVM-SystemC and included it in this distribution.

## Requirements

Make sure all pre install requirements of CRAVE and UVM-SystemC are met:

* CMake (at least v2.8.9)
* GNU Make
* g++ (at least v4.7.2)
* SystemC (the environment variable SYSTEMC_HOME must be set accordingly)
* Boost (at least v1.50.0 and the environment variable BOOST_ROOT must be set accordingly)
* zlib development libraries (e.g. zlib1g-dev).

The version of CRAVE included in this distribution by default will build a minimal configuration (Glog and 2 solver backends: CUDD and Z3). 
Other configurations with additional backends (e.g. Boolector, SWORD, CVC4, etc.) are also possible. 
If download is permitted, CRAVE can automatically download and build these backends.
Otherwise, the user needs to provide appropriate source or binary archive in deps/cache.
For more detailed instructions , please refer to the CRAVE README or contact us.

## Installation

To install and run the example, use the buildscript on the toplevel of this repository. It currently executes three common tasks:

1. ./buildscript install: this will install CRAVE (shipped with this distribution) into ./crave, locally compiles UVM-SystemC into ../.. and setup the Makefile for the example "Ubus".

2. ./buildscript compile: this will compile the example and put the executables into build/bin.

3. ./buildscript run: this will run the Ubus example.

## Tested OS

This distribution has been tested on the following 64-bit Linux (x86_64) systems:

* Fedora 22 with g++-5.3.1
* Ubuntu 14.04.4 LTS with g++-4.8.4
* Debian 7.0 with g++-4.7.2

## Contact
For bugreport and feedback: crave@systemc-verification.org

