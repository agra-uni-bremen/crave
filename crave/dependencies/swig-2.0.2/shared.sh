#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=swig
source=${package}-$version.tar.gz
build_dir=$build/${package}-$version
url=http://ovh.dl.sourceforge.net/project/${package}/${package}/${package}-${version}/$source

unset -f download
download() {
  cd $cache &&
  wget -c -O $source $url &&
  wget -c ftp://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-${pcre_version}.tar.gz &&
  cd -
}

unpack() {
  cd $cache &&
  tar -xf $source &&
  rm -rf $build_dir &&
  mv -f $cache/$package-$version $build_dir
}

pre_build() {
  cd $build_dir &&
  mkdir -p build &&
  cd build &&
  cp $cache/pcre-${pcre_version}.tar.gz .
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  COMMON_OPTS="
    --prefix=$target
  "
  cd $build_dir/build &&
  ../Tools/pcre-build.sh &&
  ../configure $COMMON_OPTS &&
  make -j$num_threads &&
  make install
}

# vim: ts=2 sw=2 et
