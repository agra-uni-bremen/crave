#!/bin/sh

if [ -z "$build" ] ; then
  error '$build is undefined'
fi

package=Python
source=${package}-$version.tar.xz
build_dir=$build/${package}-$version
url=https://www.python.org/ftp/python/$version/$source

unpack() {
  cd $cache &&
  message "unpacking $package" &&
  tar -Jxvf $source &&
  message "finished unpacking $package"
}

pre_build() {
  rm -rf $build_dir &&
  mv -f $cache/${package}-$version $build_dir
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  ./configure --prefix=$target &&
  make -j$num_threads &&
  make -j$num_threads install
}

# vim: ts=2 sw=2 et
