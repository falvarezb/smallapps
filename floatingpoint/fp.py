"""
Module with functions to manipulate the binary and decimal representations of floating-point 
numbers according to the IEEE 754 standard

Python's 'float' data type represents numbers as double-precision floating points. 
To get around this limitation, some functions in this module make use of the mpmath 
arbitrary-precision library.
"""

import struct
from math import log2, log10, floor, isnan
from functools import reduce
from typing import List, Tuple, Generator
from mpmath import mp, mpf, nstr

mp.dps = 100


def unpack_double_precision_fp(bits: List[int]) -> Tuple[int, List[int], List[int], int]:
    """Decompose the binary representation of a double-precision floating-point number into its elements: sign, fraction bits, exponent bits, unbiased exponent
    """
    exponent_bits = bits[1:12]
    biased_exp = int(list_to_str(exponent_bits), 2)
    double_precision_exponent_bias = 1023
    unbiased_exp = biased_exp - double_precision_exponent_bias
    fraction_bits = bits[12:]
    sign = 1 if bits[0] == 0 else -1
    return (sign, fraction_bits, exponent_bits, unbiased_exp)


def double_precision_significant_digits(decimal_repr: str) -> Tuple[int, str]:
    """Determines how many digits of the given decimal number are significant when represented 
    as a double-precision floating-point number

    According to https://www.exploringbinary.com/decimal-precision-of-binary-floating-point-numbers/: 

    "d-digit precision means that if we take a d-digit decimal number, convert it to b-bit floating-point, 
    and then convert it back to decimal, rounding to nearest to d digits, we will recover all of the original d-digit 
    decimal numbers. In other words, the d-digit number will round-trip."

    Return a tuple containing the shortest decimal representation that round-trips 
    (this is basically the value returned by the function 'float()') and the number of significant digits

    "7.1000000000000034345" --> (16, "7.100000000000003")
    """

    def trim_radix_point(s: str) -> str:
        """Remove radix point if it is the last character of the decimal representation
        """
        if s[-1] == '.':
            return s[:-1]
        return s

    def compute_num_digits(decimal_repr: str) -> int:
        # exclude radix point from the count
        return len(decimal_repr) - 1 if decimal_repr.find('.') > -1 else len(decimal_repr)

    def compute_exact_decimal(decimal_repr: str) -> mpf:
        double_precision_floating_point_number = float(decimal_repr)
        return mpf(double_precision_floating_point_number)

    def is_round_trip(decimal_repr: str) -> bool:
        num_digits = compute_num_digits(decimal_repr)
        exact_decimal = compute_exact_decimal(decimal_repr)
        rounded_exact_decimal = trim_radix_point(nstr(exact_decimal, num_digits, strip_zeros=False))
        return rounded_exact_decimal == decimal_repr

    def is_representative(floating_point_number: mpf, candidate: str):
        # check if candidate is a representative of the floating-point number mpf
        return floating_point_number == compute_exact_decimal(candidate)

    assert len(decimal_repr) > 0, "argument must not be empty"
    assert decimal_repr.find('e') == -1, "exponential notation not supported"

    representatives_found = []
    original_exact_decimal = compute_exact_decimal(decimal_repr)

    # find shortest representative
    representative = True
    while representative:
        representatives_found.append(decimal_repr)
        trimmed_decimal_repr = decimal_repr[:-1]
        trimmed_decimal_repr = trim_radix_point(trimmed_decimal_repr)

        # remove least significant digit and check if the result is still a representative
        decimal_repr = trimmed_decimal_repr
        representative = is_representative(original_exact_decimal, decimal_repr)
        if not representative:
            # check if any of the neighbours (other values with same number of digits) is a representative
            for i in range(0, 10):
                neighbour = decimal_repr[:-1] + str(i)
                representative = is_representative(original_exact_decimal, neighbour)
                if representative:
                    decimal_repr = neighbour
                    break

    # re-examine found representatives in reverse order to check if they round-trip
    for r in reversed(representatives_found):
        if is_round_trip(r):
            return (compute_num_digits(r), r)

        # if it does not round-trip, check if any of the neighbours does
        for i in range(0, 10):
            neighbour = r[:-1] + str(i)
            if is_representative(original_exact_decimal, neighbour) and is_round_trip(neighbour):
                return (compute_num_digits(neighbour), neighbour)

    return None


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
    """Convert a number in decimal representation to its corresponding double-precision 
    floating-point binary format

    The floating-point number is returned in binary and hexadecimal format

    7.2 --> ('0100000000011100110011001100110011001100110011001100110011001101', '0x401ccccccccccccd')    
    """
    packed = struct.pack('>d', number)
    bits = ''.join(format(byte, '08b') for byte in packed)
    hexrepr = hex(int(bits, 2))
    return (bits, hexrepr)


