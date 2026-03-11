import java.util.*;

/**
 * FloatingPointPrecision.java
 *
 * Java uses IEEE 754 double-precision (64-bit) floating point format.
 * This experiment reveals common surprises with floating-point arithmetic,
 * precision loss, rounding errors, and special values like NaN and infinities.
 *
 * Key concepts: binary representation, rounding errors, NaN semantics,
 * positive/negative zero, and safe comparison techniques.
 */
public class FloatingPointPrecision {
    public static void main(String[] args) {
        System.out.println("=== Floating Point Precision Experiments ===\n");

        // Experiment 1: The classic 0.1 + 0.2 != 0.3
        System.out.println("1. The infamous 0.1 + 0.2 problem:");
        double sum = 0.1 + 0.2;
        System.out.println("   0.1 + 0.2 = " + sum);
        System.out.println("   Expected: 0.3");
        System.out.println("   Equals 0.3? " + (sum == 0.3));
        System.out.println("   Actual value: " + String.format("%.20f", sum));
        System.out.println("   Why: 0.1 and 0.2 cannot be represented exactly in binary");

        // Experiment 2: Why decimal fractions fail in binary
        System.out.println("\n2. Binary representation of decimal fractions:");
        System.out.println("   0.1 in binary is a repeating fraction (like 1/3 in decimal)");
        System.out.println("   Stored as: 0.10000000000000000555...");
        System.out.println("   0.2 in binary also repeats:");
        System.out.println("   Stored as: 0.20000000000000001110...");
        System.out.println("   Their sum accumulates the rounding errors!");

        // Experiment 3: Precision limits with large numbers
        System.out.println("\n3. Precision loss with large numbers:");
        double large = 10_000_000.0;  // 10 million
        double smallAddition = 0.1;
        double result = large + smallAddition;
        System.out.println("   10,000,000.0 + 0.1 = " + result);
        System.out.println("   The small addition is LOST!");
        System.out.println("   Why: double has ~15-16 decimal digits of precision.");
        System.out.println("         When adding numbers of very different magnitudes,");
        System.out.println("         the smaller one gets rounded away.");

        // Experiment 4: NaN (Not a Number) behavior
        System.out.println("\n4. NaN is not equal to ANYTHING, including itself:");
        double nan = Double.NaN;
        System.out.println("   Double.NaN == Double.NaN? " + (nan == nan));
        System.out.println("   Double.NaN != Double.NaN? " + (nan != nan));
        System.out.println("   This is by design! Use Double.isNaN() to check.");
        System.out.println("   Double.isNaN(Double.NaN) = " + Double.isNaN(nan));
        System.out.println("   NaN arises from: 0.0/0.0, sqrt(-1), etc.");

        // Experiment 5: Operations with NaN propagate NaN
        System.out.println("\n5. NaN propagates through calculations:");
        double nanProp = 5.0 + Double.NaN;
        System.out.println("   5.0 + NaN = " + nanProp);
        double nanMul = 10.0 * Double.NaN;
        System.out.println("   10.0 * NaN = " + nanMul);
        System.out.println("   Any arithmetic with NaN yields NaN.");

        // Experiment 6: Positive and negative zero
        System.out.println("\n6. Positive zero vs Negative zero:");
        double posZero = 0.0;
        double negZero = -0.0;
        System.out.println("   0.0 == -0.0? " + (posZero == negZero));  // true!
        System.out.println("   1.0 /  0.0 = " + (1.0 / posZero));
        System.out.println("   1.0 / -0.0 = " + (1.0 / negZero));
        System.out.println("   They're equal according to ==");
        System.out.println("   But 1.0/-0.0 yields NEGATIVE INFINITY!");
        System.out.println("   Use Double.compare(0.0, -0.0) to distinguish.");
        System.out.println("   Double.compare(0.0, -0.0) = " + Double.compare(posZero, negZero));

        // Experiment 7: Infinity behavior
        System.out.println("\n7. Infinity from division by zero:");
        double posInf = 1.0 / 0.0;
        double negInf = -1.0 / 0.0;
        System.out.println("   1.0 /  0.0 = " + posInf);
        System.out.println("   -1.0 / 0.0 = " + negInf);
        System.out.println("   Double.POSITIVE_INFINITY = " + Double.POSITIVE_INFINITY);
        System.out.println("   Double.NEGATIVE_INFINITY = " + Double.NEGATIVE_INFINITY);
        System.out.println("   Infinity arithmetic:  inf + 5 = " + (posInf + 5));
        System.out.println("                       inf - inf = " + (posInf - posInf));
        System.out.println("   Note: inf - inf yields NaN (indeterminate)");

        // Experiment 8: Comparing floating-point numbers safely
        System.out.println("\n8. Safe comparison with epsilon tolerance:");
        double a = 0.1 + 0.2;
        double b = 0.3;
        double epsilon = 1e-10;
        boolean close = Math.abs(a - b) < epsilon;
        System.out.println("   Absolute difference |0.1+0.2 - 0.3| = " + Math.abs(a - b));
        System.out.println("   Is close (epsilon=1e-10)? " + close);
        System.out.println("   Better approach: compare relative to magnitude");
        System.out.println("   Or use BigDecimal for exact decimal arithmetic.");

        // Experiment 9: Accumulated rounding errors
        System.out.println("\n9. Accumulated rounding error in loops:");
        double total = 0.0;
        for (int i = 0; i < 10; i++) {
            total += 0.1;
        }
        System.out.println("   Sum of 10 * 0.1 = " + total);
        System.out.println("   Expected: 1.0");
        System.out.println("   Actual: " + String.format("%.20f", total));
        System.out.println("   Error: " + (total - 1.0));
        System.out.println("   Tiny error -- but it's there!");

        // Experiment 10: Float vs Double precision comparison
        System.out.println("\n10. Float (32-bit) has much less precision than Double (64-bit):");
        float f = 0.1f;
        double d = 0.1;
        System.out.println("   Float  0.1 stored as: " + String.format("%.20f", f));
        System.out.println("   Double 0.1 stored as: " + String.format("%.20f", d));
        System.out.println("   Note: Both are approximations, but double is ~15 digits vs float ~7");

        // Experiment 11: The "magic" ofulp(spacing)
        System.out.println("\n11. Machine epsilon and ulp (Unit in Last Place):");
        double one = 1.0;
        double next = Math.nextAfter(1.0, Double.POSITIVE_INFINITY);
        System.out.println("   Next double after 1.0: " + next);
        System.out.println("   Difference (ulp at 1.0): " + (next - one));
        System.out.println("   This is the smallest distinguishable increment.");
        System.out.println("   For float, ulp(1.0) ≈ 1.19e-7; for double, ≈ 2.22e-16");

        // Experiment 12: Strictfp for consistent floating-point behavior
        System.out.println("\n12. strictfp keyword for reproducible results:");
        System.out.println("   Without strictfp, JVM may use extended precision (80-bit on x86)");
        System.out.println("   strictfp forces strict IEEE 754 semantics on all platforms");
        System.out.println("   Use: strictfp double calculation() { ... }");

        // Takeaway message
        System.out.println("\n=== Takeaways ===");
        System.out.println("• Never compare floats/doubles with ==");
        System.out.println("• Use epsilon tolerance or BigDecimal for exact decimals");
        System.out.println("• Understand NaN, infinities, and signed zeros");
        System.out.println("• Accumulated errors matter in loops/sums");
        System.out.println("• Use strictfp for portable, reproducible results");
    }
}
