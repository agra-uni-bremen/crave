#!/bin/sh

if [ -z "$build" ] ; then
  error '$build is undefined'
fi

package=puma
source="$package-$version.tar.gz"
build_dir=$build/$package-$version
url="http://www.informatik.uni-bremen.de/revkit/files/$source"

unpack() {
  cd $cache &&
  message "unpacking $package" &&
  tar -xf $source &&
  mv -f $cache/$package $build_dir &&
  message "finished unpacking $package"
}

build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
  exit 1
  fi

  # target directories
  mkdir -p $target/{include,lib}

  # build
  cd "$build_dir/bin" &&
  make -j$num_threads puma &&
  cd .. &&
  cp include/*.h $target/include &&
  cp lib/lib_puma.a $target/lib &&
  cd ..
}

# vim: ts=2 sw=2 et