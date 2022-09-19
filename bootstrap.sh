#!/bin/bash
SRC_DIR=$( cd $(dirname $0) && pwd)

BUILD_DIR=$PWD/build
DEPS=$PWD/deps

BOOST=boost-1_55_0-fs

GLOG=glog-git-0.3.3
SYSTEMC=systemc-2.3.1

BOOLECTOR=boolector-2.2.0
CVC4=cvc4-unstable
STP=stp-git-basic
SWORD=SWORD-1.1
YICES2=yices-2.5.1
Z3=Z3-4.4.1

MINISAT=minisat-git
LINGELING=lingeling-ayv-86bf266-140429

CUDD=cudd-3.0.0

CMAKE=cmake
BUILD_CMAKE="no"
CMAKE_PACKAGE=cmake-2.8.4

CMAKE_ARGS="
  -DmetaSMT_ENABLE_TESTS=off
"


usage() {
  cat << EOF
$0 sets up a CRAVE build folder.
usage: $0 [--free] [--non-free] build
  --boost=/p/t/b    use this version of the Boost libraries
  --backend val     use the backend val, following backends can be
   -b val           activated: boolector, cvc4, stp, sword, yices2, z3 and cudd
  --systemc         build SystemC
  --systemc=/p/t/s  use this version of SystemC
  --clean           delete build folder before creating a new one
  --deps <dir>      build dependencies in this folder
   -d <dir>         can be shared in different projects
  --download-only   Only download dependencies but not build. Might override other options.
  --install <dir>   configure cmake to install to this directory
   -i <dir>
  --mode <type>     the CMake build type to configure, types are
   -m <type>        RELEASE, MINSIZEREL, RELWITHDEBINFO, DEBUG
   -Dvar=val        pass options to cmake
  --cmake=/p/t/c    use this version of cmake
  --cmake           build a custom cmake version
  --cache           specify the path to the sources, if a
   -ca              download is not wanted
   -j <num>         number of threads to compile the dependencies
  <build>           the folder to setup the build environment in
EOF
  exit 1
}

if ! [[ "$1" ]]; then
  usage
fi


while [[ "$@" ]]; do
  case $1 in
    --usage|--help|-h) usage;;
    --boost=*)    BOOST=""; BOOST_ROOT="${1#--boost=}";;
    --backend|-b) BACKENDS="$2 $BACKENDS"; shift;;
    --systemc=*)  WITH_SYSTEMC="yes"; SYSTEMC_HOME="${1#--systemc=}";;
    --systemc)    WITH_SYSTEMC="yes";;
    --deps|-d)    DEPS="$2"; shift;;
    --install|-i) INSTALL="$2"; shift;;
    --mode|-m)    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_BUILD_TYPE=$2"; shift;;
     -D*)         CMAKE_ARGS="$CMAKE_ARGS $1";;
    --download-only) DL_ONLY=-d;;
    --clean|-c)   CLEAN="true";;
    --cmake=*)    CMAKE="${1#--cmake=}";;
    --cmake)      BUILD_CMAKE="yes";;
    --cache|-ca)  CACHE="$2"; shift;;
     -j)          NUM_THREADS=$2; shift;;
             *)   ## assume build dir
                  BUILD_DIR="$1" ;;
  esac
  shift;
done

if [[ "$CLEAN" ]]; then
  rm -rf $BUILD_DIR
fi

BACKENDS="${BACKENDS:-boolector cvc4 stp sword yices2 z3 cudd}"
echo "activated backends are: $BACKENDS"