def round_to_nearest(bits: List[int], kth: int, is_any_following_digit_1: bool = None):
    """Round a binary integer to the k-th position (1,2.... counting from the left) using the rule "round to nearest, ties to even"

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
            return bits[:kth]
        else:
            is_any_following_digit_1 = len(list(filter(lambda x: x == 1, bits[kth + 1:]))) != 0 if is_any_following_digit_1 is None else is_any_following_digit_1
            if is_any_following_digit_1:
                # round up
                bits = bits[:kth]
                if next_binary_value(bits):
                    raise OverflowError()
                return bits
            else:
                # same distance, ties to even: round to the nearest even number (the one ending in 0)
                if bits[kth - 1] == 1:  # odd
                    bits = bits[:kth]
                    if next_binary_value(bits):
                        raise OverflowError()
                    return bits
                else:
                    # even
                    return bits[:kth]

    return bits


def to_single_precision_floating_point_binary_manual(decimal: float) -> Tuple[str, str]:
    """Manual implementation of the conversion of a decimal number to 
    its IEEE 754 single-precision floating-point binary representation

    The floating-point number is returned in binary and hexadecimal format

    52.0 --> ('01000010010100000000000000000000', '0x42500000')
    """

    # Handle special cases: positive/negative zero and infinities, NaN
    if decimal == 0.0:
        bits = '0' * 32
        return (bits, hex(int(bits, 2)))
    if decimal == float('-inf'):
        bits = '1' + '1' * 8 + '0' * 23
        return (bits, hex(int(bits, 2)))
    if decimal == float('inf'):
        bits = '0' + '1' * 8 + '0' * 23
        return (bits, hex(int(bits, 2)))
    if isnan(decimal):
        bits = '0' + '1' * 8 + '0' * 22 + '1'
        return (bits, hex(int(bits, 2)))

    # Convert the number to its IEEE 754 single-precision binary representation
    bits = []
    if decimal < 0:
        sign = '1'
        decimal = abs(decimal)
    else:
        sign = '0'

    exponent = 127  # Bias for single-precision floating-point exponent
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
    # Repeatedly multiply it by 2 and note the integer part of the result (0 or 1) as the next
    # binary digit after the binary point. 
    # Continue this process until the fractional part becomes zero or until you reach the
    # desired precision:
    # - if the fractional part becomes zero, the binary representation is exact. 
    # - if you reach thedesired precision, round the last bit to the nearest even number.
    # This algorithm works because by multiplying by 2, we shift the binary point
    # one position to the right, so that the factor of 2^-1 becomes the integer part.
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
        "".join([str(i) for i in fraction_bits]).ljust(23, '0')

    return (bits, hex(int(bits, 2)))


def to_exact_decimal(bits: List[int]) -> Tuple[mpf, int]:
    """Transform bit pattern representing a double-precision floating-point number to exact decimal

    In order to circumvate Python's floating-point arithmetic limitations, this function uses an
    arbitrary-precision library instead of Python's float

    Return a tuple containing the exact decimal representation and the unbiased exponent of 
    the binary format
    """
    sign, fraction_bits, exponent_bits, unbiased_exp = unpack_double_precision_fp(bits)
    check_infinity_or_nan(fraction_bits, exponent_bits)

    half = mpf('0.5')
    mantissa = mpf(1)
    for i in range(1, len(fraction_bits) + 1):
        place_value = fraction_bits[i - 1] * half**i
        mantissa += place_value
    return (sign * mantissa * mpf(2)**unbiased_exp, unbiased_exp)


def esegment_params(e: int) -> Tuple[int, str, str, str]:
    """Calculate parameters of the double-precision floating-point segment corresponding 
    to the value of 'e'

    Return tuple with the values: 
    - echo of e
    - exact decimal of the minimum fp in the segment
    - exact decimal of the maximum fp in the segment
    - distance between consecutive fp in the segment
    """
    p = 52
    two = mpf(2)
    (min_val, max_val) = [two**e, two**e * (two**(p + 1) - 1) / two**p]
    # distance = (max-min)/(2**p-1)
    distance = two**(e - p)
    prec = 200
    return (e, nstr(min_val, prec), nstr(max_val, prec), nstr(distance, prec))


def tabulate_esegments(start: int, end: int):
    """Pretty-print parameters of the segments corresponding to the given range [start, end-1]
    """
    segments = [esegment_params(e) for e in range(start, end)]
    max_e = 5
    max_min = max([len(segment[1]) for segment in segments])
    max_max = max([len(segment[2]) for segment in segments])
    max_distance = max([len(segment[3]) for segment in segments])

    header = f"| e{'':{max_e-1}}| min{'':{max_min-3}}| max{'':{max_max-3}}| distance{'':{max_distance-8}}|"
    row_separator = f"|{'-' * (max_e + max_min + max_max + max_distance + 7)}|"

    def prettify(r):
        return f"| {r[0]:^{max_e}}| {r[1]:{max_min}}| {r[2]:{max_max}}| {r[3]:{max_distance}}|"

    print('\n')
    print(header)
    print(row_separator)
    print("\n".join([prettify(segment) for segment in segments]))
    print('\n')


def next_binary_value(bits) -> bool:
    """Add 1 to the given bit pattern 'bits', assuming bits[0] is the MSB (most-significant bit)

    The argument is modified in place.
    Returns a boolean to indicate whether there has been overflow (True) or not (False)
    """
    i = len(bits) - 1
    while i >= 0 and bits[i] == 1:
        bits[i] = 0
        i -= 1
    if i >= 0:
        bits[i] = 1
        return False

    # overflow
    return True


def next_binary_fp(bits: List[int]) -> List[int]:
    """Return the binary representation of the next double-precision floating-point number

    Argument is modified in place
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


