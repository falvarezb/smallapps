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

provingthat $x_{k+1}$ lies in the middle between $x$ and $x_k$

If the binary representation of a floating-point number has a group of digits that repeats indefinitely, it's not feasible to examine all digits to determine whether all of them are 0 or not. Instead, when calculating the binary representation, we can check if the fraction left after calculating the _(k+1)-th_ digit is 0 or not.



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


https://docs.python.org/3/library/string.html#formatspec

1023.999999999999 8863131622783839702606201171875
1023.999999999999 89
   0.000000000000 1136868377216160297393798828125

1023.9999999999999
1023.9999999999999
1023.9999999999999
1023.9999999999998863131622783839702606201171875


0.0000000000001136868377216160297393798828125
0.0000000000001136868377216160297393798828125

0.00010129999999999999573362108318264063200331293046474456787109375
7.2057594037927956