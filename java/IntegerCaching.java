/**
 * IntegerCaching.java
 *
 * Java caches Integer objects for values -128 to 127 (JLS 5.1.7).
 * When you autobox an int in that range, you get the cached instance.
 * Outside that range, a new object is created each time.
 *
 * This leads to == comparisons that work "by accident" for small numbers
 * but fail for larger ones -- a classic source of subtle bugs.
 */
public class IntegerCaching {
    public static void main(String[] args) {
        System.out.println("=== Integer Caching Experiments ===\n");

        // Experiment 1: Small values are cached
        Integer a = 127;
        Integer b = 127;
        System.out.println("1. Integer 127 == 127:");
        System.out.println("   a == b       : " + (a == b));       // true - cached
        System.out.println("   a.equals(b)  : " + a.equals(b));   // true

        // Experiment 2: Values above 127 are NOT cached
        Integer c = 128;
        Integer d = 128;
        System.out.println("\n2. Integer 128 == 128:");
        System.out.println("   c == d       : " + (c == d));       // false! New objects each time
        System.out.println("   c.equals(d)  : " + c.equals(d));   // true

        // Experiment 3: Negative boundary
        Integer e = -128;
        Integer f = -128;
        Integer g = -129;
        Integer h = -129;
        System.out.println("\n3. Boundary at -128 and -129:");
        System.out.println("   -128 == -128 : " + (e == f));       // true - cached
        System.out.println("   -129 == -129 : " + (g == h));       // false - outside cache

        // Experiment 4: valueOf uses the cache, new Integer does NOT
        Integer i = Integer.valueOf(42);
        Integer j = Integer.valueOf(42);
        @SuppressWarnings("deprecation")
        Integer k = new Integer(42);
        System.out.println("\n4. valueOf(42) vs new Integer(42):");
        System.out.println("   valueOf == valueOf : " + (i == j));  // true
        System.out.println("   valueOf == new     : " + (i == k));  // false

        // Experiment 5: Unboxing + comparison trap
        // When comparing Integer to int, unboxing occurs and == works correctly.
        // But Integer to Integer uses reference equality.
        Integer m = 200;
        int n = 200;
        Integer o = 200;
        System.out.println("\n5. Integer vs int vs Integer (value 200):");
        System.out.println("   Integer == int     : " + (m == n));  // true - m unboxed to int
        System.out.println("   Integer == Integer : " + (m == o));  // false - reference comparison

        // Experiment 6: Long, Short, Byte also have caches
        Long la = 127L;
        Long lb = 127L;
        Long lc = 128L;
        Long ld = 128L;
        System.out.println("\n6. Long caching (same -128..127 range):");
        System.out.println("   Long 127 == 127 : " + (la == lb));  // true
        System.out.println("   Long 128 == 128 : " + (lc == ld));  // false
    }
}
