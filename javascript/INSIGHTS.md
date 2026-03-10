# JavaScript Experiments: Non-Obvious Behaviors

## 1. Type Coercion Madness (`type_coercion_madness.js`)

**What**: Demonstrates JavaScript's implicit type conversion with operators like `+`, `==`, and `!`, including the infamous `[] == ![]` and the "banana" trick.

**Expected**: `[] + []` should error or return an empty array. `[] == ![]` should be false since something cannot equal its own negation. `"" == false` and `"0" == false` being true should mean `"" == "0"` is also true (transitivity).

**Actual**: `[] + []` yields `""` (empty string). `[] == ![]` is `true`. `"" == false` is true, `"0" == false` is true, but `"" == "0"` is false — loose equality is not transitive. `('b' + 'a' + + 'a' + 'a').toLowerCase()` produces `"banana"` because `+ 'a'` evaluates to `NaN`.

**Why**: The `+` operator, when given objects, calls `.toString()` on them. Arrays become empty strings. The `==` operator follows the Abstract Equality Comparison Algorithm (ES spec 7.2.14), which applies different coercion rules depending on the types involved, and these rules do not guarantee transitivity. `typeof null === "object"` is a bug from the original implementation where type tags in memory were used — null had an all-zeros tag, which was the same as the object tag.

---

## 2. Event Loop Microtasks (`event_loop_microtasks.js`)

**What**: Shows the execution order of synchronous code, Promises (microtasks), `queueMicrotask`, `setTimeout` (macrotasks), and `process.nextTick` (Node.js-specific).

**Expected**: `setTimeout(..., 0)` should fire immediately after the current code, possibly before or interleaved with Promise callbacks.

**Actual**: Microtasks (Promise `.then()`, `queueMicrotask`) always execute before any macrotask (`setTimeout`), even with a 0ms delay. Nested microtasks continue draining the microtask queue before any macrotask runs, which can theoretically starve macrotasks. In Node.js, `process.nextTick` runs before Promise microtasks. The Promise constructor body executes synchronously — only `.then()` callbacks are deferred.

**Why**: The event loop processes all microtasks in the microtask queue after each task (macrotask) completes, before moving to the next macrotask. `process.nextTick` uses a separate queue that drains before the Promise microtask queue, a Node.js-specific design decision for backwards compatibility.

---

## 3. The Many Faces of `this` (`this_binding.js`)

**What**: Explores how `this` varies between method calls, detached functions, arrow functions, constructors, callbacks, and `bind()`.

**Expected**: `this` inside an object's method always refers to that object. Arrow functions behave like regular functions. Calling `bind()` twice should rebind `this`.

**Actual**: Detaching a method (`const fn = obj.method; fn()`) loses `this` — it becomes `undefined` in strict mode or the global object otherwise. Arrow functions permanently capture `this` from their enclosing lexical scope and completely ignore `bind()`, `call()`, and `apply()`. Calling `bind()` twice does not rebind — the first `bind()` is permanent.

**Why**: Regular functions determine `this` at call time based on how they are invoked (the receiver). Arrow functions have no own `this` binding — they lexically inherit it from the enclosing scope at definition time. `bind()` creates a new function with a frozen `this` via an internal `[[BoundThis]]` slot; subsequent `bind()` calls wrap the already-bound function, but the inner binding wins.

---

## 4. Prototype Chain Surprises (`prototype_chain.js`)

**What**: Demonstrates `Object.create(null)`, retroactive prototype modification, property shadowing vs mutation of inherited references, and the `constructor` property pitfall.

**Expected**: All objects have `toString`. Modifying a prototype after creating instances should not affect those instances. Modifying `child.data` (inherited array) should not affect the parent.

**Actual**: `Object.create(null)` creates a truly bare object with no prototype — no `toString`, no `hasOwnProperty`, and `instanceof Object` is false. Adding methods to a prototype after instance creation affects all existing instances immediately. Mutating an inherited reference (e.g., `child.data.push(4)`) modifies the parent's data because `child.data` resolves via the prototype chain to the same array object. The `constructor` property is just a regular inherited property that can become inaccurate when prototypes are reassigned.

**Why**: JavaScript's prototype chain is a live lookup mechanism — property access walks up the chain at access time, not at creation time. `Object.create(null)` sets the internal `[[Prototype]]` to `null`, skipping `Object.prototype` entirely. Property writes (`child.color = "blue"`) create own properties (shadowing), but reads that return reference types give you the shared original, so mutations propagate.

---

## 5. Floating Point Madness (`floating_point.js`)

**What**: Explores IEEE 754 precision issues, `MAX_SAFE_INTEGER` overflow, negative zero, `NaN` identity, and `BigInt` as an escape hatch.

