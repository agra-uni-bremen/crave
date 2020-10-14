#!/bin/sh

if [ -z "$build" ] ; then 
  error '$build is undefined'
fi
if [ -z "$package_dir" ] ; then 
  error '$build is undefined'
fi

package=cmake
source=${package}-$version.tar.gz
build_dir=$build/${package}-$version
url="http://www.cmake.org/files/v${version:0:3}/$source"

unpack() {
  cd $cache &&
  tar -xf $source &&
  rm -rf $build_dir &&
  mv -f $cache/$package-$version $build_dir
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  ./configure --prefix=$target &&
  make -j$num_threads &&
  make install
}

# vim: ts=2 sw=2 et
