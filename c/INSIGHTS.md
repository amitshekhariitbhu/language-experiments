# C Language Experiments — Insights

## 1. Pointer Arithmetic Scaling (`pointer_arithmetic.c`)

**What**: Adding 1 to a pointer advances by `sizeof(type)` bytes, not 1 byte. Array indexing `a[i]` is syntactic sugar for `*(a+i)`, which means `i[a]` is also valid C.

**Expected**: `ptr + 1` advances the pointer by 1 byte. `2[arr]` should be a syntax error.

**Actual**: `int *p; p+1` advances by 4 bytes (sizeof(int)). `2[arr]` compiles and returns the same value as `arr[2]`. Pointer subtraction returns element count, not byte count. Multidimensional arrays decay to pointers-to-arrays, so `mat+1` advances by `sizeof(int[4])` = 16 bytes.

**Why**: The C standard defines pointer arithmetic in units of the pointed-to type (C17 6.5.6). Array indexing is defined as `a[i] == *(a+i)`, and since addition is commutative, `*(i+a) == i[a]`. This design lets pointer arithmetic work naturally with typed arrays without manual byte-offset calculations.

---

## 2. Undefined Behavior (`undefined_behavior.c`)

**What**: Signed integer overflow, reading uninitialized variables, strict aliasing violations, sequence point violations, and over-width shifts are all undefined behavior in C.

**Expected**: Signed overflow wraps around like unsigned. Uninitialized variables contain zero or random garbage. `*(uint32_t*)&float_val` reads the float's bits.

**Actual**: Signed overflow is UB — the compiler may assume it never happens and optimize accordingly (e.g., `if (x+1 > x)` becomes always-true). Uninitialized variable reads are UB, not just "random." Type punning via pointer cast violates strict aliasing (UB); `memcpy` is the correct approach. `i = i++ + i++` is UB because `i` is modified twice without an intervening sequence point.

**Why**: The C standard grants compilers freedom to optimize by declaring these behaviors undefined (C17 6.5/2, 6.5.6). Unsigned arithmetic wraps modulo 2^N (well-defined), but signed arithmetic has no such guarantee. The strict aliasing rule (C17 6.5/7) allows compilers to assume pointers of different types don't alias, enabling critical optimizations.

---

## 3. String Gotchas (`string_gotchas.c`)

**What**: C has no string type — just null-terminated `char` arrays. The `==` operator compares pointer addresses, not string contents. Adjacent string literals are silently concatenated.

**Expected**: `==` compares string contents. A `char[5]` initialized with "Hello" works fine. Forgetting a comma in a string array causes a compiler error.

**Actual**: `==` compares addresses, so two identical strings in different arrays compare as not-equal. `char[5] = "Hello"` has no room for the null terminator, causing string functions to read past the buffer. A forgotten comma between `"banana"` and `"cherry"` silently concatenates them to `"bananacherry"`, reducing the array from 4 to 3 elements with no warning.

**Why**: C's string model is minimal by design — strings are just byte arrays with a sentinel value. The `==` operator works on the pointer values that arrays decay to (C17 6.5.9). Implicit string literal concatenation (C17 6.4.5/5) was intended for splitting long strings across lines but creates subtle bugs when commas are omitted.

---

## 4. sizeof Surprises (`sizeof_surprises.c`)

**What**: `sizeof` behaves differently on arrays vs pointers, does not evaluate its operand, and character literals in C are `int`, not `char`.

**Expected**: `sizeof(array)` and `sizeof(pointer_to_array)` return the same value. `sizeof(x++)` increments `x`. `sizeof('A')` returns 1.

**Actual**: `sizeof(arr)` returns the full array size (e.g., 40 for `int[10]`), but `sizeof(ptr)` returns 8 (pointer size). `sizeof(x++)` does NOT increment `x` — the expression is never evaluated. `sizeof('A')` returns 4 (on most platforms) because character literals in C have type `int`, not `char`. Struct `{char, int, char, double}` occupies 24 bytes, not 14, due to alignment padding. Reordering fields to `{double, int, char, char}` reduces it to 16 bytes.

