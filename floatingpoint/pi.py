from mpmath import *

def double_pi(num_steps):
    step_size = 1.0/num_steps
    sum = 0
    for i in range(num_steps):
        x = (i+0.5)*step_size
        sum += 4.0/(1.0+x**2)
    return step_size*sum

def arbitrary_precision_pi(num_steps, precision):    
    mp.dps = precision
    half = mpf("0.5")
    four = mpf(4)
    one = mpf(1)
    step_size = mpf(1)/num_steps
    sum = mpf(0)
    for i in range(num_steps):
        x = (i+half)*step_size
        sum += four/(one+x**2)
    return step_size*sum


num_steps=1e7
# print(double_pi(int(num_steps)))
print(arbitrary_precision_pi(int(num_steps), 17))

# 1e5
# 3.1415926535
# 3.1415926535
# 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280


# 1e7
# 3.1415926535897
# 3.14159265358979 (p=17, 3:10)
# 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280

# 1e7
# 3.14159265358979407179597671681 (p=30, 3:28)
# 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280

# 1e8
# 3.
# 3. (p=18, 3:10)
# 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280