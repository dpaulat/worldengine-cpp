#!/bin/bash
tools/setup-common.sh
mkdir -p build-cdt
cd build-cdt
cmake -DCMAKE_BUILD_TYPE=Debug -G"Eclipse CDT4 - Ninja" ..
