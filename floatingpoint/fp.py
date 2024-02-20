"""
Module with functions to manipulate the binary and decimal representations of floating-point 
numbers according to the IEEE 754 standard

Python's 'float' data type represents numbers as double-precision floating points. 
To get around this limitation, some functions in this module make use of the mpmath 
arbitrary-precision library.

In Python, floating-point numbers are automatically displayed in exponential notation when the absolute 
value of the number is either very large (greater than or equal to 1e16) or very small (less than 1e-4 and not equal to zero).

https://docs.python.org/3/tutorial/floatingpoint.html

Interestingly, there are many different decimal numbers that share the same nearest approximate binary fraction [...] Historically, 
the Python prompt and built-in repr() function would choose the one with 17 significant digits, 0.10000000000000001. 
Starting with Python 3.1, Python (on most systems) is now able to choose the shortest of these and simply display 0.1.
"""

from decimal import ROUND_HALF_UP, Decimal, setcontext, Context
import struct
from math import log2, log10, floor
from functools import reduce, singledispatch
from typing import List, Tuple, Generator
from fputil import str_to_list, list_to_str, next_binary_value

setcontext(Context(prec=400, rounding=ROUND_HALF_UP))


class FP:
    """Class representing a double-precision floating-point number, with the following attributes:
    - fp: the decimal value selected by Python as representative of the floating-point number
    - bits: the binary representation of the floating-point number
    - exact_decimal: the exact decimal representation of the floating-point number (https://www.exploringbinary.com/number-of-decimal-digits-in-a-binary-fraction/)
    - unbiased_exp: the unbiased exponent of the floating-point number
    """

    def __init__(self, fp: float, bits: str, exact_decimal: Decimal, unbiased_exp: int):
        self.fp = fp
        self.bits = bits
        self.exact_decimal = exact_decimal
        self.unbiased_exp = unbiased_exp

    def __repr__(self):
        return f"FP(float={self.fp}, bits={self.bits}, exact_decimal={self.exact_decimal}, unbiased_exp={self.unbiased_exp})"

    def __eq__(self, other):
        return self.fp == other.fp and self.bits == other.bits and self.exact_decimal == other.exact_decimal and self.unbiased_exp == other.unbiased_exp

    @staticmethod
    def from_decimal(dec: Decimal) -> "FP":
        """Return a FP object from the given Decimal number
        """
        bits = from_decimal_to_binary(float(dec))[0]
        return FP.from_binary(str_to_list(bits))
        

    @staticmethod
    def from_float(f: float) -> "FP":
        """Return a FP object from the given float number
        """
        bits = from_decimal_to_binary(f)[0]
        return FP.from_binary(str_to_list(bits))

    @staticmethod
    def from_binary(bits: List[int]) -> "FP":
        """Return a FP from the given binary representation

        The decimal representation returned by this function consists of: 
        - the exact decimal value of the floating-point number (https://www.exploringbinary.com/number-of-decimal-digits-in-a-binary-fraction/)
        - the decimal value selected by Python as representative of the floating-point number

        Normally, the latter is shorter than the former, and it is the value returned by the function 'float()': it is the
        decimal number with the maximum number of digits (around 16) needed to uniquely distinguish that value from the adjacent values        
        """
        sign, fraction_bits, exponent_bits, unbiased_exp = unpack_double_precision_fp(bits)
        check_infinity_or_nan(fraction_bits, exponent_bits)

        half = Decimal(0.5)
        mantissa = Decimal(1)
        for i in range(1, len(fraction_bits) + 1):
            place_value = fraction_bits[i - 1] * half**i
            mantissa += place_value
        exact_decimal = sign * mantissa * Decimal(2)**unbiased_exp
        return FP(float(exact_decimal), list_to_str(bits), exact_decimal, unbiased_exp)


