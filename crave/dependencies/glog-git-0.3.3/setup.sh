#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

version=git-0.3.3
package=glog
source=nosourcefile
branch=v0.3.3
build_dir=$build/$package-$version
url=https://github.com/google/glog.git

download() {
  mkdir -p $build_dir &&
  cd $build_dir &&
  if [ -d .git ]; then
    git pull
  else
    git clone -b $branch $url .
  fi
  git reset --hard HEAD
}

unpack() {
  true
}

build_install() {
  if [ -z "$target" ] ; then
    error '$target is undefined'
  fi
  cd "$build_dir" &&

  ./autogen.sh && ./configure --prefix="$target" &&
  make -j && make install
}

# vim: ts=2 sw=2 et
