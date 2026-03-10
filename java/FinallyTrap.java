/**
 * FinallyTrap.java
 *
 * The `finally` block in Java always executes (with very few exceptions).
 * This leads to several surprising behaviors:
 * - A return in finally OVERRIDES a return in try/catch
 * - A return in finally SWALLOWS exceptions from try/catch
 * - System.exit() prevents finally from running
 * - The return VALUE from try is computed before finally runs, but
 *   finally's return replaces it
 */
public class FinallyTrap {
    public static void main(String[] args) {
        System.out.println("=== Finally Block Trap Experiments ===\n");

        // Experiment 1: Finally overrides try's return value
        System.out.println("1. Return in try vs return in finally:");
        System.out.println("   Result: " + returnFromFinally());
        // Returns "finally", not "try"!

        // Experiment 2: Finally swallows exceptions
        System.out.println("\n2. Exception in try, return in finally:");
        try {
            System.out.println("   Result: " + exceptionSwallowed());
            System.out.println("   The exception was completely swallowed!");
        } catch (Exception e) {
            System.out.println("   Exception propagated: " + e.getMessage());
        }

        // Experiment 3: Finally runs even after return
        System.out.println("\n3. Finally runs after return in try:");
        System.out.println("   Result: " + finallyAfterReturn());

        // Experiment 4: Return value is captured BEFORE finally modifies variable
        System.out.println("\n4. Modifying return variable in finally (no return in finally):");
        System.out.println("   Result: " + modifyInFinally());
        System.out.println("   The value 10 was captured before finally changed x to 20!");

        // Experiment 5: Exception in finally replaces exception in try
        System.out.println("\n5. Exception in try AND exception in finally:");
        try {
            exceptionInFinally();
        } catch (Exception e) {
            System.out.println("   Caught: " + e.getMessage());
            System.out.println("   The original 'try exception' was lost!");
        }

        // Experiment 6: Finally with break in a loop
        System.out.println("\n6. Finally with break in loop:");
        for (int i = 0; i < 3; i++) {
            try {
                if (i == 1) break;
                System.out.println("   Loop iteration: " + i);
            } finally {
                System.out.println("   Finally for i=" + i); // Runs even when break executes
            }
        }

        // Experiment 7: System.exit() prevents finally
        System.out.println("\n7. System.exit() in try block:");
        System.out.println("   (Skipping actual System.exit to keep program running)");
        System.out.println("   System.exit() is one of the ONLY ways to prevent finally.");
        System.out.println("   Others: infinite loop in try, thread kill, JVM crash.");
    }

    static String returnFromFinally() {
        try {
            return "try";      // This return value is computed...
        } finally {
            return "finally";  // ...but this one replaces it
        }
    }

    static String exceptionSwallowed() {
        try {
            throw new RuntimeException("I should propagate!");
        } finally {
            return "swallowed";  // Exception is completely lost
        }
    }

    static int finallyAfterReturn() {
        try {
            System.out.println("   (finally block executed here)");
            return 42;
        } finally {
            System.out.println("   (yes, finally ran even though try returned)");
        }
    }

    static int modifyInFinally() {
        int x = 10;
        try {
            return x;  // The value 10 is captured here
        } finally {
            x = 20;   // This modifies x but NOT the already-captured return value
            // No return statement here, so the try's return value (10) is used
        }
    }

    static void exceptionInFinally() {
        try {
            throw new RuntimeException("try exception");
        } finally {
            throw new RuntimeException("finally exception");
            // The "try exception" is silently discarded
        }
    }
}
