# Vectorization

[Vectorization](https://www.codeproject.com/Articles/874396/Crunching-Numbers-with-AVX-and-AVX) is the process of performing mathematical operations on arrays of data in a single operation instead of iterating and processing each element individually.

This is possible because modern processors support SIMD (single instruction, multiple data) processing, whereby an operation can be applied to all elements of a vector at once in a single clock cycle.

Different manufacturers implement the SIMD capability with different instructions. In the case of Intel, there is the set of instructions called Advanced Vector Extensions (AVX).

Access to AVX instructions is done through special C functions called intrinsic functions (an intrinsic function doesn't necessarily map to a single instruction though).

Vectorization can significantly improve the performance of certain algorithms, especially when working with large datasets.

AVX2 allows for 256-bit vectors and AVX512 for 512-bit vectors. My Intel(R) Core(TM) i9-9880H CPU @ 2.30GHz does not support AVX512 though.

```
>> sysctl -a | grep machdep.cpu.leaf7_features
machdep.cpu.leaf7_features: RDWRFSGS TSC_THREAD_OFFSET SGX BMI1 AVX2 SMEP BMI2 ERMS INVPCID FPU_CSDS MPX RDSEED ADX SMAP CLFSOPT IPT SGXLC MDCLEAR IBRS STIBP L1DF ACAPMSR SSBD
```

In order to compile AVX2 intrinsics, the compiler must be passed the flag `-mavx2`:

```
gcc -mavx2 main.c
```

## Benchmark

file size = 16Gb (2^34), equivalent to an array with 2^32 (2^34/4) integers

buffer size = 8Mb (2^23)

Result after 5 runs of the scalar and AVX2 versions:

* __scalar version__ = [25.447367, 24.903282, 25.970131, 25.799534, 25.862530] --> 25.596569 ± 0.434277

* __AVX2 version__ = [21.470280, 22.316618, 21.550274, 23.462484, 21.382882] --> 22.0365076 ± 0.879792
