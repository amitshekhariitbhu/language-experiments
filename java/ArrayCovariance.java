import java.util.*;

/**
 * ArrayCovariance.java
 *
 * Java arrays are COVARIANT: String[] is a subtype of Object[].
 * This was a design decision made before generics existed (Java 1.0)
 * to allow methods like Arrays.sort(Object[]) to work with any array.
 *
 * The problem: the compiler allows you to store any Object in an Object[]
 * reference, but the runtime checks the actual array type and throws
 * ArrayStoreException if the types don't match.
 *
 * Generics (added in Java 5) are INVARIANT: List<String> is NOT a
 * subtype of List<Object>. This is the safer design.
 */
public class ArrayCovariance {
    public static void main(String[] args) {
        System.out.println("=== Array Covariance Experiments ===\n");

        // Experiment 1: Arrays are covariant -- this compiles and works
        String[] strings = {"hello", "world"};
        Object[] objects = strings;  // Legal! String[] IS-A Object[]
        System.out.println("1. String[] assigned to Object[] reference:");
        System.out.println("   objects[0] = " + objects[0]);

        // Experiment 2: ArrayStoreException at runtime
        System.out.println("\n2. Storing Integer into Object[] that's really String[]:");
        try {
            objects[0] = 42;  // Compiles fine! But runtime knows it's String[]
        } catch (ArrayStoreException e) {
            System.out.println("   ArrayStoreException: " + e.getMessage());
            System.out.println("   The JVM remembers the actual array type!");
        }

        // Experiment 3: Generics are invariant -- compile-time safety
        List<String> stringList = new ArrayList<>();
        // List<Object> objectList = stringList;  // COMPILE ERROR!
        // This is why generics are safer than arrays for type safety.
        System.out.println("\n3. Generics are invariant:");
        System.out.println("   List<String> cannot be assigned to List<Object>");
        System.out.println("   This prevents the problem at compile time!");

        // Experiment 4: Wildcards provide controlled covariance for generics
        List<String> names = Arrays.asList("Alice", "Bob");
        List<? extends Object> wildcardList = names;  // This IS allowed
        System.out.println("\n4. Wildcards (? extends) for controlled covariance:");
        System.out.println("   Can read: " + wildcardList.get(0));
        // wildcardList.add("Eve");  // COMPILE ERROR -- can't add to ? extends
        System.out.println("   But cannot add elements -- compiler prevents it");

        // Experiment 5: Array of subtypes
        Integer[] integers = {1, 2, 3};
        Number[] numbers = integers;  // Legal -- Integer[] IS-A Number[]
        System.out.println("\n5. Integer[] as Number[]:");
        try {
            numbers[0] = 3.14;  // Double is a Number, but array is Integer[]!
            System.out.println("   Stored 3.14 successfully");
        } catch (ArrayStoreException e) {
            System.out.println("   ArrayStoreException: can't store Double in Integer[]");
        }

        // Experiment 6: The practical consequence -- generic array creation
        System.out.println("\n6. Why you can't create generic arrays:");
        System.out.println("   If Java allowed List<String>[], then:");
        System.out.println("   List<String>[] lsa = new List<String>[1]; // hypothetical");
        System.out.println("   Object[] oa = lsa;           // array covariance");
        System.out.println("   oa[0] = List.of(42);         // would succeed at runtime");
        System.out.println("   String s = lsa[0].get(0);    // ClassCastException!");
        System.out.println("   Java prevents this by banning generic array creation.");

        // Experiment 7: Arrays.asList returns a fixed-size list backed by the array
        System.out.println("\n7. Bonus -- Arrays.asList covariance trap:");
        String[] arr = {"a", "b", "c"};
        List<String> asList = Arrays.asList(arr);
        arr[0] = "modified";
        System.out.println("   Modified array[0], list[0] = " + asList.get(0));
        System.out.println("   The list is backed by the original array!");
        try {
            asList.add("d");
        } catch (UnsupportedOperationException e) {
            System.out.println("   And you can't add to it: UnsupportedOperationException");
        }
    }
}
