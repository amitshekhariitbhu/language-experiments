# Go Experiment Insights

## 1. Goroutine Closure Variable Capture (`goroutine_closure.go`)

**What**: Closures created in a loop capture the variable itself, not its value at the time of creation. When stored and executed later, all closures see the same (final) value.

**Expected**: Each closure prints a different value corresponding to the iteration when it was created.

**Actual**: All closures print the final value of the shared variable (4), because they all reference the same memory location. In Go 1.22+, loop variables (`for i := ...`) are per-iteration, fixing the classic trap for direct loop variable capture, but manually shared variables still exhibit this behavior.

**Why**: Go closures capture variables by reference, not by value. The closure holds a pointer to the outer variable. When the goroutines finally execute, the loop has completed and the variable holds its final value. The fix is to create a new variable per iteration (`val := x`) or pass the value as a function parameter.

---

## 2. Slice Header vs Underlying Array (`slice_tricks.go`)

**What**: Slices are three-word structs (pointer, length, capacity) that reference an underlying array. Multiple slices can alias the same array, and `append` may silently overwrite elements in the original.

**Expected**: Appending to a sub-slice creates independent data.

**Actual**: If the sub-slice has remaining capacity in the underlying array, `append` overwrites the original slice's data without any warning. `original[2]` gets silently replaced by 999.

**Why**: A slice header is `{ptr, len, cap}`. Sub-slicing with `a[0:2]` creates a new header pointing into the same array with `cap` equal to the remaining capacity. `append` only allocates a new array when `len == cap`. The three-index slice expression `a[0:2:2]` limits capacity, forcing `append` to allocate. Additionally, nil slices and empty slices behave identically for `len`, `cap`, `range`, and `append`, but differ for nil checks and JSON marshaling (nil becomes `null`, empty becomes `[]`).

---

## 3. The nil Interface Gotcha (`interface_nil.go`)

**What**: An interface value holding a nil pointer is NOT equal to nil. This is Go's most infamous gotcha.

**Expected**: If a function returns a nil `*Dog` as an `Animal` interface, checking `result == nil` should be true.

**Actual**: `result == nil` is `false`. The interface has type information (`*main.Dog`) even though the underlying pointer is nil. An interface is only nil when both its type and value fields are nil.

**Why**: Interfaces in Go are implemented as a two-word struct: `{type pointer, value pointer}`. Assigning a typed nil (like `var d *Dog`) to an interface sets the type field to `*Dog` and the value field to nil. Since the type field is non-nil, the interface itself is not nil. This is why returning `nil` directly (not a typed nil) from functions returning interfaces is critical.

---

## 4. Defer Gotchas (`defer_gotchas.go`)

**What**: `defer` has three surprising behaviors: argument evaluation timing, closure variable capture, and the ability to modify named return values.

**Expected**: Deferred calls see the values of variables at the time they execute (function exit).

**Actual**: Deferred function *arguments* are evaluated immediately at the `defer` statement (so `defer fmt.Println(x)` captures `x`'s value at defer time). However, deferred *closures* capture the variable by reference and see its final value. Additionally, deferred functions can modify named return values after the `return` statement executes.

**Why**: `defer f(expr)` evaluates `expr` immediately and saves the result. But `defer func() { use(x) }()` captures `x` by reference (it is a closure). Named returns work because `return 1` sets the named variable, then deferred functions run with access to that variable, and the final value of the named variable is what the caller receives. Defers execute in LIFO (stack) order.

---

## 5. Map Iteration Surprises (`map_iteration.go`)

**What**: Map iteration order is deliberately randomized by the Go runtime, and concurrent map access causes a fatal (unrecoverable) panic.

**Expected**: Maps iterate in insertion order (like Python dicts) and concurrent access causes data corruption.

**Actual**: Each iteration over the same map produces a different key order. The Go runtime intentionally randomizes the starting position to prevent code from depending on order. Concurrent map reads and writes trigger a `fatal error: concurrent map writes` that crashes the entire program and cannot be caught with `recover()`.

**Why**: The randomization was added in Go 1.0 specifically to prevent developers from relying on iteration order, which is an implementation detail. The concurrent access panic was added in Go 1.6 as a runtime check — it is a `throw()` not a `panic()`, so `recover()` cannot intercept it. Solutions include `sync.Map` or protecting access with `sync.RWMutex`. Deleting keys during iteration is explicitly safe per the language spec.

---

## 6. Error Handling Subtleties (`error_handling.go`)

**What**: Go's error wrapping with `%w` creates chains that `errors.Is` and `errors.As` can traverse, but `==` and `%v` break the chain.

**Expected**: Comparing a wrapped error with `==` to the original sentinel error works.

**Actual**: `wrappedErr == ErrNotFound` is `false` because wrapping creates a new error value. Only `errors.Is(wrappedErr, ErrNotFound)` traverses the chain. Using `%v` instead of `%w` in `fmt.Errorf` converts the error to a string, permanently breaking the chain. Two `errors.New` calls with identical text produce different error values (`e1 != e2`).

**Why**: `fmt.Errorf("...: %w", err)` returns a struct that implements `Unwrap() error`, allowing `errors.Is` and `errors.As` to walk the chain. `%v` calls `.Error()` and embeds the string, producing no `Unwrap` method. Sentinel errors created with `errors.New` are distinct pointer values — identity is based on pointer equality, not string content.

---

## 7. Channel Patterns and Pitfalls (`channel_patterns.go`)

**What**: Channels have several non-obvious behaviors: nil channels block forever, closed channels return zero values, and directional types provide compile-time safety.

**Expected**: Reading from a closed channel panics. Nil channels are useless.

**Actual**: Reading from a closed channel returns the zero value with `ok=false` (no panic). Nil channels block forever on both send and receive, which is actually *useful* in `select` statements to dynamically disable cases. Buffered channels only block when the buffer is full (send) or empty (receive). Directional channel types (`chan<-`, `<-chan`) enforce send-only or receive-only semantics at compile time.

**Why**: Closed channels returning zero values enables the "range over channel" pattern and allows multiple goroutines to detect closure. Nil channel blocking is by design — in a `select` with multiple cases, setting a channel to nil effectively removes that case from consideration without restructuring code. The directionality system is a type-level feature with zero runtime cost.

---

## 8. Struct Comparison and Empty Struct (`struct_comparison.go`)

**What**: Struct comparability depends on field types, empty structs have zero size with surprising address behavior, and field ordering affects struct size due to padding.

**Expected**: All structs can be compared with `==`. Reordering fields has no effect on size.

**Actual**: Structs containing slices, maps, or functions cannot be compared with `==` (compile error). Empty structs (`struct{}{}`) take 0 bytes and may share the same address. Most surprisingly, reordering fields changes struct size: `{bool, int64, bool}` is 24 bytes but `{int64, bool, bool}` is only 16 bytes due to alignment padding rules.

**Why**: Go only allows `==` on "comparable" types (no slices, maps, or functions). Empty structs are a special case — the runtime may return the same address (`runtime.zerobase`) for all zero-size allocations. Struct padding follows platform alignment rules: each field must start at an address that is a multiple of its alignment. A `bool` followed by `int64` wastes 7 bytes of padding. Grouping fields by decreasing size minimizes padding waste.

---
