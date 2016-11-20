# Fever 
The Fever Graphics Library

## Cloning

```
git clone --recursive https://github.com/samdelion/Fever.git
```
OR
```
git clone https://github.com/samdelion/Fever.git
cd Fever
git submodule update --init --recursive
```

## Dependencies

- SDL2
- GLEW

## Building

From root directory:

```
cmake -E make_directory build
cmake -E chdir build cmake -E time cmake ../src
cmake -E time cmake --build build --target all --config Debug
```

## Contributing

Please format your code using the .clang-format file provided and adhering to
the coding standards found [here](http://llvm.org/docs/CodingStandards.html).
