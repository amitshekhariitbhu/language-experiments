/**
 * VarArgsOverload.java
 *
 * Java's method overload resolution with varargs and autoboxing creates
 * several surprising situations. The resolution follows a strict 3-phase
 * process (JLS 15.12.2):
 *
 * Phase 1: Try to match without autoboxing or varargs
 * Phase 2: Try to match with autoboxing but without varargs
 * Phase 3: Try to match with both autoboxing and varargs
 *
 * This ordering creates non-obvious method selections.
 */
public class VarArgsOverload {
    public static void main(String[] args) {
        System.out.println("=== VarArgs & Overloading Experiments ===\n");

        // Experiment 1: Specific type preferred over varargs
        System.out.println("1. Exact match vs varargs:");
        printExact("hello");        // Calls String version
        printExact("a", "b");       // Now calls varargs version

        // Experiment 2: Widening preferred over autoboxing
        System.out.println("\n2. Widening vs autoboxing:");
        wideOrBox(42);  // int 42: widened to long or boxed to Integer?
        // Answer: widened to long! Widening (Phase 1) beats boxing (Phase 2)

        // Experiment 3: Autoboxing preferred over varargs
        System.out.println("\n3. Autoboxing vs varargs:");
        boxOrVarargs(42);  // int 42: boxed to Integer or passed as int...?
        // Answer: boxed to Integer! Boxing (Phase 2) beats varargs (Phase 3)

        // Experiment 4: null is ambiguous with multiple varargs overloads
        System.out.println("\n4. Null argument with overloaded methods:");
        // ambiguous(null);  // Would cause compile error!
        // When null could match String[] or Integer[], it's ambiguous.
        System.out.println("   ambiguous(null) would not compile -- both String[] and Integer[] match");
        // Fix: cast the null
        ambiguous((String[]) null);  // Now it's clear

        // Experiment 5: Varargs with no arguments
        System.out.println("\n5. Varargs with zero arguments:");
        countArgs();            // 0 args
        countArgs(1);           // 1 arg
        countArgs(1, 2, 3);    // 3 args
        // The varargs array is never null when called with no args -- it's empty

        // Experiment 6: Passing an array to varargs
        System.out.println("\n6. Array vs individual args to varargs:");
        int[] arr = {10, 20, 30};
        printVarargs(arr);         // Passes the array AS a single argument!
        printVarargs(10, 20, 30);  // Creates a new array from the 3 args

        // Experiment 7: Overloading with Object vs Object...
        System.out.println("\n7. Object parameter vs Object varargs:");
        objectOrVarargs("test");    // Which one wins?
        objectOrVarargs("a", "b");  // And this?

        // Experiment 8: Boolean autoboxing with varargs
        System.out.println("\n8. Boolean/boolean in overload resolution:");
        booleanTrap(true);  // boolean true: which method?
    }

    // --- Helper methods ---

    static void printExact(String s) {
        System.out.println("   printExact(String): " + s);
    }
    static void printExact(String... ss) {
        System.out.println("   printExact(String...): " + java.util.Arrays.toString(ss));
    }

    static void wideOrBox(long l) {
        System.out.println("   wideOrBox(long): " + l + " -- widening won over boxing!");
    }
    static void wideOrBox(Integer i) {
        System.out.println("   wideOrBox(Integer): " + i + " -- boxing won!");
    }

    static void boxOrVarargs(Integer i) {
        System.out.println("   boxOrVarargs(Integer): " + i + " -- boxing won over varargs!");
    }
    static void boxOrVarargs(int... is) {
        System.out.println("   boxOrVarargs(int...): " + java.util.Arrays.toString(is));
    }

    static void ambiguous(String... ss) {
        System.out.println("   ambiguous(String...): " + java.util.Arrays.toString(ss));
    }
    static void ambiguous(Integer... is) {
        System.out.println("   ambiguous(Integer...): " + java.util.Arrays.toString(is));
    }

    static void countArgs(int... args) {
        System.out.println("   countArgs received " + args.length + " args (array is " +
            (args == null ? "null" : "not null, length=" + args.length) + ")");
    }

    static void printVarargs(int... args) {
        System.out.println("   printVarargs(int...): length=" + args.length +
            " values=" + java.util.Arrays.toString(args));
    }
    // Note: int[] and int... are the same to the compiler, so we can't overload them.
    // This is part of the insight: varargs IS an array under the hood.

    static void objectOrVarargs(Object o) {
        System.out.println("   objectOrVarargs(Object): " + o + " -- exact match wins!");
    }
    static void objectOrVarargs(Object... os) {
        System.out.println("   objectOrVarargs(Object...): " + java.util.Arrays.toString(os));
    }

    static void booleanTrap(boolean b) {
        System.out.println("   booleanTrap(boolean): " + b + " -- primitive match, no boxing needed");
    }
    static void booleanTrap(Boolean b) {
        System.out.println("   booleanTrap(Boolean): " + b + " -- boxed match");
    }
}
