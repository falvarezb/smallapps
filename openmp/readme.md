# OpenMP

OpenMP resources:

- https://www.openmp.org/resources/tutorials-articles/


## Introduction

Apple ships its own version of [LLVM](https://llvm.org/) with Xcode and does not support OpenMP. Therefore, it is necessary
to install LLVM separately:

```
brew install llvm
```

Plus OpenMP library:

```
brew install libomp
```

Then, to build a simple `hello world`, you'd write:

```
/usr/local/opt/llvm/bin/clang -fopenmp helloworld.c
```

In case it is needed:

```
CPPFLAGS="-I/usr/local/opt/libomp/include"
LDFLAGS="-L/usr/local/opt/libomp/lib"
```