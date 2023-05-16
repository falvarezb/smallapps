# OpenMP

OpenMP resources:

- https://www.openmp.org/resources/tutorials-articles/


## Environment configuration (MacOSX)

Apple ships its own version of [LLVM](https://llvm.org/) with Xcode and does not support OpenMP. Therefore, it is necessary to install LLVM separately:

```
brew install llvm
```

Plus OpenMP library:

```
brew install libomp
```

Then, to build a __'hello world'__:

```
/usr/local/opt/llvm/bin/clang -fopenmp helloworld.c
```

In case it is needed:

```
CPPFLAGS="-I/usr/local/opt/libomp/include"
LDFLAGS="-L/usr/local/opt/libomp/lib"
```

## Examples

### Value of π

Program to estimate the value of π by doing the numerical integration of:

$$\int_{0}^{1}\frac{4.0}{1+x^2}dx$$

 
Here's different versions of the program:

_serial_pi.c_ --> one-thread version
_false_sharing_par_pi.c_ --> parallel version with [false sharing](http://www.nic.uoregon.edu/~khuck/ts/acumem-report/manual_html/ch06s07.html#:~:text=In%20OpenMP%20programs%20False%20sharing,thread%20local%20variables%20often%20helps.&text=Avoid%20writing%20to%20global%20data%20that%20is%20accessed%20from%20multiple%20threads.&text=Align%20shared%20global%20data%20to%20cache%20line%20boundaries.)
_padded_par_ --> parallel version with [shared data aligned to cache line boundaries](http://www.catb.org/esr/structure-packing/) to avoid false sharing.
_synced_par_pi.c_ --> parallel version with shared data inside a critical section to avoid false sharing; this solution is more portable than the one using padding as it does not rely on a specific cache line size
_for_par_pi.c_ --> version using parallel reduction; this version is easily derived from the serial version


Whereas _false_sharing_par_pi.c_ and its variants rely on __SPMD__ (Single Program Multiple Data) algorithm, _for_par_pi.c_ uses the __worksharing__ approach.

SPMD algorithm executes a single program on multiple processors, with each processor working on a different subset of data. 

Worksharing is a technique for dividing a task into smaller sub-tasks and distributing those sub-tasks among multiple processors.

Here's a comparison of the different versions for a given number of steps of 5,000,000,000

```
/usr/local/opt/llvm/bin/clang serial_pi.c -o out/serial_pi
/usr/local/opt/llvm/bin/clang -fopenmp false_sharing_par_pi.c -o out/false_sharing_par_pi
/usr/local/opt/llvm/bin/clang -fopenmp padded_par_pi.c -o out/padded_par_pi       
/usr/local/opt/llvm/bin/clang -fopenmp synced_par_pi.c -o out/synced_par_pi
/usr/local/opt/llvm/bin/clang -fopenmp for_reduction_par_pi.c -o out/for_par_pi   
```

```
./out/serial_pi
./out/false_sharing_par_pi <n>
./out/padded_par_pi <n>
./out/synced_par_pi <n>
./out/for_reduction_par_pi
```

<table>
    <thead>
        <tr>
            <th>threads</th>
            <th>serial</th>
            <th>SPMD (false sharing)</th>
            <th>SPMD (padded)</th>
            <th>SPMD (synced)</th>
            <th>Worksharing (for-reduction)</th>
            <th>divide and conquer</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>8</td>
            <td rowspan=5>14.035</td>
            <td>20.931</td>
            <td>2.374</td>
            <td>2.143</td>
            <td rowspan=5>2.314</td>
            <td rowspan=5>1.886</td>
        </tr>
        <tr>
            <td>16</td>
            <td>13.985</td>
            <td>2.090</td>
            <td>2.040</td>
        </tr>
        <tr>
            <td>100</td>
            <td>4.483</td>
            <td>2.120</td>
            <td>2.045</td>
        </tr>
        <tr>
            <td>1000</td>
            <td>4.029</td>
            <td>2.069</td>
            <td>2.045</td>
        </tr>
    </tbody>
</table>

__Notes__: 

- time in seconds
- 8 is the number os physical cores of my machine
- 16 is the number of virtual cores of my machine

__Analysis__:

- the effect of false sharing is inversely proportional to the number of threads; that makes sense as the array _double sum[num_threads]_ will span more and more cache lines and therefore the probability of 2 threads accessing the same cache line will be lower
- the performance of the _padded_ and _synced_ versions is similar but the _synced_ version is portable across different hardware architectures
- the _for-reduction_ version uses the default scheduler and default number of threads that, in my machine, is 16 (one per core)
- the _for-reduction_ version can be compiled without OpenMP flag and become _serial_pi_ again.