def fp_gen(seed: float) -> Generator[Tuple[float, mpf, int], None, None]:
    """Return a generator of double-precision floating-point numbers as defined by IEEE 754.

    The floating-point numbers generated are represented by its exact decimal representation 
    (there is a one-to-one correspondence between the binary format of a floating-point number 
    and its exact decimal representation)

    The tuple produced by the generator contains:
    - decimal representation up to the standard double-precision (around 16 digits)
    - exact decimal representation
    - unbiased exponent of the binary format


    The generator is seeded by a number in decimal representation that is passed 
    to the function as an argument:
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
        next_bits = next_binary_fp(bits)
        exact_decimal, exp = to_exact_decimal(next_bits)


def identify_range(x: float) -> List[Tuple[int, int]]:
    """Given a float, calculate the nearest powers of 10 and 2 and return them in ascending order

    72057594037927956 -> [(10, 16), (2, 56), (10, 17), (2, 57)] that reads: 
    10^16 < 2^56 < 72057594037927956 < 10^17 < 2^57
    """
    previous_power_of_2 = floor(log2(x))
    previous_power_of_10 = floor(log10(x))
    next_power_of_2 = previous_power_of_2 + 1
    next_power_of_10 = previous_power_of_10 + 1
    return sorted([(2, previous_power_of_2), (10, previous_power_of_10), (2, next_power_of_2), (10, next_power_of_10)], key=lambda x: x[0]**x[1])


def explore_segment_precision(start: mpf, end: mpf, precision: mpf) -> bool:
    current_mpf = start
    current_exact_decimal = mpf(float(current_mpf))

    while current_mpf < end:
        previous_exact_decimal = current_exact_decimal
        current_mpf = current_mpf + precision
        current_exact_decimal = mpf(float(current_mpf))
        if previous_exact_decimal == current_exact_decimal:
            return False
        print(current_mpf / end)
    return True


if __name__ == "__main__":
    # print(mpf(7.1))
    #print(to_double_precision_floating_point_binary(7.2))
    print(to_single_precision_floating_point_binary_manual(52))
    # print(double_precision_significant_digits("72057594037927955"))
    # print(identify_range(1023.999999999999887))
    # print(esegment_params(9))
    # print(explore_segment_precision(mpf(1023), mpf(1024), mpf(1e-18)))
    # fp_gen = fp_gen(1)
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))
    # print(next(fp_gen))


# decimal = 1.2
# binary_val = to_floating_point_binary(decimal,False)[0]
# exact_decimal = to_exact_decimal(binary_val)
# print(decimal)
# print(binary_val)
# print(exact_decimal)
    
    # tabulate_esegments(-10,11)
