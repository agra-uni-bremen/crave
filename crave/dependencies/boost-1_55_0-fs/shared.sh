#!/bin/sh

if [ -z "$build" ] ; then
  error '$build is undefined'
fi

package=boost
source=${package}_$version.tar.bz2
build_dir=$build/${package}_$version
url=http://sourceforge.net/projects/boost/files/boost/${version//_/.}/$source/download

unpack() {
  cd $cache &&
  message "unpacking $package" &&
  tar -xf $source &&
  message "finished unpacking $package"
}

pre_build() {
  rm -rf $build_dir &&
  mv -f $cache/${package}_$version $build_dir
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
    exit 1
  fi
  COMMON_OPTS="
    --prefix=$target
    --layout=system
    variant=release
    link=shared
    toolset=gcc
  "
  LIBRARIES="
    --with-filesystem
    --with-system
  "
  cd $build_dir &&
  test -x bjam || ./bootstrap.sh &&
  ./bjam -q $COMMON_OPTS $LIBRARIES -j$num_threads install || {
    local needed="false"
    if [ ! -f /usr/include/zlib.h ] ; then
      echo 'zlib.h was not found.'
      needed="true"
    fi
    if [ ! -f /usr/include/bzlib.h ] ; then
      echo 'bzlib.h was not found'
      needed="true"
    fi
    if [ "$needed" = "true" ] ; then
      echo "Install the packages containing the above header files to compile boost properly."
    fi
    exit 1
  }
}

