#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=gmp
source="$package-$version.tar.bz2"
build_dir=$build/$package-$version
url="ftp://ftp.gmplib.org/pub/$package-$version/$source"

unpack(){
  cd $cache &&
  message "unpacking $package" &&
  tar -xf $source &&
  message "finished unpacking $package" &&
  rm -rf $build_dir &&
  mv -f $cache/$package-$version $build_dir
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi

  # build mpfr
  cd "$build_dir" &&
  mkdir -p build &&
  cd build &&
  ../configure --prefix=$target &&
  make -j$num_threads &&
  make install
}

# vim: ts=2 sw=2 et
