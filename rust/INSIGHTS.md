# Rust Experiment Insights

## 1. Ownership & Move Semantics (`ownership_move.rs`)

**What**: Demonstrates how values are moved (not copied) by default, partial struct moves, and closure capture modes.
**Expected**: Assigning a variable to another creates a copy, like in most languages. Both remain usable.
**Actual**: For heap-allocated types like `String`, assignment *moves* ownership -- the original variable becomes invalid. Integers and other `Copy` types are duplicated. Structs can be *partially* moved, leaving some fields accessible and others not.
**Why**: Rust enforces single ownership to guarantee memory safety without garbage collection. Types that implement the `Copy` trait (fixed-size, stack-only data like integers) are bitwise-copied. Types that manage heap memory (String, Vec) are moved to prevent double-free. Partial moves track ownership at the field level, allowing fine-grained control.

---

## 2. Borrow Checker & NLL (`borrow_checker.rs`)

**What**: Shows mutable vs immutable borrow rules, Non-Lexical Lifetimes, struct field borrowing, and the `split_at_mut` workaround.
**Expected**: You can't use a mutable reference anywhere near an immutable one. Borrowing two elements of a Vec mutably should work since they're different elements.
**Actual**: NLL (Non-Lexical Lifetimes) tracks where references are *last used*, not where they go out of scope. So you can create an immutable borrow, use it, and then create a mutable borrow in the same scope. Struct fields can be borrowed mutably simultaneously (the compiler sees them as disjoint). But Vec elements cannot -- the compiler can't prove `v[0]` and `v[1]` don't overlap, so you must use `split_at_mut`.
**Why**: Before Rust 2018, borrow lifetimes extended to the end of the enclosing scope (lexical). NLL analyzes the control flow graph to find the actual last use point of each reference. Struct field borrows work because fields have statically known disjoint memory. Vec indexing goes through `Index` trait methods that borrow the whole Vec, making the compiler unable to prove non-overlapping access.

---

## 3. Lifetime Elision & 'static (`lifetime_elision.rs`)

**What**: Explores when lifetime annotations are required vs automatically inferred, what `'static` really means, and struct lifetimes.
**Expected**: Lifetime annotations are always needed for references. `'static` means the value is a constant that can never change.
**Actual**: Three elision rules handle most cases automatically: (1) each input reference gets its own lifetime, (2) single input lifetime propagates to output, (3) `&self` lifetime propagates to output. `'static` doesn't mean "immutable" or "constant" -- it means the value *could* live for the entire program. Owned types like `String` satisfy `'static` because they don't borrow from anything that could expire.
**Why**: Lifetime elision was added because the same patterns appeared so frequently that annotating them was pure boilerplate. The `'static` bound is satisfied by any type that doesn't contain non-static references -- owned types trivially satisfy this because they own all their data. String literals are `&'static str` because they're embedded in the binary.

---

## 4. Pattern Matching (`pattern_matching.rs`)

**What**: Demonstrates exhaustive matching, destructuring, match guards, @ bindings, or-patterns, and `let-else`.
**Expected**: Pattern matching works like switch/case with a default fallthrough. Missing cases produce a warning.
**Actual**: Missing match arms are a *compile error*, not a warning -- the compiler enforces exhaustiveness. `@` bindings let you capture a value while simultaneously testing it against a pattern. `if let` and `while let` provide lightweight alternatives. `let-else` (Rust 1.65+) combines irrefutable pattern matching with an else branch that must diverge. Filter closures on iterators require `&&x` destructuring because `.iter()` yields `&T` and `.filter()` adds another `&`.
**Why**: Exhaustive matching is possible because Rust enums are closed types -- no subclass can be added later. The compiler checks all variants are handled. The `&&x` pattern in filter exists because `filter` takes `&Self::Item`, and when `Self::Item` is already `&T` (from `.iter()`), you get `&&T`. The `@` binding syntax fills a gap where you need both the value and a range/pattern check.

---

## 5. Trait Objects & Dispatch (`trait_objects.rs`)