if [[ "$CRAVE_OFFLINE_BUILD" = "ON" ]]; then
  # Check variables
  echo "Offline build activated."
  if [[ -z "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT must be set for offline build."
    exit 1;
  fi
  if [[ -z "$SYSTEMC_HOME" ]]; then
    echo "SYSTEMC_HOME must be set for offline build."
    exit 1;
  fi
  if [ "$BUILD_CMAKE" = "yes" ]; then
    echo "Building cmake in offline build is not allowed."
    exit 1;
  fi

  if ! [ -f crave_package_deps.tar.gz ]; then
    echo "Could not find the dependency cache crave_package_deps.tar.gz. Please provide crave_package_deps.tar.gz or switch to online build"
    exit 1;
  fi

  mkdir -p $DEPS
  tar --strip-components=1 -C $DEPS -xzf crave_package_deps.tar.gz
  CACHE=$DEPS
else
  echo "Regular build activated. Dependencies will be downloaded and built automatically."
  echo "Offline build can be activated by setting the environment variable CRAVE_OFFLINE_BUILD to ON."
fi

DIS_BOOLECTOR="yes"
DIS_CVC4="yes"
DIS_STP="yes"
DIS_SWORD="yes"
DIS_YICES2="yes"
DIS_Z3="yes"

DIS_CUDD="yes"

for BACKEND in $BACKENDS
do
  if [[ "$BACKEND" = "boolector" ]]; then
    REQUIRES="$LINGELING $BOOLECTOR $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_Boolector=on $CMAKE_ARGS"
    DIS_BOOLECTOR="no"
  fi
  if [[ "$BACKEND" = "cvc4" ]]; then
    REQUIRES="$CVC4 $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_CVC4=on $CMAKE_ARGS"
    DIS_CVC4="no"
  fi
  if [[ "$BACKEND" = "stp" ]]; then
    REQUIRES="$STP $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_STP=on $CMAKE_ARGS"
    DIS_STP="no"
  fi
  if [[ "$BACKEND" = "sword" ]]; then
    REQUIRES="$SWORD $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_SWORD=on $CMAKE_ARGS"
    DIS_SWORD="no"
  fi
  if [[ "$BACKEND" = "yices2" ]]; then
    REQUIRES="$YICES2 $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_YICES2=on $CMAKE_ARGS"
    DIS_YICES2="no"
  fi
  if [[ "$BACKEND" = "z3" ]]; then
    REQUIRES="$Z3 $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_Z3=on $CMAKE_ARGS"
    DIS_Z3="no"
  fi

  if [[ "$BACKEND" = "cudd" ]]; then
    REQUIRES="$CUDD $REQUIRES"
    CMAKE_ARGS="-DmetaSMT_USE_CUDD=on $CMAKE_ARGS"
    DIS_CUDD="no"
  fi
done

if [[ "$DIS_BOOLECTOR" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_Boolector=off $CMAKE_ARGS"
fi
if [[ "$DIS_CVC4" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_CVC4=off $CMAKE_ARGS"
fi
if [[ "$DIS_STP" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_STP=off $CMAKE_ARGS"
fi
if [[ "$DIS_SWORD" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_SWORD=off $CMAKE_ARGS"
fi
if [[ "$DIS_YICES2" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_YICES2=off $CMAKE_ARGS"
fi
if [[ "$DIS_Z3" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_Z3=off $CMAKE_ARGS"
fi
if [[ "$DIS_CUDD" = "yes" ]]; then
  CMAKE_ARGS="-DmetaSMT_USE_CUDD=off $CMAKE_ARGS"
fi

mk_and_abs_dir() {
  mkdir -p $1 &&
  cd $1 &>/dev/null &&
  pwd
}
BUILD_DIR=$(mk_and_abs_dir $BUILD_DIR) &&
INSTALL=$(mk_and_abs_dir ${INSTALL:-$BUILD_DIR/root}) &&
DEPS=$(mk_and_abs_dir ${DEPS:-$BUILD_DIR}) &&

if [ "$WITH_SYSTEMC" = "yes" ]; then
  if [ "$SYSTEMC_HOME" = "" ]; then
    REQUIRES="$SYSTEMC $REQUIRES"
    SYSTEMC_HOME="$DEPS/$SYSTEMC"
  fi
fi
if [ -z "$GLOG_ROOT" ]; then
  REQUIRES="$GLOG $REQUIRES"
  GLOG_ROOT="$DEPS/$GLOG"
fi &&
if [ -z "$BOOST_ROOT" ]; then
  REQUIRES="$BOOST $REQUIRES"
  BOOST_ROOT="$DEPS/$BOOST"
  export DEPS_BOOST=$BOOST

fi

if [ -n "$CACHE" ]; then
  CACHE="-c $(mk_and_abs_dir ${CACHE})"
fi

if ! [ -d dependencies ]; then
  # packaged mode
  echo "could not find the dependency repository"
  exit 2
fi

cd dependencies

if [ "$BUILD_CMAKE" = "yes" ]; then
  ./build "$DEPS" $CMAKE_PACKAGE &&
  CMAKE=$DEPS/$CMAKE_PACKAGE/bin/cmake
fi

if [ -x "$CMAKE" ]; then
  export PATH="$(dirname $CMAKE):$PATH"
fi

if ! ./build -j ${NUM_THREADS:-1} $DL_ONLY "$DEPS" $CACHE $REQUIRES; then
  echo "Building dependencies failed. Please see above for error"
  exit 3
fi

cd $BUILD_DIR &&

PREFIX_PATH=$(echo " $REQUIRES"| sed "s@[ ^] *@;$DEPS/@g")

eval_echo() {
 $@
 echo "$@"
}

if [ -z "$DL_ONLY" ]; then
  cd $BUILD_DIR
  eval_echo $CMAKE $SRC_DIR \
    -DCMAKE_INSTALL_PREFIX="$INSTALL" \
    -DCMAKE_PREFIX_PATH="$PREFIX_PATH" \
    $CMAKE_ARGS \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DWITH_SYSTEMC="$WITH_SYSTEMC" \
    -DSYSTEMC_HOME="$SYSTEMC_HOME" \
    -DGLOG_ROOT="$GLOG_ROOT"
  echo "finished bootstrap, you can now call make in $BUILD_DIR"
fi


