import java.math.BigDecimal;
import java.math.MathContext;
import java.math.RoundingMode;
import static java.lang.Math.*;

public class FloatingPointNumber {

    static int SINGLE_FP_EXPONENT_BIAS = 127;
    static BigDecimal exactDecimal(String singleFloatingPointBinary) {        
        BigDecimal mantissa = BigDecimal.ONE;
        BigDecimal half = new BigDecimal("0.5");
        String fraction = singleFloatingPointBinary.substring(9);
        for (int i = 0; i < fraction.length(); i++) {
            BigDecimal decimalValue = BigDecimal.valueOf(Long.parseLong(fraction.substring(i,i+1))).multiply(half.pow(i+1));
            mantissa = mantissa.add(decimalValue);
        }
        int exponent = Integer.valueOf(singleFloatingPointBinary.substring(1,9),2) - SINGLE_FP_EXPONENT_BIAS;
        int sign = fraction.charAt(0) == '0' ? 1 : -1;
        BigDecimal result = mantissa.multiply(BigDecimal.valueOf(2).pow(exponent, new MathContext(50, RoundingMode.HALF_EVEN)));
        return result.multiply(BigDecimal.valueOf(sign));
    }

    static float float_pi(int numSteps) {
        float stepSize = 1.0f/numSteps;
        float sum = 0f;
        for (int i=0; i<numSteps; i++) {
            float x = (i+0.5f)*stepSize;
            sum += (4.0f/(1.0f+pow(x,2)));
        }
        return sum*stepSize;
    }
    
    static double double_pi(int numSteps) {
        double stepSize = 1.0d/numSteps;
        double sum = 0d;
        for (int i=0; i<numSteps; i++) {
            double x = (i+0.5d)*stepSize;
            sum += (4.0d/(1.0d+pow(x,2)));
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
        //System.out.println(exactDecimal("00111110101010101010101010101011"));        
        int numSteps = (int)1e9;
        int precision = 24;
        System.out.println("Calculating pi with " + numSteps + " steps and precision " + precision + " digits:");
        var t0 = System.currentTimeMillis();
        System.out.println(float_pi(numSteps));
        System.out.println("" + (System.currentTimeMillis() - t0) / 1000.0 + " seconds");
        t0 = System.currentTimeMillis();
        System.out.println(double_pi(numSteps));
        System.out.println("" + (System.currentTimeMillis() - t0) / 1000.0 + " seconds");
        t0 = System.currentTimeMillis();
        System.out.println(arbitrary_precision_pi(numSteps, precision));
        System.out.println("" + (System.currentTimeMillis() - t0) / 1000.0 + " seconds");
    }
}

