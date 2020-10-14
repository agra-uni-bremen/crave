#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=minisat
source=$package-$version.tar.gz
build_dir=$build/$package-$version
url=http://minisat.se/downloads/$source

unpack() {
  cd $cache &&
  tar -xzf $source &&
  rm -rf $build_dir &&
  mkdir -p $build_dir &&
  mv -f $cache/$package $build_dir/$package
}

pre_build() {
  cd $build_dir &&
  install_cmake_files $cmake_files_dir
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  cmake_build_install
}

# vim: ts=2 sw=2 et