class Segment:
    """Class representing a segment of double-precision floating-point numbers, with the following attributes:
    - unbiased_exp: the unbiased exponent that defines the segment
    - min_val: the minimum floating-point number in the segment represented as an exact decimal
    - max_val: the maximum floating-point number in the segment represented as an exact decimal
    - distance: the distance between consecutive binary floating-point numbers in the segment represented as an exact decimal
    """

    def __init__(self, unbiased_exp: int, min_val: Decimal, max_val: Decimal, distance: Decimal) -> None:
        self.unbiased_exp = unbiased_exp
        self.min_val = min_val
        self.max_val = max_val
        self.distance = distance

    def __repr__(self):
        return f"Segment(unbiased_exp={self.unbiased_exp}, min_val={self.min_val}, max_val={self.max_val}, distance={self.distance})"

    def __eq__(self, other):
        return self.unbiased_exp == other.unbiased_exp and self.min_val == other.min_val and self.max_val == other.max_val and self.distance == other.distance


def unpack_double_precision_fp(bits: List[int]) -> Tuple[int, List[int], List[int], int]:
    """Decompose the binary representation of a double-precision floating-point number 
    into its elements: 
    - sign
    - fraction bits
    - exponent bits
    - unbiased exponent
    """
    exponent_bits = bits[1:12]
    biased_exp = int(list_to_str(exponent_bits), 2)
    double_precision_exponent_bias = 1023
    unbiased_exp = biased_exp - double_precision_exponent_bias
    fraction_bits = bits[12:]
    sign = 1 if bits[0] == 0 else -1
    return (sign, fraction_bits, exponent_bits, unbiased_exp)


def check_infinity_or_nan(fraction: List[int], exponent: List[int]) -> None:
    """Check if the bit pattern corresponds to the special floating-point values 'Infinity' or 'NaN'
    If so, raise an OverflowError, else return None
    """
    # check if exponent is all ones
    if reduce(lambda x, y: bool(x) and bool(y), exponent, True):
        # check if fraction is all zeros
        if not reduce(lambda x, y: bool(x) or bool(y), fraction, False):
            raise OverflowError("Infinity")
        raise OverflowError("NaN")


def from_decimal_to_binary(number: float) -> Tuple[str, str]:
    """Convert a double-precision floating-point number from decimal to binary representation

    from_binary_to_decimal(from_decimal_to_binary(x)) == x

    The floating-point number is returned in binary and hexadecimal format

    7.2 --> ('0100000000011100110011001100110011001100110011001100110011001101', '0x401ccccccccccccd')    
    """
    packed = struct.pack('>d', number)
    bits = ''.join(format(byte, '08b') for byte in packed)
    hexrepr = hex(int(bits, 2))
    return (bits, hexrepr)

@singledispatch
def segment_params(x, ctx: Context) -> Segment:
    """See specific implementations for the different types of the argument: segment_params_int and segment_params_float

    Calculate the parameters of a double-precision floating-point segment 
    (the segment is determined differently depending on the type of the argument)

    Return an object of type Segment
    """
    raise NotImplementedError


@segment_params.register
def segment_params_int(e: int, ctx: Context) -> Segment:
    """Calculate the parameters of the double-precision floating-point segment corresponding 
    to the unbiased exponent 'e'

    See base case 'segment_params' for more details
    """
    setcontext(ctx)
    p = 52
    two = Decimal(2)
    min_val: Decimal = two**e
    max_val: Decimal = two**(e + 1) * (1 - two**(-p - 1))
    distance: Decimal = two**(e - p)
    return Segment(e, min_val.normalize(), max_val.normalize(), distance.normalize())


@segment_params.register
def segment_params_float(f: float, ctx: Context) -> Segment:
    """Calculate the parameters of the double-precision floating-point segment containing
    the given floating-point number 'f'

    See base case 'segment_params' for more details
    """
    setcontext(ctx)
    bits: str = from_decimal_to_binary(f)[0]
    unbiased_exp: int = unpack_double_precision_fp(str_to_list(bits))[3]
    return segment_params(unbiased_exp, ctx)


