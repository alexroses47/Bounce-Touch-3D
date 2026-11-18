#!/bin/zsh
# Compila las reglas del motor (port/source/rules.cpp) para el Mac: tests/build/librules.dylib
set -e
cd "${0:A:h}"
mkdir -p build
clang++ -std=c++17 -O1 -ffp-contract=off -shared -fPIC -DBT_HOST_TESTS -I ../port/source ../port/source/rules.cpp -o build/librules.dylib
echo "build/librules.dylib"
