"""Single precision floating point number functions
"""

from math import isnan
from typing import List, Tuple
from fputil import next_binary_value

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
