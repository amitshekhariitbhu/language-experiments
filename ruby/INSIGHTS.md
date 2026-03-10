# Ruby Experiment Insights

## 1. Everything Is an Object (`everything_is_object.rb`)

**What**: Ruby has no primitive types. Integers, floats, booleans, and nil are all full objects with methods, object IDs, and class hierarchies.

**Expected**: Numbers like `5` are primitive values (as in Java, C, JavaScript) and can't receive method calls directly.

**Actual**: `5.even?` returns `true`, `5.times { }` iterates, `nil.to_a` returns `[]`, and even `Class.class` returns `Class` (a circular reference). Small integers have predictable object IDs following the formula `2n + 1`.

**Why**: Ruby's VM represents small integers as "tagged pointers" (called Fixnums historically) for performance, but the language semantics treat them as full objects. The MRI implementation uses a bit-tagging scheme where the lowest bit of the object ID indicates whether a value is an immediate (integer, symbol, true, false, nil) rather than a heap-allocated object. This is why `true.object_id` is always 2, `false.object_id` is 0, and `nil.object_id` is 8.

---

## 2. Blocks, Procs, and Lambdas (`blocks_procs_lambdas.rb`)

**What**: Ruby has three callable constructs — blocks, Procs, and Lambdas — that look similar but differ critically in how they handle `return` and argument count (arity).

**Expected**: Since both `Proc.new` and `lambda` create `Proc` objects, they should behave the same way. A `return` inside either one should just return from the callable.

**Actual**: A `return` inside a `Proc.new` exits the *enclosing method* (not just the Proc), while a `return` inside a lambda exits only the lambda. Procs also silently ignore extra arguments and fill missing ones with `nil`, while lambdas raise `ArgumentError` for wrong arity.

**Why**: Procs are designed to behave like inline code blocks — they're "code fragments" that run in the context of their enclosing method. Lambdas are designed to behave like methods — they have strict argument checking and localized return. Both are instances of `Proc`, but `lambda?` returns `true` only for lambdas, and the VM checks this flag when executing `return`.

---

## 3. method_missing Magic (`method_missing_magic.rb`)

**What**: When Ruby can't find a method on an object, it calls `method_missing` instead of immediately raising `NoMethodError`. This enables "ghost methods" — methods that don't exist in the class but respond dynamically.

**Expected**: Calling an undefined method should raise an error. There's no way to intercept undefined method calls.

**Actual**: `method_missing` intercepts the call with the method name and arguments, letting you handle it dynamically. You can build fluent DSLs, proxy objects, and ActiveRecord-style `find_by_*` finders. The companion `respond_to_missing?` makes reflection work correctly.

**Why**: Ruby's method dispatch walks the ancestor chain looking for a method. If it reaches `BasicObject` without finding one, it restarts the walk looking for `method_missing` instead. This two-pass lookup is built into the VM. The pattern is so common that Rails uses it extensively (e.g., `find_by_name`, `column_name` accessors). The performance optimization of using `define_method` to cache ghost methods on first call is a well-known Ruby idiom.

---

## 4. Open Classes (`open_classes.rb`)

**What**: Ruby classes are never "sealed." You can reopen any class — including core classes like `Integer` and `String` — and add, remove, or replace methods at any time. This is called "monkey patching."

**Expected**: Built-in classes like `Integer` and `String` are locked and can't be modified. Adding `factorial` to all integers shouldn't be possible.

**Actual**: `class Integer; def factorial; ... end; end` works perfectly, and now `5.factorial` returns `120`. You can even redefine `Integer#+` so that `2 + 2` returns `5`. Refinements (Ruby 2.0+) provide scoped monkey patching that limits changes to the current file/module scope.

**Why**: Ruby classes are just objects (instances of `Class`) with a method table that's a mutable hash. "Reopening" a class simply means referencing an existing constant and adding entries to its method table. There's no compilation step that freezes the class. Refinements work by maintaining a separate method table that's consulted first, scoped to the lexical context where `using` was called.

---

## 5. Symbols vs Strings (`symbol_vs_string.rb`)

**What**: Symbols (`:hello`) and strings (`"hello"`) represent text differently. Symbols are immutable, unique, and interned; strings are mutable and allocated fresh each time.

