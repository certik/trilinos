#! /bin/bash

cmake \
    -D CMAKE_BUILD_TYPE:STRING=Release \
    -D Trilinos_ASSERT_MISSING_PACKAGES=OFF \
    -D Trilinos_ENABLE_TeuchosCore=ON \
    -D Trilinos_ENABLE_DEBUG=OFF \
    -D Trilinos_ENABLE_TESTS=ON \
    -D CMAKE_INSTALL_PREFIX:PATH="xx" \
    ..
