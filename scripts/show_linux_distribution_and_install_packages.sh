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

install-apt(){
  apt-get -qq update
  apt-get -qq -y upgrade
  apt-get install -qq -y build-essential cmake git wget unzip zlib1g-dev # essential
  apt-get install -qq -y bison flex # for STP
  apt-get install -qq -y python-dev libgmp-dev # for Z3
  apt-get install -qq -y gperf # for Yices2
  apt-get install -qq -y autoconf libtool openjdk-8-jdk # for CVC4
}

install-yum(){
  yum -q update
  yum -q -y upgrade
  yum install -q -y make automake gcc gcc-c++ kernel-devel cmake git wget unzip zlib-devel bzip2 bzip2-libs patch # essential
  yum install -q -y bison flex # for STP
  yum install -q -y python-devel libgmp-devel # for Z3
  yum install -q -y gperf # for Yices2
  yum install -q -y autoconf libtool openjdk-8-jdk # for CVC4
}

I=`basename $0`
echo $I version 1.1
fn_distro
echo $os

if [[ $os == *"Ubuntu"* ]]; then
  install-apt
elif [[ $os == *"CentOS"* ]]; then
  install-yum
else
  echo "No installation script can be found for this OS, try to use apt-get"
  install-apt
fi
