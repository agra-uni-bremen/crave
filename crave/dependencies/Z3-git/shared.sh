#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$build is undefined'
  exit 1
fi

package=Z3
source=nosourcefile
build_dir=$build/$package-$version
url='https://github.com/Z3Prover/z3.git'

download() {
  mkdir -p $cache/$package-$version &&
  cd $cache/$package-$version &&
  if [ -d .git ]; then
    git pull
  else
    git clone $url .
    git checkout $branch
  fi
}

unpack() {
  cp -R $cache/$package-$version $build_dir
}
  
pre_build() {
  true
}
 
build_install() {
  if [ -z "$target" ] ; then
    echo '$target is undefined'
    exit 1
  fi
  cd "$build_dir" &&
  ./configure --staticlib --prefix="$target" &&
  cd build &&
  make -j $num_threads &&
  make install &&
  install_cmake_files $cmake_files_dir &&
  cp Z3Config.cmake "$target"
}
