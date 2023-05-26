#!/bin/bash

rm -rf out
mkdir out

#single-threaded version of pi.c
cc pi.c util.c -o out/serialpi
#multi-threaded version of pi.c
/usr/local/opt/llvm/bin/clang -fopenmp pi.c util.c -o out/parpi

#single-threaded version of dandc.c
cc dandc.c util.c -o out/serialdandc
#multi-threaded version of dandc.c
/usr/local/opt/llvm/bin/clang -fopenmp dandc.c util.c -o out/pardandc

/usr/local/opt/llvm/bin/clang -fopenmp spmd.c util.c -o out/spmd
/usr/local/opt/llvm/bin/clang -fopenmp spmd_padded.c util.c -o out/spmd_padded    
/usr/local/opt/llvm/bin/clang -fopenmp spmd_synced.c util.c -o out/spmd_synced