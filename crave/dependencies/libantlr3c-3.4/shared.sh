#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=libantlr3c
source="$package-$version.tar.gz"
build_dir=$build/$package-$version
url="http://www.antlr3.org/download/C/$source"

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
  ./configure --prefix="$target" --disable-abiflags &&
  make &&
  make install
}

