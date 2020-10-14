#!/bin/sh

package=minisat
version=git
build_dir=$build/$package-$version
url=https://github.com/stp/minisat.git

unset -f download
download() {
  mkdir -p $build_dir &&
  cd $build_dir &&
  if [ -d .git ]; then
  	env GIT_SSL_NO_VERIFY=true git pull
  else
  	env GIT_SSL_NO_VERIFY=true git clone $url .
  fi
}

unpack() {
  true
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
    exit 1
  fi
  cd $build_dir &&
  cmake_build_install &&
  cp $package_dir/MiniSatConfig.cmake $target/
}

# vim: ts=2 sw=2 et
