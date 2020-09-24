#!/bin/bash
platform='unknown'
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
	platform='linux'
elif [[ "$OSTYPE" == "freebsd"* ]]; then
	platform='bsd'
elif [[ "$OSTYPE" == "darwin"* ]]; then
	platform='mac'
elif [[ "$OSTYPE" == "cygwin" ]]; then
	platform='windows'
elif [[ "$OSTYPE" == "msys" ]]; then
	platform='windows'
elif [[ "$OSTYPE" == "win32" ]]; then
	platform='windows'
else
	echo "OSTYPE == ${OSTYPE} not reconized, assuming it's windows based"
	platform='windows'
fi

if [[ "$platform" == "windows" ]]; then
	powershell kill -n devenv
fi

mkdir -p build
cd build
cmake ..

if [[ "$platform" == "windows" ]]; then
	start SDLBgfxTestProject.sln
fi
