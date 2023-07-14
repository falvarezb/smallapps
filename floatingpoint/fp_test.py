from fp import *


def test_next_binary_value_overflow():
    bits = [1, 1, 1]
    assert next_binary_value(bits)    


def test_next_binary_value_success():
    bits = [1, 0, 1]
    assert not next_binary_value(bits)
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


def test_next_binary_fp_argument_overflow():
    try:
        bits = str_to_list(
            "0111111111110011001100110011001100110011001100110011001100110011")
        next_binary_fp(bits)
        assert False
    except OverflowError as e:
        assert e.args[0] == "NaN"        

def test_next_binary_fp_result_overflow():
    try:
        bits = str_to_list(
            "0111111111101111111111111111111111111111111111111111111111111111")
        next_binary_fp(bits)
        assert False
    except OverflowError as e:
        assert e.args[0] == "Infinity"


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

def test_fp_gen():
    fp_generator = fp_gen(0.00000000000012343)
    assert next(fp_generator) == (0.00000000000012343, mpf('0.0000000000001234300000000000102340991445314016317948146994609714965918101370334625244140625'), -43)
    assert next(fp_generator) == (0.00000000000012343000000000004, mpf('0.00000000000012343000000000003547764811160377940497012878851013084613441606052219867706298828125'), -43)

def test_fp_gen_infinity():
    fp_generator = fp_gen(1.7976931348623157e+308)
    assert next(fp_generator) == (1.7976931348623157e+308, mpf('179769313486231570814527423731704356798070567525844996598917476803157260780028538760589558632766878171540458953514382464234321326889464182768467546703537516986049910576551282076245490090389328944075868508455133942304583236903222948165808559332123348274797826204144723168738177180919299881250404026184124858368'), 1023)
    try:
        next(fp_generator)
        assert False
    except OverflowError as e:
        assert e.args[0] == "Infinity"

def test_precision():
    # 7.100000000000003
    assert precision("7.100000000000003") == 16
    assert precision("7.1000000000000031974423109204508364200592041015625") == 50
    assert precision("7.1000000000000034345") == 16
    # 7.1
    assert precision("7.1000000000000000000000000000000000") == 16
    assert precision("7.09999999999999968") == 17
        