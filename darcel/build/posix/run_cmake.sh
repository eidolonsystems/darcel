#!/bin/bash
directory=$(pwd)
if [ "$1" = "" ]
then
  build_type="Release"
else
  build_type=$1
fi
pushd $directory
. ./set_env.sh
export LIB_INSTALL_DIRECTORY=$directory/../../library
export TEST_INSTALL_DIRECTORY=$directory/../../tests
export PROJECT_BUILD_TYPE=$build_type
cmake -G"Unix Makefiles" $directory/../config
unset PROJECT_BUILD_TYPE
popd
