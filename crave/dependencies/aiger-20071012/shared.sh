#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi


package=aiger

source=$package-$version.zip
build_dir=$build/$package-$version
url=http://fmv.jku.at/aiger/$source

unpack() {
  cd $cache &&
  unzip -o $source &&
  rm -rf build_dir &&
  mv -f $cache/$package-$version $build_dir
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
  mkdir -p build &&
  cd build &&
  cmake .. -DCMAKE_INSTALL_PREFIX=$target &&
  make install
}

# vim: ts=2 sw=2 et