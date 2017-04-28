#!/bin/bash

cmake -E make_directory build_xcode
cmake -E chdir build_xcode cmake -E time cmake -G Xcode ../src
cmake -E time cmake --build build_xcode --config Debug --clean-first
