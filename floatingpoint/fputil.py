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
