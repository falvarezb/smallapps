from typing import List


def str_to_list(s: str) -> List[int]:
    """Convert a string made up of digits into a list of integers

    If any of the characters in the string is not a digit, raise a ValueError

    '1001' --> list[1,0,0,1]
    """
    return [int(digit) for digit in s]


def list_to_str(l: list) -> str:
    """Concatenate the elements of a list into a single string

    list[1,0,0,1] --> '1001'  
    """
    return "".join([str(e) for e in l])

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