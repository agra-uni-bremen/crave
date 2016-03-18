#!/bin/bash

#IMPORTANT to catch all errors
set -e

I=`basename $0`
echo "$I version 0.1, 2016-02-01, 16:49"
echo "*********************************************************"
echo


### some helper functions

showRegionBegin() {
echo "#################################################################"
echo "## BEGIN: $1"
}


showRegionEnd() {
echo "## END: $1"
echo "#################################################################"
}

### some helper functions end


export CC=gcc
export CXX=g++

#export HOMEDIR=`pwd`
echo "Check HOMEDIR: $HOMEDIR"
if [ -z "$HOMEDIR" ]; then
    echo "Error: Define HOMEDIR !!"
    exit 1
fi
if ! [ -d "$HOMEDIR" ];then
    echo "Error: $HOMEDIR not found!"
    exit 1
fi
echo "ok"

#set +x  # echo off
echo
echo "#################################################################"
echo "Information:"
echo
./show_gits_summary.sh
echo
./show_linux_distribution.sh
echo 
echo "Environment variables"
echo "CC: $CC"
echo "CXX: $CXX"
$CXX --version
echo "#################################################################"
echo

#GBR=`git branch | grep '*' | sed 's/* //'`
#CRAVE_BUILD=build-${GBR}
CRAVE_BUILD=build
#git checkout Makefile
#sed -i "s/build/${CRAVE_BUILD}/" Makefile

showRegionBegin "${CRAVE_BUILD}"

pwd
rm build* -Rf # clean
echo "make:"
make

showRegionEnd "${CRAVE_BUILD}"

showRegionBegin "make test"

cd ${CRAVE_BUILD}
pwd
echo "make test:"
make test

showRegionEnd "make test"


