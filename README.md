<a name="top"></a>

<p align="center"><img src="docs/banner.png" alt="Fever"/></p>
***
<p align="center">
<a href="http://spacemacs.org"><img src="https://cdn.rawgit.com/syl20bnr/spacemacs/442d025779da2f62fc86c2082703697714db6514/assets/spacemacs-badge.svg" alt="Made with Spacemacs"></a>
<a href="https://travis-ci.com/sevanspowell/Fever"><img src="https://travis-ci.com/sevanspowell/Fever.svg?token=JnRKqMsbJh9FxmkLU49o&branch=master" alt="Build Status"></a>
<a href="http://www.twitter.com/sevanspowell"><img src="http://i.imgur.com/tXSoThF.png" alt="Twitter" align="right"></a>
</p>
***

The Fever Graphics Library

<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-generate-toc again -->
**Table of Contents**

- [Cloning](#cloning)
- [Dependencies](#dependencies)
- [Building](#building)
- [Contributing](#contributing)

<!-- markdown-toc end -->


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
