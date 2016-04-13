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

I=`basename $0`
echo $I version 1.0
fn_distro
echo $os
