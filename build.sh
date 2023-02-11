#!/usr/bin/env sh

cd ~/Documents/Spacetorio

cmake -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B ./out/build/x64-debug/ -S . \
 &&    cd ./out/build/x64-debug/ \
 &&    ninja \
 &&    ./Spacetorio/Spacetorio
