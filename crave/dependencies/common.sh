
error() {
  echo "ERROR: $@" >&2
  echo "TERMINATING" >&2
  exit 1
}

message() {
  echo "$@"
}

run_scripts() {
  cd $base_dir
  package_dir=$base_dir/$p
  source functions.sh
  source $p/setup.sh
  for s in $@; do
    echo "calling $s for $p"
    $s || error "$s failed for $p"
  done
}

with_deps() {
  build="-"
  local i
  local dependencies
  for i in $@; do
    package_dir=$i
    source $package_dir/setup.sh
    with_deps $dependencies
    echo $i
  done
}


download_http() {
  url="$2"
  name="$1"
  if type -p wget &>/dev/null; then
    wget -c -O "$name" "$url"
  elif type -p curl &>/dev/null; then
    curl -C - -o "$name"  "$url"
  else 
    error "no tool for http download found"
  fi
}

download_git() {
  dir="$1"
  url="$2"
  branch="$3"

  mkdir -p $dir &&
  cd $dir &&
  if [ -d .git ]; then
    git set-url origin "$url"
    git fetch origin
  else
    git clone -b "$branch" "$url" .
  fi
  git reset --hard "origin/$branch"
}


#
# install all CMakeLists.txt files from the package to the current folder.
# includes subfolders and keeps hierarchy
#
# currently all files are symlinked.
#
install_cmake_files() {
  find ${1:-$package_dir} -name CMakeLists.txt -o -name "*.cmake"| while read f
  do
    ln -sf $f $(echo "$f" |sed "s@^${1:-$package_dir}/*@@")
  done
}

cmake_build_install() {
  local src=${1:-..}
  shift
  mkdir -p build &&
  cd build &&

  echo cmake -DCMAKE_INSTALL_PREFIX=$target -DCMAKE_BUILD_TYPE=${BUILD_TYPE} $@ ${src} &&
  cmake -DCMAKE_INSTALL_PREFIX=$target -DCMAKE_BUILD_TYPE=${BUILD_TYPE} $@ ${src} &&
  make -j$num_threads install
}

setup_environment() {
  ARCH=${ARCH:-$(uname -m)}
  duplicate=${duplicate:-skip}
  BUILD_TYPE=${BUILD_TYPE:-RELEASE}

  case "$ARCH" in
    i?86)   
      export CFLAGS="-m32 $CFLAGS"
      export CXXFLAGS="-m32 $CXXFLAGS"
      ;;
  esac
}

setup_environment


# vim: ts=2 sw=2 et
