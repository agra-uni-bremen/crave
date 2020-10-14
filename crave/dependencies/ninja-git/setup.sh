#!/bin/sh

version=git

source $base_dir/ninja-release/shared.sh

unset -f download
download() {
  mkdir -p $build_dir &&
  cd $build_dir &&
  if [ -d .git ]; then
    git pull
  else
    git clone $url .
  fi
}

# vim: ts=2 sw=2 et
