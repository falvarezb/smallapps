"""
Module with functions to handle the binary and decimal representations of floating-point numbers according to the IEEE 754 standard

Python's 'float' data type represents numbers as double-precision floating points. 
To get around this limitation, some functions in this module make use of the mpmath arbitrary-precision library.
"""

import struct
from typing import List, Tuple
from mpmath import *
from functools import reduce

mp.dps = 100


def check_infinity_or_nan(fraction: List[int], exponent: List[int]) -> None:
    """Check if the bit pattern corresponds to the special floating-point values 'Infinity' or 'NaN'

    If so, raise an OverflowError, else do nothing
    """
    # check if exponent is all ones
    if reduce(lambda x, y: bool(x) and bool(y), exponent, True):
        # check if fraction is all zeros
        if not reduce(lambda x, y: bool(x) or bool(y), fraction, False):
            raise OverflowError("Infinity")
        raise OverflowError("NaN")


def str_to_list(bits: str) -> List[int]:
    """Convert a string made up of digits into a list of integers

    '1001' --> list[1,0,0,1]
    """
    return [int(digit) for digit in bits]


def list_to_str(bits: list) -> str:
    """Concatenate the elements of a list into a single string

    list[1,0,0,1] --> '1001'  
    """
    return "".join([str(bit) for bit in bits])


def to_double_precision_floating_point_binary(number: float) -> Tuple[str, str]:
    """Convert a number in decimal representation to its corresponding double-precision floating-point binary format

    The floating-point number is returned in binary and hexadecimal format

    7.2 --> ('0100000000011100110011001100110011001100110011001100110011001101', '0x401ccccccccccccd')    
    """
    packed = struct.pack('>d', number)
    bits = ''.join(format(byte, '08b') for byte in packed)
    hexrepr = hex(int(bits, 2))
    return (bits, hexrepr)


def round_to_nearest(bits, kth: int, is_any_following_digit_1: bool = None):
    """Round a binary integer to the k-th position using the rule "round to nearest, ties to even"

    Rounds to the nearest value; if the number falls midway, it is rounded to the nearest value with 
    an even least significant digit.

    'bits' is modified in place and trimmed to the kth position

    Algorithm:
    - If the digit following the rounding position is 0, round down (truncate).    
    - If the digit following the rounding position is 1 and all of the following digits are 0, apply the tie-breaking rule:
        - If the digit at the rounding position is even (0), round down (truncate).
        - If the digit at the rounding position is odd (1), round up (add 1).
    - If the digit following the rounding position is 1 and any of the following digits is 1, round up (add 1).

    Args:
        bits (list[int]): bits of the binary integer, bits[0] is the MSB (most-significant bit)
        kth (int): k-th position to which the binary integer is to be rounded
        is_any_following_digit_1 (bool): Optional value to indicate if any of the digits following the (k+1)-th position is 1; in case it is not provided, the function itself checks each of the elements
    """
    if len(bits) > kth:
        if bits[kth] == 0:
            # round down
            bits = bits[:kth+1]
        else:
            is_any_following_digit_1 = len(filter(
                lambda x: x == 1, bits[kth+1:])) != 0 if is_any_following_digit_1 is None else is_any_following_digit_1
            if is_any_following_digit_1:
                # round up
                add1(bits)
            else:
                # same distance, ties to even: round to the nearest even number (the one ending in 0)
                if bits[kth-1] == 1:  # odd
                    add1(bits)

        bits = bits[:kth]


def add1(bits):
    """Add 1 to a binary integer

    'bits' is modified in place

    Args:
        bits (list[int]): bits of the binary integer, bits[0] is the MSB (most-significant bit)
    """

    if bits[len(bits)-1] == 0:
        bits[len(bits)-1] = 1
    else:
        i = len(bits)-1
        # FIXME handle overflow when bits[i]=1 for all values of 'i'
        while bits[i] == 1:
            bits[i] = 0
            i -= 1
        bits[i] = 1


def to_floating_point_binary_ieee754(decimal: float):
    """Like 'to_floating_point_binary' but implementing the IEEE-754 algorithm manually

    """
    # Handle special cases: positive/negative zero and infinities
    if decimal == 0.0:
        return '0' * 32
    elif decimal == float('-inf'):
        return '1' + '0' * 31
    elif decimal == float('inf'):
        return '0' + '1' * 31

    # Convert the number to its IEEE 754 single-precision binary representation
    bits = []
    if decimal < 0:
        sign = '1'
        decimal = abs(decimal)
    else:
        sign = '0'

    exponent_bias = 127  # Bias for single-precision floating-point exponent
    exponent = exponent_bias
    fraction = decimal

    # Normalize the fraction and calculate the exponent
    while fraction >= 2:
        fraction /= 2
        exponent += 1
    while fraction < 1:
        fraction *= 2
        exponent -= 1

    # Remove the implicit leading 1 from the fraction
    fraction -= 1

    # Convert the exponent to binary
    exponent_bits = bin(exponent)[2:].zfill(8)

    # Convert the fraction to binary
    fraction_bits = []
    while fraction != 0 and len(fraction_bits) < 24:
        fraction *= 2
        bit = int(fraction)
        fraction_bits.append(bit)
        fraction -= bit

    # rounding to the nearest even number
    round_to_nearest(fraction_bits, 23, fraction > 0)

    # Combine the sign, exponent, and fraction bits
    bits = sign + exponent_bits + \
        "".join([str(i) for i in fraction_bits[:-1]]).zfill(23)

    return (bits, hex(int(bits, 2)))


