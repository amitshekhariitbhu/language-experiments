/**
 * StringPooling.java
 *
 * Demonstrates Java's string interning mechanism and the difference
 * between == (reference equality) and .equals() (value equality).
 *
 * Java maintains a "string pool" (intern pool) where string literals
 * are stored. When you write a string literal, the JVM checks the pool
 * first and reuses existing references. But strings created with `new`
 * bypass the pool, creating fresh heap objects.
 */
public class StringPooling {
    public static void main(String[] args) {
        System.out.println("=== String Pool Experiments ===\n");

        // Experiment 1: Literal vs literal
        String a = "hello";
        String b = "hello";
        System.out.println("1. Two literals 'hello':");
        System.out.println("   a == b       : " + (a == b));       // true - same pool reference
        System.out.println("   a.equals(b)  : " + a.equals(b));   // true

        // Experiment 2: Literal vs new String
        String c = new String("hello");
        System.out.println("\n2. Literal vs new String('hello'):");
        System.out.println("   a == c       : " + (a == c));       // false - different objects
        System.out.println("   a.equals(c)  : " + a.equals(c));   // true - same value

        // Experiment 3: intern() forces pool lookup
        String d = c.intern();
        System.out.println("\n3. After c.intern():");
        System.out.println("   a == d       : " + (a == d));       // true - intern returns pool ref
        System.out.println("   c == d       : " + (c == d));       // false - c is still the heap copy

        // Experiment 4: Compile-time constant folding
        // The compiler concatenates constant expressions at compile time,
        // so the result is a single literal that goes into the pool.
        String e = "hel" + "lo";
        System.out.println("\n4. Compile-time concatenation 'hel' + 'lo':");
        System.out.println("   a == e       : " + (a == e));       // true - compiler folds constants

        // Experiment 5: Runtime concatenation defeats pooling
        String f1 = "hel";
        String f2 = f1 + "lo";  // runtime concat, creates new object
        System.out.println("\n5. Runtime concatenation (variable + literal):");
        System.out.println("   a == f2      : " + (a == f2));      // false - not a compile-time constant

        // Experiment 6: final variables ARE compile-time constants
        final String g1 = "hel";
        String g2 = g1 + "lo";  // g1 is final, so compiler can fold this
        System.out.println("\n6. Final variable + literal concatenation:");
        System.out.println("   a == g2      : " + (a == g2));      // true - final makes it a constant

        // Experiment 7: The empty string is also pooled
        String h = "";
        String i = new String("").intern();
        System.out.println("\n7. Empty string pooling:");
        System.out.println("   \"\" == \"\".intern() : " + (h == i)); // true
    }
}
