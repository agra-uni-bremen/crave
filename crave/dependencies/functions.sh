#!/bin/sh

# download() downloads the sources for the package.
download () {
  if [ -z $url ] || [ -z $source ]; then
    error "no url or source is defined"
  fi
  if ! [ ${url:0:7} == http:// ] && ! [ ${url:0:6} == ftp:// ] && ! [ ${url:0:8} == https:// ]; then
    error "automatic downloading is only supported for http(s) and ftp urls."
  fi

  cd $cache
  download_http $source "$url"
  cd -
}

# unpack() defines the routine for unpack the package.
unpack() {
  false
}

# pre_build() prepare the package to complie properly.
pre_build() {
  true
}

# build_install() executes the compile-process and afterwards install the package.
build_install() {
  false
}

# vim: ts=2 sw=2 et