from fp import *


def test_next_binary_overflow():
    bits = [1, 1, 1]
    assert next_binary(bits)    


def test_next_binary_success():
    bits = [1, 0, 1]
    assert not next_binary(bits)
    assert bits == [1, 1, 0]


def test_next_binary_fp_increase_fraction():
    bits = str_to_list(
        "0011111111110011001100110011001100110011001100110011001100110011")
    next_binary_fp(bits)
    assert list_to_str(
        bits) == "0011111111110011001100110011001100110011001100110011001100110100"


def test_next_binary_fp_increase_exponent():
    bits = str_to_list(
        "0011111111111111111111111111111111111111111111111111111111111111")
    next_binary_fp(bits)
    assert list_to_str(
        bits) == "0100000000000000000000000000000000000000000000000000000000000000"


def test_next_binary_fp_overflow():
    try:
        bits = str_to_list(
            "0111111111110011001100110011001100110011001100110011001100110011")
        next_binary_fp(bits)
    except OverflowError:
        assert True


def test_to_exact_decimal_positive():
    bits = str_to_list(
        "0011111111110011001100110011001100110011001100110011001100110011")
    assert to_exact_decimal(bits) == (mpf(
        '1.1999999999999999555910790149937383830547332763671875'),0)


def test_to_exact_decimal_negative():
    bits = str_to_list(
        "1011111111110011001100110011001100110011001100110011001100110011")
    assert to_exact_decimal(bits) == (mpf(
        '-1.1999999999999999555910790149937383830547332763671875'), 0)


def test_to_exact_decimal_nan():
    try:
        bits = str_to_list(
            "1111111111110011001100110011001100110011001100110011001100110011")
        to_exact_decimal(bits)
        assert False
    except OverflowError as e:
        assert e.args[0] == "NaN"


def test_to_exact_decimal_infinity():
    try:
        bits = str_to_list(
            "1111111111110000000000000000000000000000000000000000000000000000")
        to_exact_decimal(bits)
        assert False
    except OverflowError as e:
        assert e.args[0] == "Infinity"
