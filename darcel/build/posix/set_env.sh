#!/bin/bash
local_path=$(pwd)
if [ "$(uname -s)" = "Darwin" ]
then
  so_ext="dylib"
else
  so_ext="so"
fi
export DARCEL_INCLUDE_PATH=$local_path/../../include
