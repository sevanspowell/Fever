#!/bin/bash

cmake -E make_directory build
cmake -E chdir build cmake -E time cmake ../src
cmake -E time cmake --build build --target all --config Debug --clean-first
