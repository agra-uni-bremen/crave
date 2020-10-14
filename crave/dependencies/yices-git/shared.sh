#!/bin/sh

if [ -z "$build" ] ; then 
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then 
  echo '$package_dir is undefined'
  exit 1
fi

package=yices
source=nosourcefile
build_dir=$build/$package-$version
url='https://github.com/SRI-CSL/yices2'

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
  cd $build_dir &&
  autoconf &&
  ./configure --prefix="$target" &&
  make &&
  make install &&
  cp $config_files_dir/yices2-config.cmake "$target"
}

