#!/bin/sh

version=2.2.0

dependencies="lingeling-ayv-86bf266-140429 minisat-git"

patches_dir=$base_dir/boolector-$version
cmake_files_dir=$base_dir/boolector-$version

source $base_dir/boolector-$version/shared.sh
