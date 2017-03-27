#!/bin/bash

apt-get update
apt-get -y upgrade
apt-get install -y build-essential cmake git wget unzip zlib1g-dev # essential
apt-get install -y bison flex # for STP
apt-get install -y python-dev libgmp-dev # for Z3
apt-get install -y gperf # for Yices2
apt-get install -y autoconf libtool openjdk-8-jdk # for CVC4
GIT_SSL_NO_VERIFY=true git submodule sync
GIT_SSL_NO_VERIFY=true git submodule update --init
