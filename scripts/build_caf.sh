#!/bin/bash

# Directory containing this bash script
readonly DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

readonly PREVIOUS_DIRECTORY=$(pwd)

readonly ROOT_DIR=$DIR/..

cd $ROOT_DIR/external

cd ./actor-framework-0.17.4

CC="ccache gcc" CXX="ccache g++" ./configure --build-type=Release --no-examples --no-python --with-log-level=TRACE --with-actor-profiler --with-runtime-checks
cd build 
CC="ccache gcc" CXX="ccache g++" make -j$(nproc)

cd $PREVIOUS_DIRECTORY

exit 0