**Expected**: `:hello` and `"hello"` are interchangeable. Creating the same string twice reuses the same memory (like Java's string interning).

**Actual**: `:hello.object_id` is always the same value (same object), while two `"hello"` string literals have different `object_id`s (different heap allocations). Frozen strings (`"hello".freeze`) bridge the gap — Ruby interns frozen string literals so they share the same object. Symbol hash key lookups are faster because comparison is by pointer identity, not character-by-character.

**Why**: Ruby maintains a global symbol table (accessible via `Symbol.all_symbols`). When you write `:hello`, Ruby checks this table and returns the existing entry or creates a new one. Strings allocate a new heap object each time. Since Ruby 2.2, dynamically created symbols (via `to_sym`) can be garbage collected, fixing a historical denial-of-service vector where user input converted to symbols would leak memory indefinitely.

---

## 6. Truthiness (`truthiness.rb`)

**What**: Ruby's truthiness rules are simpler than most languages: only `false` and `nil` are falsy. Everything else — including `0`, `""`, `[]`, `{}` — is truthy.

**Expected**: `0` is falsy (as in C, Python, JavaScript), empty strings are falsy (as in Python, JavaScript), and empty collections are falsy (as in Python).

**Actual**: `if 0` enters the truthy branch. `if ""` enters the truthy branch. `if []` enters the truthy branch. Only `if false` and `if nil` skip it. The `||=` operator is affected: `x = false; x ||= "default"` overwrites `false` with `"default"`, while `x = 0; x ||= "default"` keeps `0`.

**Why**: Ruby's design philosophy prioritizes simplicity and consistency. Matz (Ruby's creator) decided that the concept of "emptiness" is domain-specific — an empty array might be a valid, meaningful value. By making only `false` and `nil` falsy, the rules are trivially simple to remember. The `&&` and `||` operators return the last evaluated value (not a boolean), enabling patterns like `name = params[:name] || "Anonymous"`.

---

## 7. Mixin Modules (`mixin_modules.rb`)

**What**: Ruby uses `include`, `extend`, and `prepend` to mix module methods into classes, but each keyword inserts the module at a different point in the method resolution order (MRO).

**Expected**: `include` and `prepend` both add instance methods, so they should behave the same. A module mixed in should not be able to override a class's own methods.

**Actual**: `include` inserts the module AFTER the class in the ancestor chain (so the class's own methods take priority). `prepend` inserts the module BEFORE the class (so the module's methods override the class's). `extend` adds methods to the class itself (as class methods), not to instances. This means `prepend` can wrap/intercept method calls using `super`, which is how many Ruby gems implement transparent logging, caching, and AOP patterns.

**Why**: Ruby's method dispatch walks the `ancestors` array linearly. `include` appends the module after the class, `prepend` inserts it before the class. When multiple modules are included, later includes are checked first (they appear earlier in the ancestor chain). Ruby also linearizes the MRO to solve the diamond problem — if two included modules both include a shared module, it appears only once in the ancestors chain.

---

## 8. Enumerable & Lazy Enumerators (`enumerable_lazy.rb`)

**What**: Ruby's `Enumerable` module provides a rich set of collection methods, and `lazy` enumerators allow you to chain operations on infinite sequences without computing everything upfront.

**Expected**: You can't work with infinite sequences — `(1..).map { |n| n**2 }` would hang forever trying to map all integers.

**Actual**: `(1..).lazy.map { |n| n**2 }.first(10)` computes only 10 values, then stops. Lazy enumerators use pull-based evaluation: each element is fully processed through the entire chain before the next element is started. In benchmarks, lazy evaluation performs significantly fewer operations than eager evaluation when you only need a subset of results. Custom infinite enumerators (like Fibonacci) can be created with `Enumerator.new` and used with `.next` for coroutine-like external iteration.

**Why**: Eager Enumerable methods (like `map`, `select`) create intermediate arrays at each step. `Lazy` wraps the enumerator in a `Enumerator::Lazy` that defers computation. Each method in the chain returns another lazy enumerator rather than an array. Values are pulled through the pipeline one at a time only when a terminal operation (like `first`, `to_a`, `force`) demands them. This is Ruby's implementation of the same concept as Java Streams, Python generators, or Haskell's default lazy evaluation.
