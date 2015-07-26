#!/bin/bash
# Taken from https://github.com/minetest/minetest LGPL 2.1 or later

set -e

dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ $# -ne 1 ]; then
	echo "Usage: $0 <build directory>"
	exit 1
fi
builddir=$1
mkdir -p $builddir
mkdir -p $builddir/bin
builddir="$( cd "$builddir" && pwd )"
packagedir=$builddir/packages
libdir=$builddir/libs

toolchain_file=$dir/toolchain_mingw64.cmake
irrlicht_version=1.8.1
zlib_version=1.2.8

mkdir -p $packagedir
mkdir -p $libdir

cd $builddir

# Get stuff
[ -e $packagedir/irrlicht-$irrlicht_version.zip ] || wget http://sfan5.pf-control.de/irrlicht-$irrlicht_version-win64.zip \
	-c -O $packagedir/irrlicht-$irrlicht_version.zip
[ -e $packagedir/zlib-$zlib_version.zip ] || wget http://sfan5.pf-control.de/zlib-$zlib_version-win64.zip \
	-c -O $packagedir/zlib-$zlib_version.zip


# Extract stuff
cd $libdir
[ -d irrlicht-$irrlicht_version ] || unzip -o $packagedir/irrlicht-$irrlicht_version.zip
[ -d zlib ] || unzip -o $packagedir/zlib-$zlib_version.zip -d zlib

# Get nodeboxeditor
cd $builddir
if [ ! "x$EXISTING_nodeboxeditor_DIR" = "x" ]; then
	ln -s $EXISTING_nodeboxeditor_DIR nodeboxeditor
else
	[ -d nodeboxeditor ] && (cd nodeboxeditor && git pull) || (git clone https://github.com/rubenwardy/nodeboxeditor)
fi
cd nodeboxeditor
git_hash=`git show | head -c14 | tail -c7`

# Build the thing
[ -d _build ] && rm -Rf _build/
mkdir _build
cd _build
cmake .. \
	-DCMAKE_TOOLCHAIN_FILE=$toolchain_file \
	-DCMAKE_INSTALL_PREFIX=/tmp \
	\
	-DIRRLICHT_INCLUDE_DIR=$libdir/irrlicht-$irrlicht_version/include \
	-DIRRLICHT_LIBRARY=$libdir/irrlicht-$irrlicht_version/lib/Win64-gcc/libIrrlicht.dll.a \
	-DIRRLICHT_DLL=$libdir/irrlicht-$irrlicht_version/bin/Win64-gcc/Irrlicht.dll \
	\
	-DZLIB_INCLUDE_DIR=$libdir/zlib/include \
	-DZLIB_LIBRARIES=$libdir/zlib/lib/libz.dll.a \
	-DZLIB_DLL=$libdir/zlib/bin/zlib1.dll

make package -j2

# EOF
