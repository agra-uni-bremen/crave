#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=llvm+clang
build_dir=$build/$package-$version.build
source_dir=$cache/llvm-${version}.src
url_llvm="http://llvm.org/releases/$version/$source_llvm"
url_clang="http://llvm.org/releases/$version/$source_clang"

unset -f download
download () {
  cd $cache
  download_http $source_llvm $url_llvm &&
  download_http $source_clang $url_clang &&
  cd -
}

unpack(){
  cd $cache &&
  tar -xf $source_llvm &&
  tar -xf $source_clang &&
  mv cfe-$version.src $source_dir/tools/clang &&
  rm -rf $build_dir &&
  mv -f $source_dir $build_dir
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi
  cd $build_dir &&
  cmake_build_install &&
  ## install scan-build tools
  true
}

# vim: ts=2 sw=2 et
