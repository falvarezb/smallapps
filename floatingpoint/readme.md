# About

Implementation of the [IEEE 754]((https://en.wikipedia.org/wiki/IEEE_754-1985)) standard to convert floating-point numbers between their binary and decimal representations.


The implementation covers [single](https://en.wikipedia.org/wiki/Single-precision_floating-point_format) and [double-precision](https://en.wikipedia.org/wiki/Double-precision_floating-point_format) formats.


Python's own _float_ data type represents numbers as double-precision floating points. To get around this limitation, 
some functions in this module make use of the [mpmath](https://mpmath.org) arbitrary-precision library.



### Rule "round to nearest, ties to even"

This rule is the standard method to round floating-point numbers to be represented as IEEE 754 single/double-precision binary numbers.

The algorithm to apply this rule is:

- If the digit following the rounding position is 0, round down (truncate).    
- If the digit following the rounding position is 1 and all of the following digits are 0, apply the tie-breaking rule:
    - If the digit at the rounding position is even (0), round down (truncate).
    - If the digit at the rounding position is odd (1), round up (add 1).
- If the digit following the rounding position is 1 and any of the following digits is 1, round up (add 1).

In order to understand the rule, it's helpful to keep in mind the following fact: in binary, adding 1 to the _(k+1)-th_ position is half the increment than adding 1 to the _k-th_ position (__providing that the MSB is in the first position, the leftmost one__).

__e.g.__ decimal value 8 in binary is _1000_; adding 1 to the 3rd position is an increment of 2^1 resulting in _1010_ (10), whereas adding 1 to the 4th position is an increment of 2^0 resulting in _1001_ (9).

So when rounding a binary number to the _k-th_ position, if (_k+1)-th_ is 1 and all subsequent digits are 0, that value will lie exactly in the middle between the "round down" and "round up" values.
On the other hand, if any of the subsequent digits is 1, then the number will be greater than the middle value and closer to the "round up" one.

More formally:

- increment on the _k-th_ position = 2^k
- increment on the _(k+1)-th_ position = 2^(k-1)

Then both distances are the same:

- distance between the number and the "round down" value is: 2^(k-1)
- distance between the number and the "round up" value is: 2^k - 2^(k-1) = 2^k (1 - 2^-1) = 2^k 2^-1 = 2^(k-1)


If the binary representation of a floating-point number has a group of digits that repeats indefinitely, it's not feasible to examine all digits to determine whether all of them are 0 or not. Instead, when calculating the binary representation, we can check if the fraction left after calculating the _(k+1)-th_ digit is 0 or not.

1/3 = 0.3333333333
arbitrary precision

### BigDecimal

scala> val b = BigDecimal.exact(7.4)                                            isDecimalDouble isBinaryDouble  isExactDouble
b: scala.math.BigDecimal = 7.4000000000000003552713678800500929355621337890625  false           true            true

scala> val b = BigDecimal.decimal(7.4)                                          
b: scala.math.BigDecimal = 7.4                                                  true            false           false

scala> val b = BigDecimal.binary(7.4)
b: scala.math.BigDecimal = 7.400000000000000355271367880050093                  false           true            false  

scala> val b = BigDecimal.decimal(7)
b: scala.math.BigDecimal = 7                                                    true            true            true

scala> val b = BigDecimal(1)/BigDecimal(3)
b: scala.math.BigDecimal = 0.3333333333333333333333333333333333                 false           false           false

### References

[Floating-point assembly](https://staffwww.fullcoll.edu/aclifton/cs241/lecture-floating-point-simd.html)

### Online converters
- [baseconvert](https://baseconvert.com/ieee-754-floating-point)
- [exploringbinary](https://www.exploringbinary.com/floating-point-converter/)


0000000000000000000000000000000000000000000000000000
1111111111111111111111111111111111111111111111111111
1.9999999999999998