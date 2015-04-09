#! /bin/bash

cmake \
    -D Trilinos_ASSERT_MISSING_PACKAGES=OFF \
    -D Trilinos_ENABLE_TeuchosCore=ON \
    -D Trilinos_ENABLE_DEBUG=ON \
    -D Trilinos_ENABLE_TESTS=ON \
    ..
