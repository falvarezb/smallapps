# OpenMP

OpenMP resources:

- https://www.openmp.org/resources/tutorials-articles/


## Introduction

Apple ships its own version of [LLVM](https://llvm.org/) with Xcode and does not support OpenMP. Therefore, it is necessary to install LLVM separately:

```
brew install llvm
```

Plus OpenMP library:

```
brew install libomp
```

Then, to build a __'hello world'__:

```sh
/usr/local/opt/llvm/bin/clang -fopenmp helloworld.c
```

In case it is needed:

```
CPPFLAGS="-I/usr/local/opt/libomp/include"
LDFLAGS="-L/usr/local/opt/libomp/lib"
```


## Some theory

### Shared Memory Computer

OpenMP assumes a Shared Memory Computer:

- Symmetric Multiprocessor (SMP)
- Non Uniform Address Space Multiprocessor (NUMA)

The shared address space and programming models encourage us to think of them as SMP systems.
Any multiprocessor CPU with a cache is a NUMA system. Start out by treating the system as an SMP and just accept that much of your optimization work will address cases where that case breaks down.


## Examples

### Value of π

Program to estimate the value of π by implementing an algorithm to approximate the following function as a sum of rectangles:

$$\int_{0}^{1}\frac{4.0}{1+x^2}dx$$

Here's different versions of the program:

`serial_pi.c` --> one-thread version a
`false_sharing_par_pi.c` --> parallel version with [false sharing](http://www.nic.uoregon.edu/~khuck/ts/acumem-report/manual_html/ch06s07.html#:~:text=In%20OpenMP%20programs%20False%20sharing,thread%20local%20variables%20often%20helps.&text=Avoid%20writing%20to%20global%20data%20that%20is%20accessed%20from%20multiple%20threads.&text=Align%20shared%20global%20data%20to%20cache%20line%20boundaries.)
`padded_par` --> parallel version with shared data aligned to cache line boundaries to avoid false sharing. See more details about [padding](http://www.catb.org/esr/structure-packing/)

