# Java Surprising Behaviors -- Experiment Findings

Run each experiment with:
```
javac java/FileName.java -d /tmp/java_exp && java -cp /tmp/java_exp FileName
```

---

## 1. String Pooling (`StringPooling.java`)

**What**: Explores how Java's string pool, the `intern()` method, and compile-time constant folding affect `==` comparisons between strings.

**Expected**: `==` should never work for string comparison -- everyone knows to use `.equals()`.

**Actual**: `==` *does* work for string literals, compile-time concatenation of literals, and even concatenation involving `final` variables. It fails for runtime concatenation and `new String()`. The `final` keyword on a local variable makes it a compile-time constant, so `final String x = "hel"; x + "lo"` is folded to `"hello"` at compile time and goes into the pool.

**Why**: The JVM maintains a string intern pool. All string literals are automatically interned (JLS 3.10.5). The compiler performs constant folding on compile-time constant expressions (JLS 15.28), which includes `final` local variables initialized with literals. `new String()` always allocates on the heap, bypassing the pool. `intern()` manually adds a string to the pool or returns the existing pool reference.

---

## 2. Integer Caching (`IntegerCaching.java`)

**What**: Tests `==` comparison on autoboxed Integer values at various ranges, including the cache boundary at -128/127.

**Expected**: Either `==` always works (like primitives) or never works (like objects) for Integer comparisons.

**Actual**: `==` works for values -128 to 127 but fails for 128 and above (and -129 and below). `Integer.valueOf()` uses the cache; `new Integer()` does not. When comparing `Integer` to `int`, the Integer is unboxed and `==` works correctly regardless of value. Long, Short, and Byte have the same -128..127 cache.

**Why**: JLS 5.1.7 requires that autoboxing cache Integer values in the range [-128, 127]. `Integer.valueOf(int)` returns cached instances for this range. Outside the range, each boxing creates a new object. When one operand is primitive `int`, the other is unboxed via `intValue()`, converting to a primitive comparison.

---

## 3. Generic Type Erasure (`GenericErasure.java`)

**What**: Demonstrates that Java generics are erased at runtime, allowing type-unsafe operations that defeat the generic type system.

**Expected**: `List<String>` and `List<Integer>` are different types that the runtime can distinguish and enforce.

**Actual**: At runtime, both are just `java.util.ArrayList` -- same class. You can cast `List<Integer>` to `List<String>` without any runtime exception. Through raw types, you can add an Integer to a `List<String>`. The error only surfaces later when you try to read the element as a String (ClassCastException at the implicit cast inserted by the compiler). The compiler also generates synthetic "bridge methods" to maintain polymorphism after erasure.

**Why**: Java generics use type erasure for backward compatibility with pre-generics code (Java < 5). The compiler inserts type checks and casts at compile time, then strips all generic type parameters from the bytecode. At runtime, there is no `List<String>` -- only `List`. This is why you cannot use `instanceof` with parameterized types, cannot create generic arrays, and cannot overload methods that differ only in their generic type parameters.

---

## 4. Concurrent Modification (`ConcurrentModification.java`)

**What**: Shows when `ConcurrentModificationException` is thrown during collection iteration, when it is silently skipped, and how to safely modify collections during iteration.

**Expected**: Modifying a collection during iteration always throws `ConcurrentModificationException`.

**Actual**: Removing the second-to-last element from an ArrayList during for-each iteration does NOT throw an exception. The loop silently skips the last element. This happens because after removal, the iterator's cursor equals the new size, so `hasNext()` returns false before `next()` gets a chance to check modCount. `CopyOnWriteArrayList` never throws -- its iterator works on a snapshot. The correct approaches are `Iterator.remove()` or `removeIf()`.

**Why**: The fail-fast mechanism compares a `modCount` field in `next()`, not in `hasNext()`. When removing element at index `size-2`, the cursor advances to `size-1` (which equals the new size after removal), so `hasNext()` returns false and the loop ends without calling `next()`. This is a known gotcha in the ArrayList iterator implementation. CopyOnWriteArrayList copies the entire backing array on each write, so iterators see a stable snapshot.