def tabulate_esegments(start: int, end: int):
    """Pretty-print parameters of the segments corresponding to the given range [start, end-1]
    """
    segments = [segment_params(e, Context(prec=400, rounding=ROUND_HALF_UP)) for e in range(start, end)]
    max_e = 5
    max_min = max([len(str(segment.min_val)) for segment in segments])
    max_max = max([len(str(segment.max_val)) for segment in segments])
    max_distance = max([len(str(segment.distance)) for segment in segments])

    header = f"| e{'':{max_e-1}}| min{'':{max_min-3}}| max{'':{max_max-3}}| distance{'':{max_distance-len('distance') if len('distance') < max_distance else 1}}|"
    row_separator = f"|{'-' * (max_e + max_min + max_max + max_distance + 12)}|"

    def prettify(r):
        return f"| {r[0]:^{max_e}}| {r[1]:{max_min}}| {r[2]:{max_max}}| {r[3]:{max_distance+5}}|"

    print('\n')
    print(header)
    print(row_separator)
    print("\n".join([prettify(segment) for segment in segments]))
    print('\n')


def next_binary_fp(bits: List[int]) -> List[int]:
    """Return the binary representation of the next double-precision floating-point number

    Raise OverflowError if the argument or the resulting value is either 'Infinity' or 'NaN'

    Args:
        bits (list[int]): bit pattern of the original double-precision floating-point number

    Returns:
        list[int]: bit pattern of the next double-precision floating-point number
    """
    bits = bits.copy()
    _, fraction_bits, exponent_bits, _ = unpack_double_precision_fp(bits)
    check_infinity_or_nan(fraction_bits, exponent_bits)

    if next_binary_value(fraction_bits):
        # overflow in fraction, increase exponent
        next_binary_value(exponent_bits)
        check_infinity_or_nan(fraction_bits, exponent_bits)

    bits[12:] = fraction_bits
    bits[1:12] = exponent_bits
    return bits


def fp_gen(seed: float) -> Generator[FP, None, None]:
    """Return a generator of double-precision floating-point numbers as defined by IEEE 754.

    The numbers are generated in ascending order by incrementing the binary representation of 
    the given seed one unit at a time. In case of reaching the values "Infinity" or "NaN", the generator
    throws an OverflowError.

    Args:
        seed (float): initial floating-point number

    Yields:
        FP: double-precision floating-point number
    """
    assert seed >= 0, "seed must be positive or zero"

    bits = from_decimal_to_binary(seed)[0]
    bits = str_to_list(bits)
    fp = FP.from_binary(bits)

    while True:
        yield fp
        bits = next_binary_fp(bits)
        fp = FP.from_binary(bits)


def next_n_binary_fp(seed: float, n: int) -> list[FP]:
    """Convenience function to return the next n double-precision floating-point numbers in ascending order

    See next_binary_fp() for more details
    """
    assert n > 0, "n must be a positive integer"
    fp_generator = fp_gen(seed)
    return [next(fp_generator) for _ in range(n)]


