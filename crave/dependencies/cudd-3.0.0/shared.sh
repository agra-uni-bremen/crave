#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=cudd
source=$package-$version.tar.gz
build_dir=$build/$package-$version
url='https://github.com/nbruns1/cudd.git'

download() {
  mkdir -p $cache/$package-$version &&
  cd $cache/$package-$version &&
  if [ -d .git ]; then
    git checkout $branch
  else
    git clone $url .
    git checkout $branch
  fi
}

unpack() {
  cp -R $cache/$package-$version $build_dir
}

pre_build() {
  true
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  touch configure.ac aclocal.m4 configure Makefile.am Makefile.in &&
  mkdir -p build &&
  cd build &&
  ../configure --enable-obj --enable-dddmp --prefix=$target &&
  make -j$num_threads install &&
  install_cmake_files $cmake_files_dir &&
  cp CUDDConfig.cmake "$target"
}

# vim: ts=2 sw=2 et
