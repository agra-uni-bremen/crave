#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=runlim
source="${package}-$version.tar.gz"
build_dir="$build/${package}-$version"
url="http://fmv.jku.at/runlim/$source"

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
  ./configure &&
  make -j$num_threads &&
  mkdir -p "$target" &&
  cp runlim "$target"
}

# vim: ts=2 sw=2 et

