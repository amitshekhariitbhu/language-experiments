/**
 * AutoboxingPerf.java
 *
 * Demonstrates the massive performance difference between primitive int
 * and boxed Integer in tight loops. Each autoboxing/unboxing operation
 * creates a new object (outside the -128..127 cache range), causing
 * enormous GC pressure and orders-of-magnitude slowdowns.
 */
public class AutoboxingPerf {
    public static void main(String[] args) {
        System.out.println("=== Autoboxing Performance Trap ===\n");

        final int ITERATIONS = 10_000_000;

        // Warm up the JIT
        for (int warmup = 0; warmup < 3; warmup++) {
            sumPrimitive(ITERATIONS);
            sumBoxed(ITERATIONS);
        }

        // Experiment 1: Primitive int sum
        System.out.println("1. Summing " + ITERATIONS + " numbers with primitive long:");
        long start1 = System.nanoTime();
        long result1 = sumPrimitive(ITERATIONS);
        long time1 = System.nanoTime() - start1;
        System.out.println("   Result: " + result1);
        System.out.println("   Time:   " + (time1 / 1_000_000.0) + " ms");

        // Experiment 2: Boxed Long sum (autoboxing trap)
        System.out.println("\n2. Summing " + ITERATIONS + " numbers with boxed Long:");
        long start2 = System.nanoTime();
        long result2 = sumBoxed(ITERATIONS);
        long time2 = System.nanoTime() - start2;
        System.out.println("   Result: " + result2);
        System.out.println("   Time:   " + (time2 / 1_000_000.0) + " ms");

        // Comparison
        double ratio = (double) time2 / time1;
        System.out.println("\n3. Performance comparison:");
        System.out.println("   Boxed is ~" + String.format("%.1f", ratio) + "x slower than primitive");
        System.out.println("   Each += on Long creates a new Long object!");

        // Experiment 3: Counting object allocations conceptually
        System.out.println("\n4. Why it's slow:");
        System.out.println("   Long sum += i  is actually:");
        System.out.println("   sum = Long.valueOf(sum.longValue() + (long)i)");
        System.out.println("   That's " + ITERATIONS + " Long objects created and garbage collected!");

        // Experiment 4: Equality comparison trap
        System.out.println("\n5. Equality comparison cost:");
        long eqStart1 = System.nanoTime();
        int eqCount1 = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            if (i == 500) eqCount1++;  // primitive comparison
        }
        long eqTime1 = System.nanoTime() - eqStart1;

        long eqStart2 = System.nanoTime();
        int eqCount2 = 0;
        for (int i = 0; i < ITERATIONS; i++) {
            Integer boxed = i;
            if (boxed.equals(500)) eqCount2++;  // method call + unboxing
        }
        long eqTime2 = System.nanoTime() - eqStart2;

        System.out.println("   Primitive == comparison: " + (eqTime1 / 1_000_000.0) + " ms (found " + eqCount1 + ")");
        System.out.println("   Boxed .equals() compare: " + (eqTime2 / 1_000_000.0) + " ms (found " + eqCount2 + ")");
    }

    static long sumPrimitive(int iterations) {
        long sum = 0;  // primitive
        for (int i = 0; i < iterations; i++) {
            sum += i;
        }
        return sum;
    }

    static long sumBoxed(int iterations) {
        Long sum = 0L;  // BOXED -- this is the trap!
        for (int i = 0; i < iterations; i++) {
            sum += i;   // unbox sum, add i, rebox result -- every iteration
        }
        return sum;
    }
}
