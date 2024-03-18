#!/bin/bash

echo -e "\nBuilding Debug"
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug &&
cmake --build build/debug