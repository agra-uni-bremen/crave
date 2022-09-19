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
scripts/show_gits_summary.sh
echo
scripts/show_linux_distribution.sh
echo
echo "Environment variables"
echo "CC: $CC"
echo "CXX: $CXX"
$CXX --version
echo "#################################################################"
echo

CI_SKIP=`git log -1 --oneline | grep "[ci skip]"`

if [ -n "$CI_SKIP" ]; then
  echo "skip build"
  exit
fi

showRegionBegin "Build crave"

pwd
rm build -Rf # clean
echo "make:"
make

showRegionEnd "Build crave"

showRegionBegin "make test"

cd build
pwd
echo "make test:"
make test

showRegionEnd "make test"


