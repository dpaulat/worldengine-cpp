#!/bin/bash
tools/setup-common.sh
mkdir -p build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
