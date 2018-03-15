#!/bin/bash
set -o errexit
set -o pipefail
mkdir -p ./../../include/darcel
printf "#define DARCEL_VERSION \""> ./../../include/darcel/version.hpp
git rev-list --count --first-parent HEAD | tr -d "\n" >> ./../../include/darcel/version.hpp
printf \""\n" >> ./../../include/darcel/version.hpp
