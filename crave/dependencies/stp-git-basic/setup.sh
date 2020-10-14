#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=stp
version=git-basic
source=nosourcefile
build_dir=$build/$package-$version
url=https://github.com/stp/stp.git

dependencies="minisat-git"

download() {
  mkdir -p $build_dir &&
  cd $build_dir &&
  if [ -d .git ]; then
    git pull
  else
    git clone $url .
  fi
  git reset --hard HEAD
}

unset -f unpack
unpack() {
  true
}

pre_build() {
  true
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi
  cd "$build_dir" &&
  export CMAKE_PREFIX_PATH="$root/minisat-git" &&
  cmake_build_install .. -DENABLE_PYTHON_INTERFACE=off -DONLY_SIMPLE=on -DNOCRYPTOMINISAT=on
}
