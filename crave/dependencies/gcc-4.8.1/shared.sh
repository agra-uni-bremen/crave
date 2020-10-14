#!/bin/sh

if [ -z "$build" ] ; then
  echo '$build is undefined'
  exit 1
fi
if [ -z "$package_dir" ] ; then
  echo '$package_dir is undefined'
  exit 1
fi

package=gcc
source="gcc-$version.tar.bz2"
build_dir=$build/$package-$version
url="http://ftp.gwdg.de/pub/misc/gcc/releases/$package-$version/$source"

MPFR="mpfr-${version_mpfr}"
GMP="gmp-${version_gmp}"
MPC="mpc-${version_mpc}"

MPFR_URL="ftp://gcc.gnu.org/pub/gcc/infrastructure/$MPFR.tar.bz2"
GMP_URL="ftp://gcc.gnu.org/pub/gcc/infrastructure/$GMP.tar.bz2"
MPC_URL="ftp://gcc.gnu.org/pub/gcc/infrastructure/$MPC.tar.gz"

download () {
  cd $cache &&
  download_http $source "$url" &&
  download_http "$MPFR.tar.bz2" "$MPFR_URL" &&
  download_http "$GMP.tar.bz2" "$GMP_URL" &&
  download_http "$MPC.tar.gz" "$MPC_URL" &&
  cd -
}

unpack() {
  cd $cache &&
  tar -xf $source &&
  rm -rf $build_dir &&
  mv -f $package-$version $build_dir &&

  tar -xf "${MPFR}.tar.bz2" &&
  mv -f "${MPFR}" "${build_dir}/mpfr" &&

  tar -xf "${GMP}.tar.bz2" &&
  mv -f "${GMP}" "${build_dir}/gmp" &&

  tar -xf "${MPC}.tar.gz" &&
  mv -f "${MPC}" "${build_dir}/mpc"
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
  GCCMACHINE=`gcc -dumpmachine`
  GCCVERSION=`gcc -dumpversion`
  export CFLAGS="-O2 -I/usr/include/$GCCMACHINE"
  export LDFLAGS="-O2 -B/usr/lib/$GCCMACHINE -L/usr/lib/$GCCMACHINE"
  export LIBRARY_PATH=/usr/lib/$GCCMACHINE
  #make distclean
  local OPTS="
    --prefix=$target
    --disable-multilib
    --enable-languages=c,c++
  "
  echo ./configure $OPTS  &&
  ./configure $OPTS  &&
  make -j5$num_threads &&
  make install
}

