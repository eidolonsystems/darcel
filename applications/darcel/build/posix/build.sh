#!/bin/bash
set -o errexit
set -o pipefail
directory=$(pwd)
$directory/../../../../build/posix/local_build.sh $directory $1
