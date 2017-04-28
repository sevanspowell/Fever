#!/bin/bash

cmake -E make_directory build
cmake -E chdir build cmake -E time cmake ../src
cmake -E time cmake --build build --target all --config Debug
find ./build | ag clang_complete | xargs cat | sort | uniq | sed '/-W.*$/ d' > .clang_complete