**Why**: `sizeof` is a compile-time operator (C17 6.5.3.4) that determines size from the type alone without evaluating the expression. Arrays carry size information in their type, but this is lost when they decay to pointers. Character literals have type `int` in C (unlike C++ where they're `char`). Struct padding ensures each member meets its alignment requirement (C17 6.7.2.1).

---

## 5. Integer Promotion Traps (`integer_promotion.c`)

**What**: Small integer types are promoted to `int` in expressions. Comparing signed and unsigned values converts the signed value to unsigned, producing counterintuitive results.

**Expected**: `-1 < 1u` is true. `sizeof(small_array) - sizeof(large_array)` is negative. `~(unsigned char)0xFF` is `0x00`.

**Actual**: `-1 < 1u` is FALSE because -1 is converted to `UINT_MAX` (4294967295), which is greater than 1. `sizeof(arr1) - sizeof(arr2)` wraps to a huge positive number because `size_t` is unsigned. `~(unsigned char)0xFF` is `0xFFFFFF00` because the `char` is promoted to `int` before the bitwise NOT, setting all upper bits.

**Why**: Integer promotion rules (C17 6.3.1.1) convert `char` and `short` to `int` before any arithmetic. When signed and unsigned types of the same rank meet in a binary operation, the signed operand is converted to unsigned (C17 6.3.1.8). This "usual arithmetic conversion" was designed for performance on early hardware but creates subtle bugs that even experienced C programmers fall into.

---

## 6. Macro Pitfalls (`macro_pitfalls.c`)

**What**: C preprocessor macros perform textual substitution, leading to double evaluation of arguments, precedence bugs, and broken control flow.

**Expected**: `DOUBLE(3+1)` returns 8. `MAX(x++, y++)` increments each variable once. Multi-statement macros work in if-else.

**Actual**: `#define BAD_DOUBLE(x) x * 2` expands `BAD_DOUBLE(3+1)` to `3 + 1 * 2 = 5`. `MAX(x++, y++)` increments the winning argument twice because the ternary operator evaluates it in both the comparison and the result. Multi-statement macros without `do { } while(0)` break `if-else` chains because only the first statement is controlled by the `if`. `STRINGIFY(VERSION)` produces the string `"VERSION"` instead of `"42"` unless double indirection is used.

**Why**: The preprocessor operates on tokens before compilation (C17 6.10.3). It has no knowledge of types, scopes, or expressions. Each macro parameter is pasted verbatim wherever it appears, causing multiple evaluation. The `do { } while(0)` idiom creates a single compound statement that works with semicolons and control flow. Stringification (`#`) happens before macro expansion, requiring the `XSTRINGIFY(x) STRINGIFY(x)` indirection pattern.

---

## 7. Function Pointers (`function_pointers.c`)

**What**: Functions decay to pointers automatically (like arrays). Dereferencing a function pointer is optional. Function pointers enable callbacks, dispatch tables, and pseudo-OOP patterns.

**Expected**: You must use `&` to get a function's address. You must dereference `(*fptr)(args)` to call through a pointer. `(****fptr)(args)` should crash or fail.

**Actual**: A function name automatically decays to a function pointer, so `func == &func`. You can call via `fptr(args)` or `(*fptr)(args)` — both are valid. Even `(****fptr)(args)` works because each dereference yields the function, which immediately decays back to a pointer. Arrays of function pointers create dispatch tables (jump tables), and structs with function pointer members simulate vtables for OOP-style polymorphism.

**Why**: The C standard (C17 6.3.2.1) specifies that a function designator is converted to a pointer to the function, except when used with `sizeof` or `&`. Dereferencing a function pointer yields a function designator, which immediately decays back to a pointer — creating an idempotent cycle. `qsort` (C17 7.22.5.2) exemplifies the callback pattern with its comparator function pointer parameter.

---

## 8. Memory Layout (`memory_layout.c`)

**What**: Stack, heap, global, and code segments occupy distinct address ranges. Unions allow type punning with shared memory. Bit fields pack data below byte granularity. Flexible array members allow variable-length structs.

**Expected**: Struct size equals the sum of its member sizes. Union members are stored separately. You can take the address of any struct member.

**Actual**: `struct { char; int; char; }` is 12 bytes, not 6, due to alignment padding. Union members share the exact same memory — writing to one member and reading another reinterprets the bits. Endianness can be detected by writing a `uint32_t` to a union and reading its `uint8_t` bytes. Bit field members cannot have their address taken. Flexible array members (`char data[]` at struct end) have zero size in `sizeof` but allow single-allocation variable-length data.

**Why**: The ABI requires each type to be aligned to its natural boundary (C17 6.2.8) — typically its own size — for CPU performance. Unions overlay all members at offset 0 (C17 6.7.2.1). Bit fields (C17 6.7.2.1) allow sub-byte packing but lose addressability since C's smallest addressable unit is a byte. Flexible array members (C99, C17 6.7.2.1) exist specifically to support the common pattern of a header struct followed by variable-length payload in a single allocation.

---

*Each experiment file is standalone and compilable with:*
```bash
gcc -std=c17 -Wall -o /tmp/c_exp <file>.c && /tmp/c_exp
```
