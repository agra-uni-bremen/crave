#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=SWORD
bits=32

case "$ARCH" in
  i?86)   source="sword-${version}-32bit.tar.gz"; bits=32;;
  x86_64|x86-64) source="sword-${version}-64bit.tar.gz"; bits=64;;
  *) error "$package not avaiable for architechture $ARCH"; ;;
esac

build_dir=$build/${source/.tar.gz}
url=http://www.informatik.uni-bremen.de/agra/doc/software/$source

unpack() {
  cd $cache &&
  tar -xf $source &&
  rm -rf $build_dir &&
  mv -f $cache/sword-$version-${bits}bit $build_dir
}

pre_build() {
  cd $build_dir &&
  install_cmake_files $cmake_files_dir &&
  sed -i 1s/SWORD_VERSION/"SWORD_VERSION $version"/ $build_dir/SWORDConfig.cmake
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  mkdir -p $target &&
  cp -r $build_dir/* $target 
}

# vim: ts=2 sw=2 et
