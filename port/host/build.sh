#!/bin/zsh
# Compila el núcleo del juego para macOS (sin gráficos): port/host/build/bt_host
set -e
cd "${0:A:h}"
mkdir -p build
SRC=../source
clang++ -std=c++20 -O2 -g -ffp-contract=off -DBT_HOST -Wall -Wno-unused-function \
	-I"$SRC" host_main.cpp \
	"$SRC/game.cpp" "$SRC/ball.cpp" "$SRC/collision.cpp" "$SRC/rules.cpp" \
	"$SRC/scene.cpp" "$SRC/script.cpp" "$SRC/world.cpp" \
	-o build/bt_host
echo "ok build/bt_host"
