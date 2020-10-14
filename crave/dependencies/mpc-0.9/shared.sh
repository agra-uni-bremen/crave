#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=mpc
source="$package-$version.tar.gz"
build_dir=$build/$package-$version
url="http://www.multiprecision.org/mpc/download/$source"

dependencies="gmp-$version_gmp mpfr-$version_mpfr"

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

  cd "$build_dir" &&
  ./configure --prefix=$target --with-gmp="$root/gmp-$version_gmp" --with-mpfr="$root/mpfr-$version_mpfr" &&
  make &&
  make install
}

