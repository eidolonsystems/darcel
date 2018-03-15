cd %~dp0../..
mkdir include
cd include
mkdir darcel
cd %~dp0
printf "#define DARCEL_VERSION """> %~dp0../../include/darcel/version.hpp
git rev-list --count --first-parent HEAD | tr -d "\n\" >> %~dp0../../include/darcel/version.hpp
printf """\n" >> %~dp0../../include/darcel/version.hpp