---

## 5. Finally Block Traps (`FinallyTrap.java`)

**What**: Explores how `finally` blocks interact with return values, exceptions, and control flow in non-obvious ways.

**Expected**: A `return` in `try` returns that value; `finally` is just for cleanup.

**Actual**: A `return` in `finally` completely overrides the `return` in `try`. Even more dangerously, a `return` in `finally` silently swallows exceptions thrown in the `try` block -- the exception is lost forever. Modifying a local variable in `finally` (without a return) does NOT change the return value, because the return value was already captured. An exception in `finally` replaces any exception from `try`. `System.exit()` is one of the very few things that prevents `finally` from executing.

**Why**: The JVM specification (JVMS 3.13) states that `finally` is implemented by duplicating the finally code at each exit point. When `finally` has its own `return`, it replaces the pending return value or exception on the operand stack. The return value from `try` is stored in a temporary local variable before `finally` executes; if `finally` doesn't return, that stored value is used. If `finally` does return, its value wins. `System.exit()` calls `Runtime.halt()` which terminates the JVM process immediately.

---

## 6. Autoboxing Performance (`AutoboxingPerf.java`)

**What**: Benchmarks the performance difference between using primitive `long` vs boxed `Long` in a tight summation loop of 10 million iterations.

**Expected**: Boxed types might be slightly slower due to object overhead.

**Actual**: Boxed `Long` is typically 5-10x slower than primitive `long` (varies by JVM and hardware). The statement `Long sum += i` is actually `sum = Long.valueOf(sum.longValue() + (long)i)` -- creating a new Long object every iteration. With 10 million iterations, that means 10 million temporary Long objects that immediately become garbage.

**Why**: Autoboxing/unboxing inserts calls to `valueOf()` and `longValue()` at every arithmetic operation. Each `valueOf()` call outside the [-128, 127] cache range allocates a new object on the heap. This causes massive GC pressure and cache thrashing. The JIT compiler can sometimes optimize this away, but not reliably. This is why Effective Java (Item 6) warns: "prefer primitives to boxed primitives, and watch out for unintentional autoboxing."

---

## 7. Array Covariance (`ArrayCovariance.java`)

**What**: Demonstrates that Java arrays are covariant (String[] is-a Object[]) while generics are invariant (List<String> is NOT List<Object>), and the runtime consequences.

**Expected**: If the compiler allows assigning String[] to Object[], then storing any Object in it should work.

**Actual**: The compiler allows `Object[] o = new String[5]` and `o[0] = 42` without complaint. But at runtime, the JVM throws `ArrayStoreException` because the actual array type is String[]. This means arrays have a type hole -- the compiler can't catch the error, so it must be caught at runtime. Generics fix this by being invariant: `List<Object> l = stringList` simply won't compile. Wildcards (`? extends`) provide controlled covariance that prevents writing.

**Why**: Arrays were designed in Java 1.0 (pre-generics) and needed covariance for utility methods like `Arrays.sort(Object[])`. Every array object stores its component type at runtime and checks it on every store operation (`aastore` bytecode). This per-store runtime check is the cost of covariance. Generics (Java 5) learned from this mistake and chose invariance, using wildcards for flexibility. This is also why generic array creation (`new List<String>[10]`) is illegal -- it would combine the type holes of both systems.

---

## 8. VarArgs and Overload Resolution (`VarArgsOverload.java`)

**What**: Explores Java's three-phase method overload resolution and how it interacts with varargs, autoboxing, and widening.

**Expected**: The method with the "closest" parameter type wins, and the rules are intuitive.

**Actual**: Java uses a strict three-phase resolution: (1) match without boxing or varargs, (2) match with boxing but no varargs, (3) match with both. This means widening (`int` to `long`) beats autoboxing (`int` to `Integer`), and autoboxing beats varargs (`int` to `int...`). Passing `null` to overloaded varargs methods (`String...` vs `Integer...`) causes a compile-time ambiguity error. An `int[]` argument matches an `int[]` parameter exactly, but individual `int` arguments match `int...` varargs. A single-argument non-varargs `Object` parameter beats `Object...` varargs.

