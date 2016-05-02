#!/bin/bash



function getGitInfo {
  HERE=`pwd`
  cd $1
  echo -n "Directory: "
  pwd
  GURL=`git config --get remote.origin.url`
  LCH=`git log -1 --format="%H"`
  GBR=`git branch | grep '*'`
  echo " Git url: $GURL" 
  echo " Git branch: $GBR" 
  echo " Last commit hash: $LCH"
  echo 
  cd $HERE
}


I=`basename $0`
echo $I version 1.0, 2016-02-01
echo

getGitInfo .
getGitInfo examples
getGitInfo dependencies
getGitInfo metaSMT

