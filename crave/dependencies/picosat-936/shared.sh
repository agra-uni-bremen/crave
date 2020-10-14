#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$package_dir is undefined'
  exit 1
fi


package=picosat
source=$package-$version.tar.gz
build_dir=$build/$package-$version
url=http://fmv.jku.at/$package/$source

unpack(){
  cd $cache &&
  tar -xf $source &&
  rm -rf $build_dir &&
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
  cmake_build_install
}

# vim: ts=2 sw=2 et
