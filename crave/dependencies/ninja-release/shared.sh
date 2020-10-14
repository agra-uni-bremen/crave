#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$build is undefined'
  exit 1
fi

package=ninja
source=nosourcefile
build_dir=$build/$package-$version
url=git://github.com/martine/ninja.git

download() {
  mkdir -p $build_dir &&
  cd $build_dir &&
  if [ -d .git ]; then
    git pull
  else
    git clone $url . -b $version
  fi
}

unpack() {
  true
}

build_install() {
  if [ -z "$target" ] ; then
    error '$target is undefined'
  fi
  cd "$build_dir" &&
  python bootstrap.py &&
  mkdir -p "$target/bin"
  cp "$package" "$target/bin"
}

# vim: ts=2 sw=2 et
