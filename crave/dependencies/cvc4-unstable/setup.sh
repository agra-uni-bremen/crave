#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$package_dir is undefined'
  exit 1
fi

package=cvc4
version=unstable
source="latest-unstable.tar.gz"
build_dir=$build/$package-$version
url="http://cvc4.cs.stanford.edu/downloads/builds/src/unstable/$source"

unpack() {
  cd $cache &&
  src_dir=`tar --exclude="*/*" -tf $source` &&
  echo $src_dir &&
  tar -xf $source &&
  mv -f $src_dir $build_dir &&
  cd $build_dir
}

if [ -z "$BOOST_ROOT" ]; then
  dependencies="$DEPS_BOOST"
  boost_path="$root/$DEPS_BOOST"
else
  boost_path="$BOOST_ROOT"
fi

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi

  cd "$build_dir" &&
  # build libantlr3c with provided script
  contrib/get-antlr-3.4
  # build CVC4 with --bsd to allow usage under the terms of
  # the modified BSD license.
  ./configure --prefix="$target" --bsd --with-antlr-dir=$build_dir/antlr-3.4 --with-boost=$boost_path ANTLR=$build_dir/antlr-3.4/bin/antlr3 &&
  make -j $num_threads &&
  make install &&
  cp -f "$package_dir/CVC4Config.cmake" "$target/CVC4Config.cmake"
}

