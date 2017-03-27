#!/bin/bash

yum update
yum -y upgrade
yum install -y make automake gcc gcc-c++ kernel-devel cmake git wget unzip zlib bzip2 bzip2-libs # essential
yum install -y bison flex # for STP
yum install -y python-devel libgmp-devel # for Z3
yum install -y gperf # for Yices2
yum install -y autoconf libtool openjdk-8-jdk # for CVC4
GIT_SSL_NO_VERIFY=true git submodule sync
GIT_SSL_NO_VERIFY=true git submodule update --init
