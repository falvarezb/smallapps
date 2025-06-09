# About

Functions to manipulate the binary and decimal representations of floating-point numbers according to the [IEEE 754]((https://en.wikipedia.org/wiki/IEEE_754-1985)) standard.


## Rule "round to nearest, ties to even"

This rule is IEEE 754's standard method to round floating-point numbers.

Algorithm to apply this rule to binary numbers:

- If the digit following the rounding position is 0, round down (truncate).    
- If the digit following the rounding position is 1 and all of the following digits are 0, apply the tie-breaking rule:
    - If the digit at the rounding position is even (0), round down (truncate).
    - If the digit at the rounding position is odd (1), round up (add 1).
- If the digit following the rounding position is 1 and any of the following digits is 1, round up (add 1).

### Explanation
In binary, adding 1 to the _(k+1)-th_ position is half the increment than adding 1 to the _k-th_ position (__counting positions from the MSB - most significant bit -__).

e.g. decimal value 8 in binary is _1000_; adding 1 to the 3rd position is an increment of 2^1 resulting in _1010_ (10), whereas adding 1 to the 4th position is an increment of 2^0 resulting in _1001_ (9).

So when rounding a binary number to the _k-th_ position, if (_k+1)-th_ is 1 and all subsequent digits are 0, that value will lie exactly in the middle between the "round down" and "round up" values.
On the other hand, if any of the subsequent digits is 1, then the number will be greater than the middle value and closer to the "round up" one.

More formally, given the binary number $x$:

- $x_k = x + 2^k$
- $x_{k+1} = x + 2^{k-1}$

Then:

- $x_{k+1} - x = (x + 2^{k-1}) - x = 2^{k-1}$
- $x_k - x_{k+1} = (x + 2^k) - (x + 2^{k-1}) = 2^k (1 - 2^{-1}) = 2^k 2^{-1} = 2^{k-1}$

proving that $x_{k+1}$ lies in the middle between $x$ and $x_k$

If the binary representation of a floating-point number has a group of digits that repeats indefinitely, it's not feasible to examine all digits to determine whether all of them are 0 or not. Instead, when calculating the binary representation, we can check if the fraction left after calculating the _(k+1)-th_ digit is 0 or not.



## Online converters
- [baseconvert](https://baseconvert.com/ieee-754-floating-point)
- [exploringbinary](https://www.exploringbinary.com/floating-point-converter/)



## Benchmark

This benchmark compares the performance of different floating-point representations (single float, double float, and BigDecimal) in calculating the value of π (Pi) using a numerical integration method.

Actual [Pi value](https://www.piday.org/million/) used for comparison:

Pi = 3.141592653589793238462643383279502884197169399375105820974


| Single float | 1e7 | 1e8 | 1e9 |
|----------|----------|----------|----------|
|   Value      |    3.099413     |    0.67108864     |    0.06710886     |
|   Time (s)      |    0.059     |   0.623     |    5.531    |

| Double float | 1e7 | 1e8 | 1e9 |
|----------|----------|----------|----------|
|   Value      |    3.141592653589731     |    3.1415926535904264     |    3.1415926535899708    |
|   Time (s)      |    0.055     |   0.591    |    5.169    |

| BigDecimal (prec=32) | 1e7 | 1e8 | 1e9 |
|----------|----------|----------|----------|
|   Value      |    3.141592653589794071795976717045     |    3.1415926535897932467959767163538     |    3.14159265358979323854597671658459    |
|   Time (s)      |    2.916     |   27.218    |    377.414    |


## Annex

The algorithm used to calculate Pi is a numerical integration technique called the midpoint rectangle method.

The value of π can be represented as a definite integral:

$$ 
\pi = \int_0^1 \frac{4}{1 + x^2} dx 
$$


### Algorithm Details

The algorithm divides the interval [0, 1] into numSteps equal-width subintervals (rectangles). For each subinterval:

- Calculate the midpoint: $ x_i = (i + 0.5) \times \text{stepSize} $
- Evaluate the function: $ f(x_i) = \frac{4}{1 + x_i^2} $
- Sum the areas: Each rectangle's area is $ f(x_i) \times \text{stepSize} $
- Total sum: The sum of all rectangle areas approximates the integral, and thus π.

