import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;
import static java.lang.Math.*;

public class FloatingPointNumber {

    static int EXPONENT_BIAS = 127;
    static BigDecimal binaryToFloat(String binary) {
        double d = 0.123456789123456789123456789123456789123456789;
        System.out.println(d);
        BigDecimal mantissa = BigDecimal.ONE;
        BigDecimal half = new BigDecimal("0.5");
        String fraction = binary.substring(9);
        for (int i = 0; i < fraction.length(); i++) {
            BigDecimal decimalValue = BigDecimal.valueOf(Long.parseLong(fraction.substring(i,i+1))).multiply(half.pow(i+1));
            mantissa = mantissa.add(decimalValue);
        }
        int exponent = Integer.valueOf(binary.substring(1,9),2) - EXPONENT_BIAS;
        int sign = fraction.charAt(0) == '0' ? 1 : -1;
        BigDecimal result = mantissa.multiply(BigDecimal.valueOf(2).pow(exponent, new MathContext(50, RoundingMode.HALF_EVEN)));
        return result.multiply(BigDecimal.valueOf(sign));
    }

    static double pi(int numSteps) {
        double stepSize = 1.0/numSteps;
        double sum = 0;
        for (int i=0; i<numSteps; i++) {
            double x = (i+0.5d)*stepSize;
            sum += (4.0/(1.0+pow(x,2)));
        }
        return sum*stepSize;
    }

    static BigDecimal arbitrary_precision_pi(int numSteps, int precision) {
        MathContext mc = new MathContext(precision, RoundingMode.HALF_UP);
        BigDecimal half = new BigDecimal("0.5");
        BigDecimal four = BigDecimal.valueOf(4);
        BigDecimal stepSize = BigDecimal.ONE.divide(BigDecimal.valueOf(numSteps), mc);
        BigDecimal sum = BigDecimal.ZERO;
        for (int i=0; i<numSteps; i++) {
            BigDecimal x = (BigDecimal.valueOf(i).add(half)).multiply(stepSize);
            sum = sum.add(four.divide(BigDecimal.ONE.add(x.pow(2)), mc));
        }
        return sum.multiply(stepSize);
    }
    public static void main(String[] args) {
        //System.out.println(binaryToFloat("00111110101010101010101010101011"));
        //System.out.println(BigDecimal.valueOf(4).divide(BigDecimal.valueOf(3), new MathContext(50, RoundingMode.HALF_EVEN)));
        //System.out.println(pi((int)1e9));
        System.out.println(arbitrary_precision_pi((int)1e9, 24));
    }
}
//1e7 - 3.956
//3.14159265358979
//3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280


//1e8 - 57.152
//3.1415926535897932
//3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280

//1e8 - 57.152
//3.1415926535897932
//3.1415926535897932  (p=18)
//3.1415926535897932 (24)
//3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280