**Expected**: `0.1 + 0.2` should equal `0.3`. Adding 1 and 2 to `MAX_SAFE_INTEGER` should produce different numbers. `-0` and `0` should behave identically or `-0` should not exist.

**Actual**: `0.1 + 0.2` is `0.30000000000000004`. `MAX_SAFE_INTEGER + 1 === MAX_SAFE_INTEGER + 2` is `true`. `-0 === 0` is `true` but `Object.is(-0, 0)` is `false`. The literal `9999999999999999` is parsed as `10000000000000000`. `Math.sign(-0)` returns `-0`, not `-1`. Mixing `BigInt` and `Number` in arithmetic throws a `TypeError`, but `1n == 1` is `true` with loose equality.

**Why**: All JavaScript numbers are 64-bit IEEE 754 doubles, which have 53 bits of mantissa. Numbers like 0.1 have infinite binary representations and must be rounded, accumulating error. Beyond 2^53 - 1, consecutive integers cannot be represented — only even numbers, then multiples of 4, etc. Negative zero exists in IEEE 754 for mathematical completeness (e.g., preserving sign in `1/x` as `x` approaches zero from the negative side).

---

## 6. Closures, Scope, and Hoisting (`closure_scope.js`)

**What**: Demonstrates the classic `var` loop bug, `let` fix, hoisting differences, the temporal dead zone, and stale closures.

**Expected**: A loop creating closures over `var i` should capture each iteration's value. Variables declared with `let` or `var` should behave similarly. Accessing a variable before its declaration should always throw.

**Actual**: `var` in a loop creates one shared binding — all closures see the final value (5). `let` creates a new binding per iteration, so closures correctly capture each value. `var` declarations are hoisted and initialized to `undefined` (so access before declaration returns `undefined` instead of throwing). `let`/`const` are hoisted but enter a "Temporal Dead Zone" (TDZ) where access throws `ReferenceError`. Closures capture variables by reference, not value — so a closure created before a variable is changed will see the new value when invoked.

**Why**: `var` is function-scoped and has a single binding for the entire function. `let`/`const` are block-scoped, and the ES6 spec mandates that `for` loops create a fresh binding per iteration. The TDZ exists to catch the use-before-initialization bugs that `var`'s silent `undefined` allowed. Closures hold a reference to the variable's environment record, not a snapshot of its value.

---

## 7. Equality Operators (`equality_operators.js`)

**What**: Compares `==`, `===`, and `Object.is()` across edge cases including `NaN`, `null`/`undefined`, negative zero, falsy values, and object references.

**Expected**: `NaN` should equal itself. `null == false` should be true since both are falsy. All whitespace strings should be truthy. `===` should be the definitive equality check.

**Actual**: `NaN !== NaN` is the only value in JavaScript not equal to itself (by both `==` and `===`). `null` only loosely equals `undefined` and nothing else — not `0`, not `""`, not `false`. Whitespace-only strings like `"\t"`, `"\n"`, `" "` all coerce to `0` and are `== 0`. Arrays coerce via `toString()`, so `[1,2] == "1,2"` is true. `Object.is()` differs from `===` in exactly two cases: `Object.is(NaN, NaN)` is `true`, and `Object.is(-0, +0)` is `false`.

**Why**: The `==` algorithm follows specific steps: if types match, use `===`; if one is `null`/`undefined`, return true only if the other is also `null`/`undefined`; otherwise, convert to numbers. NaN is defined by IEEE 754 as unequal to everything, including itself. `Object.is()` implements the SameValue algorithm from the spec, which treats NaN as equal to NaN and distinguishes the two zeros.

---

## 8. Generators and Iterators (`generators_iterators.js`)

**What**: Demonstrates generator pause/resume, infinite sequences, two-way communication via `yield`, `yield*` delegation, and error injection with `.throw()`.

**Expected**: Generator functions run to completion when called. `return` values appear in `for...of` loops. Generators are a niche feature with limited practical use.

**Actual**: Calling a generator function returns an iterator without executing any code — the body only runs when `.next()` is called. The `return` value of a generator is invisible to `for...of`, spread, and destructuring (they stop at `done: true` without exposing the value). `.next(value)` sends a value INTO the generator — the argument becomes what `yield` evaluates to, enabling two-way communication. `.return()` forces the generator to exit but still triggers `finally` blocks. `.throw()` injects an error at the `yield` point, which can be caught inside the generator.

**Why**: Generators implement the iterator protocol with suspend/resume semantics via coroutines. Each `yield` is a suspension point that saves the execution context. The `for...of` protocol calls `.next()` until `done: true` and uses the `value` from non-done results — the final `{value, done: true}` object's value is deliberately excluded. `yield*` delegates the iterator protocol to another iterable, forwarding `.next()`, `.return()`, and `.throw()` calls transparently, and its expression value is the delegated generator's return value.