# def to_floating_point_decimal(binary: str) -> float:
#     """Transforms binary to decimal

#     The resulting value is not exact but the value rounded according to the IEEE_754 standard implemented by Python itself

#     Args:
#         binary (str): string representing a binary number

#     Returns:
#         float: double-precision floating-point decimal
#     """
#     sign = 1 if binary[0] == '0' else -1
#     fraction = binary[9:]
#     exp = int(binary[1:9], 2)-127
#     mantissa = 1.0
#     for i in range(1, len(fraction)+1):
#         decimal_value = int(fraction[i-1])*(0.5)**i
#         mantissa += decimal_value
#     return sign*mantissa*2**exp


def to_exact_decimal(bits: List[int]) -> Tuple[mpf, int]:
    """Transform bit pattern representing a double-precision floating-point number to exact decimal

    In order to circumvate Python's floating-point arithmetic limitations, this function uses an arbitrary-precision library instead
    of Python's float

    Return a tuple containing the exact decimal representation and the unbiased exponent of the binary format
    """
    exponent_bits = bits[1:12]
    biased_exp = int(list_to_str(exponent_bits), 2)
    fraction = bits[12:]
    check_infinity_or_nan(fraction, exponent_bits)

    half = mpf('0.5')
    exponent_bias = 1023
    unbiased_exp = biased_exp-exponent_bias
    sign = 1 if bits[0] == 0 else -1

    mantissa = mpf(1)
    for i in range(1, len(fraction)+1):
        decimal_value = fraction[i-1]*half**i
        mantissa += decimal_value
    return (sign*mantissa*mpf(2)**unbiased_exp, unbiased_exp)


def fprange(e, p=52):
    (min, max) = [float(2**e), 2**e*(2**(p+1)-1)/2**p]
    # distance = (max-min)/(2**p-1)
    distance = 2**(e-p)
    return (e, min, max, distance)


def prettify(r):
    return f"|{r[0]: < 5}  |  {r[1]: < 25}  |  {r[2]: <27} | {r[3]: <9}|"


def next_binary_value(bits) -> bool:
    """Add 1 to the given bit pattern 'bits', assuming bits[0] is the MSB (most-significant bit)

    The argument is modified in place.
    Returns a boolean to indicate whether there has been overflow (True) or not (False)
    """
    i = len(bits)-1
    while i >= 0 and bits[i] == 1:
        bits[i] = 0
        i -= 1
    if i >= 0:
        bits[i] = 1
        return False

    # overflow
    return True


def next_binary_fp(bits: List[int]) -> None:
    """Return the binary representation of the next double-precision floating-point number

    Argument is modified in place
    Raise OverflowError if the argument or the resulting value is either 'Infinity' or 'NaN'

    Args:
        bits (list[int]): bit pattern of the original double-precision floating-point number

    Returns:
        list[int]: bit pattern of the next double-precision floating-point number
    """
    fraction = bits[12:]
    exponent = bits[1:12]
    check_infinity_or_nan(fraction, exponent)

    if next_binary_value(fraction):
        # overflow in fraction, increase exponent
        next_binary_value(exponent)
        check_infinity_or_nan(fraction, exponent)

    bits[12:] = fraction
    bits[1:12] = exponent


def fp_gen(seed: float) -> Tuple[float, mpf, int]:
    """Return a generator of double-precision floating-point numbers as defined by IEEE 754.

    The floating-point numbers generated are represented by its exact decimal representation (there is a one-to-one correspondence between
    the binary format of a floating-point number and its exact decimal representation)

    The tuple produced by the generator contains:
    - decimal representation up to the standard double-precision (around 16 digits)
    - exact decimal representation
    - unbiased exponent of the binary format


    The generator is seeded by a number in decimal representation that is passed to the function as an argument:
        - the seed must be zero or a positive number
        - the first produced element is the double-precision floating-point number corresponding to the given seed
        - afterwards, elements are returned in ascending order
        - generator stops and throws OverflowError when finding the values "Infinity" or "NaN"

    WARNING: only positive floating-point numbers are generated (negative ones can be obtained by symmetry)
    """
    assert seed >= 0, "seed must be positive or zero"

    bits = to_double_precision_floating_point_binary(seed)[0]
    bits = str_to_list(bits)
    exact_decimal, exp = to_exact_decimal(bits)

    while True:
        yield (float(exact_decimal), exact_decimal, exp)
        next_binary_fp(bits)
        exact_decimal, exp = to_exact_decimal(bits)


if __name__ == "__main__":
    print(to_double_precision_floating_point_binary(7.2))
    # fp_gen = fp_gen(0.00000000000012343)
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))

0.00000000000012343000000000003547764811160377940497012878851013084613441606052219867706298828125
0.00000000000000000000000000002524354896707237777317531408904915934954260592348873615264892578125
# decimal = 1.2
# binary_val = to_floating_point_binary(decimal,False)[0]
# exact_decimal = to_exact_decimal(binary_val)
# print(decimal)
# print(binary_val)
# print(exact_decimal)

# print('\n')
# print(f"| e     |    min                      |   max                        | distance |")
# print(f"|-------------------------------------------------------------------------------|")
# print("\n".join([prettify(fprange(e)) for e in range(50,60)]))
# print('\n')

# print(mpf(1.20000000000000017763568394002504646778106689453125)-mpf(1.1999999999999999555910790149937383830547332763671875))

# 0.0000000000000002220446049250313
# 0.0000000000000002220446049250313080847263336181640625
# 0.0000000000000002220446049250313080847263336181640625