def map_ndigit_decimals_to_fp(fp: FP, d: int):
    """Return the list of d-digit decimal numbers that map to the given double-precision floating-point number
    The list is ordered in ascending order

    Args:
        fp (FP): double-precision floating-point number
        d (int): number of significant digits to be considered

    Returns:
        tuple[int, Decimal, list[Decimal]]: number of d-digit decimal numbers that map to the given double-precision floating-point number, 
        the distance between consecutive d-digit numbers, and the list of numbers
    """
    _, digits, exp = fp.exact_decimal.normalize().as_tuple()
    match exp:
        case str(exp):
            raise ValueError("dec must be a finite number")
        case _:
            exp = int(exp)

    dec_len = len(digits)
    incr = Decimal(10)**(exp + dec_len - d)

    # first d-digit number smaller than the given number
    lower_d_digit_number = Decimal(f"{str(fp.exact_decimal)[:(d if exp >= 0 else d+1)]}{'0' * (dec_len - d)}")
    # first d-digit number greater than the given number
    upper_d_digit_number = lower_d_digit_number + incr

    numbers = []
    # checking d-digit numbers smaller than the given number
    while float(lower_d_digit_number) == fp.fp:
        numbers.append(lower_d_digit_number)
        lower_d_digit_number -= incr

    # checking d-digit numbers greater than the given number
    while float(upper_d_digit_number) == fp.fp:
        numbers.append(upper_d_digit_number)
        upper_d_digit_number += incr

    return (len(numbers), incr, sorted(numbers))


def identify_surrounding_powers_of_2_and_10(x: float) -> List[Tuple[int, int]]:
    """Given a float, calculate the nearest powers of 10 and 2 and return them in ascending order

    72057594037927956 -> [(10, 16), (2, 56), (10, 17), (2, 57)] that reads: 
    10^16 < 2^56 < 72057594037927956 < 10^17 < 2^57

    https://www.exploringbinary.com/how-the-positive-powers-of-ten-and-two-are-interleaved/
    https://www.exploringbinary.com/7-bits-are-not-enough-for-2-digit-accuracy/
    """
    previous_power_of_2 = floor(log2(x))
    previous_power_of_10 = floor(log10(x))
    next_power_of_2 = previous_power_of_2 + 1
    next_power_of_10 = previous_power_of_10 + 1
    return sorted([(2, previous_power_of_2), (10, previous_power_of_10), (2, next_power_of_2), (10, next_power_of_10)], key=lambda x: x[0]**x[1])


def is_segment_precision(start: Decimal, end: Decimal, d: int) -> bool:
    """Determines whether the precision of the segment [start, end] is 'd' digits

    The precision of the segment is 'd' digits if each d-digit number in the segment maps to
    a different double-precision floating-point number
    """
    generator = fp_gen(float(start))
    current_fp: FP = next(generator)
    num_mapped_decimals = map_ndigit_decimals_to_fp(current_fp, d)[0]
    while num_mapped_decimals < 2 and current_fp.exact_decimal < end:
        current_fp = next(generator)
        num_mapped_decimals = map_ndigit_decimals_to_fp(current_fp, d)[0]

    if num_mapped_decimals < 2:
        return True

    print(f"{num_mapped_decimals} {d}-digit decimals mapped to {current_fp.exact_decimal.normalize()}")
    return False


if __name__ == "__main__":
    # print(mpf(7.1))
    # print(to_double_precision_floating_point_binary(7.2))
    # print(to_single_precision_floating_point_binary_manual(52))
    # print(double_precision_significant_digits("72057594037927956"))
    # print(identify_range(1023.999999999999887))
    # print(identify_range(72057594037927956))
    # print(segment_params(9, Context(prec=100, rounding=ROUND_HALF_UP)))
    # print(explore_segment_precision(mpf(1023), mpf(1024), mpf(1e-18)))
    # fp_generator = fp_gen(72057594037927870)
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(next(fp_generator))
    # print(get_n_fp(72057594037927956, 3))
    # print(normalise_to_significant_digits(72057594037927956, 16))
    # print(normalise_to_significant_digits(0.0454, 1))
    print(map_ndigit_decimals_to_fp(FP.from_decimal(Decimal('72057594037927968')), 17))

    # decimal = 72057594037927945
    # binary_val = to_double_precision_floating_point_binary(decimal)[0]
    # exact_decimal = to_exact_decimal(str_to_list(binary_val))
    # print(decimal)
    # print(binary_val)
    # print(exact_decimal)
    # tabulate_esegments(50,59)
    print(is_segment_precision(Decimal(72057594037927945), Decimal(72057594037928000), 15))
