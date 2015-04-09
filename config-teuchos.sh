#! /bin/bash

cmake \
    -D CMAKE_BUILD_TYPE:STRING=Debug \
    -D Trilinos_ASSERT_MISSING_PACKAGES=OFF \
    -D Trilinos_ENABLE_TeuchosCore=ON \
    -D Trilinos_ENABLE_DEBUG=ON \
    -D Trilinos_ENABLE_TESTS=ON \
    ..
