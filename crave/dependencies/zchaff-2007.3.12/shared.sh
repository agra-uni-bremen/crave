#!/bin/sh


if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=zchaff
case "$ARCH" in
  i?86)   
    source="${package}.${version}.zip"
    export CFLAGS=-m32
    export CXXFLAGS=-m32
    ;;
  x86_64|x86-64)
    source="${package}.64bit.${version}.zip"; bits=64;;
  *) error "$package not avaiable for architechture $ARCH"; ;;
esac
build_dir=$build/$package-$version
url=http://www.princeton.edu/~chaff/zchaff/$source

unpack() {
  cd $cache &&
  unzip $source &&
  patch -p0 < $patches_dir/missing_includes.patch &&
  rm -rf $build_dir &&
  mv -f $cache/$package${bits:-""} $build_dir
}

pre_build() {
  cd $build_dir &&
  install_cmake_files $cmake_files_dir &&
  sed -i 3s/zChaff_VERSION/"zChaff_VERSION $version"/ $build_dir/CMakeLists.txt
}

build_install() {
  if [ -z "$target" ] ; then 
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  cmake_build_install
}

# vim: ts=2 sw=2 et
