SETLOCAL

if exist Catch2-2.2.1 goto end_catch_setup
  git clone --branch v2.2.1 https://github.com/catchorg/Catch2.git Catch2-2.2.1
:end_catch_setup

if exist tclap-1.2.1 goto end_tclap_setup
  git clone --branch tclap-1-2-1-release-final https://github.com/eile/tclap.git tclap-1.2.1
:end_tclap_setup

ENDLOCAL
