#!/bin/bash

fn_distro(){
  arch=$(uname -m)
  kernel=$(uname -r)
  if [ -f /etc/lsb-release ]; then
          #os=$(lsb_release -s -d)
	  os=$(cat /etc/lsb-release | grep 'DISTRIB_DESCRIPTION')
  elif [ -f /etc/debian_version ]; then
          os="Debian $(cat /etc/debian_version)"
  elif [ -f /etc/redhat-release ]; then
          os=`cat /etc/redhat-release`
  else
          os="$(uname -s) $(uname -r)"
  fi
}

parse_ci_job_name() {
  echo $CI_JOB_NAME
  if [[ $CI_JOB_NAME == *"cvc4"* ]]; then
    REQUIRE_CVC4_DEPS="yes"
  fi
  if [[ $CI_JOB_NAME == *"z3"* ]]; then
    REQUIRE_Z3_DEPS="yes"
  fi
}

install-apt(){
  apt-get -qq update
  apt-get -qq -y upgrade
  apt-get install -qq -y build-essential cmake git wget unzip zlib1g-dev # essential
  apt-get install -qq -y bison flex # for STP
  apt-get install -qq -y gperf libgmp-dev # for Yices2 (gmp also needed by Z3 and CVC4)
  if [[ $REQUIRE_Z3_DEPS == "yes" ]]; then
    apt-get install -qq -y python-dev # for Z3
  fi
  if [[ $REQUIRE_CVC4_DEPS == "yes" ]]; then
    apt-get install -qq -y autoconf libtool $JDK_PACKAGE_NAME # for CVC4
  fi
}

install-yum(){
  yum -q update
  yum -q -y upgrade
  yum install -q -y make automake gcc gcc-c++ kernel-devel cmake git wget unzip zlib-devel bzip2 bzip2-libs patch # essential
  yum install -q -y bison flex # for STP
  yum install -q -y gperf libgmp-devel # for Yices2 (gmp also needed by Z3 and CVC4)
  if [[ $REQUIRE_Z3_DEPS == "yes" ]]; then
    yum install -q -y python-devel # for Z3
  fi
  if [[ $REQUIRE_CVC4_DEPS == "yes" ]]; then
    yum install -q -y autoconf libtool $JDK_PACKAGE_NAME # for CVC4
  fi
}

I=`basename $0`
echo $I version 1.1
fn_distro
echo $os

parse_ci_job_name

if [[ $os == *"Ubuntu"* ]]; then
  install-apt
elif [[ $os == *"CentOS"* ]]; then
  install-yum
else
  echo "No installation script can be found for this OS, try to use apt-get"
  install-apt
fi
