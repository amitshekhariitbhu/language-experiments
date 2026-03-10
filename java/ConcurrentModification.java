import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * ConcurrentModification.java
 *
 * Demonstrates the difference between fail-fast iterators (ArrayList, HashMap)
 * and fail-safe/snapshot iterators (CopyOnWriteArrayList).
 *
 * Fail-fast iterators maintain a "modification count" (modCount). If the
 * collection is structurally modified during iteration (except through the
 * iterator's own methods), the iterator throws ConcurrentModificationException.
 *
 * CopyOnWriteArrayList takes a snapshot of the array when creating an iterator,
 * so modifications to the list don't affect in-progress iterations.
 */
public class ConcurrentModification {
    public static void main(String[] args) {
        System.out.println("=== Concurrent Modification Experiments ===\n");

        // Experiment 1: Classic ConcurrentModificationException
        System.out.println("1. Modifying ArrayList during for-each iteration:");
        List<String> list = new ArrayList<>(Arrays.asList("a", "b", "c", "d"));
        try {
            for (String s : list) {
                if (s.equals("b")) {
                    list.remove(s);  // Modifies list during iteration
                }
            }
        } catch (ConcurrentModificationException e) {
            System.out.println("   ConcurrentModificationException thrown!");
        }

        // Experiment 2: The sneaky case where it does NOT throw
        // Removing the second-to-last element can skip the exception because
        // after removal, hasNext() returns false before next() can detect
        // the modification. This is iterator implementation-specific.
        System.out.println("\n2. Removing second-to-last element (sneaky case):");
        List<String> list2 = new ArrayList<>(Arrays.asList("a", "b", "c"));
        try {
            for (String s : list2) {
                System.out.println("   Visiting: " + s);
                if (s.equals("b")) {
                    list2.remove(s);  // Removes "b", size goes from 3 to 2
                    // Iterator's cursor is now at index 2, which == new size 2
                    // so hasNext() returns false and loop ends -- no exception!
                }
            }
            System.out.println("   No exception! Final list: " + list2);
            System.out.println("   Notice: 'c' was never visited (silently skipped)!");
        } catch (ConcurrentModificationException e) {
            System.out.println("   ConcurrentModificationException thrown");
        }

        // Experiment 3: Safe removal using Iterator.remove()
        System.out.println("\n3. Safe removal using Iterator.remove():");
        List<String> list3 = new ArrayList<>(Arrays.asList("a", "b", "c", "d"));
        Iterator<String> it = list3.iterator();
        while (it.hasNext()) {
            String s = it.next();
            if (s.equals("b") || s.equals("d")) {
                it.remove();  // This is the correct way
            }
        }
        System.out.println("   After removing 'b' and 'd': " + list3);

        // Experiment 4: CopyOnWriteArrayList -- snapshot iterator
        System.out.println("\n4. CopyOnWriteArrayList (no ConcurrentModificationException):");
        CopyOnWriteArrayList<String> cowList = new CopyOnWriteArrayList<>(
            Arrays.asList("x", "y", "z")
        );
        System.out.print("   Iterating: ");
        for (String s : cowList) {
            System.out.print(s + " ");
            if (s.equals("x")) {
                cowList.add("w");       // Modifies the list
                cowList.remove("z");    // These changes don't affect the iterator
            }
        }
        System.out.println();
        System.out.println("   Iterator saw snapshot: [x, y, z]");
        System.out.println("   But actual list is now: " + cowList);

        // Experiment 5: HashMap also fails fast
        System.out.println("\n5. HashMap concurrent modification:");
        Map<String, Integer> map = new HashMap<>();
        map.put("a", 1);
        map.put("b", 2);
        map.put("c", 3);
        try {
            for (String key : map.keySet()) {
                if (key.equals("a")) {
                    map.put("d", 4);  // Structural modification during iteration
                }
            }
            System.out.println("   No exception (race condition in check)");
        } catch (ConcurrentModificationException e) {
            System.out.println("   ConcurrentModificationException on HashMap too!");
        }

        // Experiment 6: removeIf -- the modern safe way (Java 8+)
        System.out.println("\n6. removeIf (Java 8+ safe removal):");
        List<Integer> nums = new ArrayList<>(Arrays.asList(1, 2, 3, 4, 5, 6));
        nums.removeIf(n -> n % 2 == 0);  // Remove all even numbers
        System.out.println("   After removeIf(even): " + nums);
    }
}