**Why**: JLS 15.12.2 defines the three-phase process to maintain backward compatibility. Phase 1 uses only subtyping (widening), matching pre-Java-5 behavior. Phase 2 adds autoboxing. Phase 3 adds varargs. A method found in an earlier phase always wins over one found in a later phase. For null arguments with multiple applicable varargs methods, neither is more specific than the other, causing ambiguity. The `int[]` vs `int...` distinction exists because varargs is syntactic sugar -- `int...` IS `int[]` at the bytecode level, but the compiler distinguishes them during resolution.

---

## 9. Suppressed Exceptions (`SuppressedExceptions.java`)

**What**: Demonstrates how try-with-resources preserves both primary and close() exceptions via the suppressed mechanism, while plain try-finally silently loses the original exception.

**Expected**: Both try-finally and try-with-resources handle cleanup exceptions the same way.

**Actual**: In plain try-finally, if both the try block and finally block throw exceptions, the try block's exception is LOST -- only the finally exception propagates. Try-with-resources (Java 7+) preserves the try block's exception as primary and attaches close() exceptions as "suppressed" exceptions via getSuppressed(). Multiple resources are closed in reverse declaration order, and each close() exception is suppressed.

**Why**: The JVM can only propagate one exception. Pre-Java 7, finally's exception replaced try's. Try-with-resources generates bytecode that catches close() exceptions and calls addSuppressed() on the primary exception. If no primary exception exists, the close() exception becomes the primary. The Throwable.addSuppressed() API (Java 7) enables this chain.

---

## 10. Mutable HashMap Key (`MutableHashMapKey.java`)

**What**: Shows that mutating an object used as a HashMap key makes the entry unretrievable -- it becomes a "ghost" entry visible only by iteration.

**Expected**: HashMap entries are always retrievable by their key.

**Actual**: After mutating the key's id (which changes its hashCode), get() and containsKey() both return null/false, even though size() still shows 1. The entry is stuck in the wrong hash bucket. Even creating a new key with the original hash value can't find it. Breaking the equals/hashCode contract (equals without hashCode) allows "equal" objects to coexist as separate keys. IdentityHashMap uses == instead of equals(), keeping equal objects separate.

**Why**: HashMap uses hashCode() to determine the bucket at put() time. If the key's hashCode changes after insertion, the entry remains in the old bucket. get() computes the new hashCode, looks in the wrong bucket, and finds nothing. The equals/hashCode contract (JLS) requires that equal objects must have equal hashCodes; violating this breaks all hash-based collections.

---

## 11. Floating Point Precision (`FloatingPointPrecision.java`)

**What**: Explores IEEE 754 double-precision arithmetic quirks: rounding errors, NaN semantics, signed zeros, infinities, and safe comparison techniques.

**Expected**: Floating-point arithmetic is deterministic and `0.1 + 0.2` should equal `0.3`.

**Actual**: Many surprising behaviors:
- `0.1 + 0.2 = 0.30000000000000004` (not 0.3) due to binary representation limits
- `NaN` never equals anything, including itself: `NaN == NaN` is `false`
- `±0.0` are equal via `==`, but `1.0 / -0.0` yields negative infinity
- `Infinity - Infinity = NaN` (indeterminate)
- Precision loss: `10,000,000.0 + 0.1 = 10,000,000.0` (small value lost)
- Summing `0.1` ten times yields `0.9999999999999999`, not `1.0`
- `float` (32-bit) has ~7 decimal digits; `double` (64-bit) has ~15-16

**Why**: IEEE 754 binary floating-point stores numbers as `sign × mantissa × 2^exponent`. Decimal fractions like 0.1 are repeating binary fractions, so they're rounded to the nearest representable value. These rounding errors accumulate. Special values (NaN, ±Infinity, ±0.0) have dedicated bit patterns and follow specific rules to handle edge cases like division by zero or undefined operations. The limited mantissa bits cause precision to drop dramatically when adding numbers of very different magnitudes. The spacing between consecutive representable numbers (ulp) is about 2.22×10^-16 near 1.0 for doubles.
