#!/bin/bash
tools/setup-common.sh
mkdir -p build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