**What**: Compares static dispatch (generics) vs dynamic dispatch (dyn Trait), shows object safety rules, and enum dispatch as an alternative.
**Expected**: Generics and trait objects are interchangeable. Any trait can be used as a trait object.
**Actual**: `&dyn Trait` is a *fat pointer* (16 bytes: data pointer + vtable pointer), twice the size of a regular reference (8 bytes). Traits with methods returning `Self` or with generic methods are NOT object-safe and cannot be used as `dyn Trait`. Static dispatch via generics produces specialized code at compile time (monomorphization) with zero runtime overhead. Enum dispatch avoids both heap allocation and vtable overhead but requires knowing all variants at compile time.
**Why**: Dynamic dispatch needs a vtable to look up method implementations at runtime. Methods returning `Self` can't go in a vtable because the concrete type (and thus return size) is unknown. Generic methods can't go in a vtable because each instantiation would need a separate entry. Monomorphization duplicates code for each concrete type, trading binary size for speed. The fat pointer design avoids needing a header on every object (unlike Java/C# vtable-in-object approach).

---

## 6. Shadowing & Mutation (`shadowing_mutation.rs`)

**What**: Shows the difference between `let` shadowing and `mut`, type-changing shadows, interior mutability with Cell/RefCell, and the freezing pattern.
**Expected**: Shadowing and mutation are the same thing. Immutability means you can never change a value.
**Actual**: Shadowing creates an entirely *new* variable that hides the old one -- it can even change the type. `mut` modifies the existing variable in place and cannot change its type. The "freezing pattern" uses shadowing to make a mutable variable immutable: `let x = x;`. Interior mutability (Cell, RefCell) allows mutation through shared references, bypassing compile-time borrow checking in favor of runtime checks. Constants are inlined (no address), statics have a fixed address.
**Why**: Shadowing was designed to support the common pattern of transforming data through stages while reusing the name (e.g., parsing a string to a number). It's not mutation because the old value is dropped (or remains in an outer scope). Interior mutability exists because some patterns (shared mutable state, caches, reference-counted graphs) genuinely need mutation behind shared references. Cell uses `unsafe` internally but provides a safe API for Copy types. RefCell tracks borrows at runtime and panics on violations.

---

## 7. String Types & UTF-8 (`string_types.rs`)

**What**: Explores String vs &str, UTF-8 encoding consequences, char vs byte iteration, concatenation semantics, and Cow<str>.
**Expected**: Strings can be indexed by position like arrays. A character is one byte. String concatenation is symmetric.
**Actual**: `s[0]` doesn't compile for String/&str because UTF-8 characters are 1-4 bytes, making O(1) indexing impossible. `.len()` returns *bytes*, not characters. Slicing at a non-character boundary panics at runtime. The `+` operator for strings takes ownership of the left operand but borrows the right (`s1 + &s2` consumes s1). A `char` in Rust is always 4 bytes (a Unicode scalar value), while characters in a UTF-8 string are 1-4 bytes. `Cow<str>` avoids allocation when no modification is needed.
**Why**: Rust chose correctness over convenience for strings. UTF-8 is the encoding everywhere (no UCS-2/UTF-16 confusion like Java/JavaScript). Forbidding indexing prevents the class of bugs where code assumes ASCII and breaks on international text. The `+` operator is actually `fn add(self, other: &str) -> String`, which reuses s1's buffer. `char` being 4 bytes is because it represents a Unicode scalar value (up to U+10FFFF), which needs 21 bits. Hindi/Arabic/emoji text demonstrates that "character count" itself is ambiguous (bytes vs scalar values vs grapheme clusters).

---

## 8. Option, Result & Error Handling (`option_result.rs`)

**What**: Shows Rust's null-free design, Option/Result combinators, the ? operator, niche optimization, and collecting Results.
**Expected**: Option adds memory overhead. Error handling requires verbose try/catch blocks. You always need to manually check for None/Err.
**Actual**: `Option<&T>` is the *same size* as `&T` (8 bytes, not 16) due to niche optimization -- the compiler uses the null pointer value to represent None since references can never be null. The `?` operator provides concise early-return on None/Err. Collecting an iterator of `Result<T, E>` into `Result<Vec<T>, E>` automatically short-circuits on the first error. Combinators like `map`, `and_then`, `unwrap_or_else` enable functional chaining without explicit matching.
**Why**: Niche optimization exploits "impossible" bit patterns. Since `&T` can never be null, the all-zeros pattern is available to represent None. This extends to Box, NonZeroU32, and other types with "niches." The `?` operator desugars to a match that returns early on Err/None, with automatic error type conversion via the From trait. The collect-into-Result trick works because `Result` implements `FromIterator`, which stops at the first Err. This design eliminates null pointer exceptions at compile time while maintaining zero-cost abstractions.
