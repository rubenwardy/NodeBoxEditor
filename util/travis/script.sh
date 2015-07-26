#!/bin/bash -e
# Taken from https://github.com/minetest/minetest LGPL 2.1 or later

if [[ $PLATFORM == "Linux" ]]; then
	mkdir -p travisbuild
	cd travisbuild
	CMAKE_FLAGS='-DCMAKE_BUILD_TYPE=Debug'
	cmake $CMAKE_FLAGS ..
	make -j2 && exit 0
elif [[ $PLATFORM == Win* ]]; then
	[[ $CC == "clang" ]] && exit 1 # Not supposed to happen
	# We need to have our build directory outside of the minetest directory because
	#  CMake will otherwise get very very confused with symlinks and complain that
	#  something is not a subdirectory of something even if it actually is.
	# e.g.:
	# /home/travis/minetest/minetest/travisbuild/minetest
	# \/  \/  \/
	# /home/travis/minetest/minetest/travisbuild/minetest/travisbuild/minetest
	# \/  \/  \/
	# /home/travis/minetest/minetest/travisbuild/minetest/travisbuild/minetest/travisbuild/minetest
	# You get the idea.
	OLDDIR=$(pwd)
	cd ..
	export EXISTING_nodeboxeditor_DIR=$OLDDIR
	if [[ $PLATFORM == "Win32" ]]; then
		$OLDDIR/util/buildbot/buildwin32.sh travisbuild && exit 0
	elif [[ $PLATFORM == "Win64" ]]; then
		$OLDDIR/util/buildbot/buildwin64.sh travisbuild && exit 0
	fi
else
	echo "Unknown platform \"${PLATFORM}\"."
	exit 1
fi
