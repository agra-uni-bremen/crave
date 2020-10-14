#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=mpfr
source="$package-$version.tar.bz2"
build_dir=$build/$package-$version
url="http://www.mpfr.org/mpfr-current/$source"

dependencies="gmp-$version_gmp"

unpack() {
  cd $cache &&
  tar -xf $source &&
  mv -f $package-$version $build_dir
}

pre_build() {
  true
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi

  # build mpfr
  cd "$build_dir" &&
  CFLAGS="-O2" ./configure --prefix=$target --with-gmp="$root/gmp-$version_gmp" &&
  make -j$num_threads &&
  make install
}
