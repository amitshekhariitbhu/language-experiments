# C++ Experiment Insights

## 1. Undefined Behavior (`undefined_behavior.cpp`)

**What**: Demonstrates signed integer overflow, strict aliasing violations, uninitialized variable reads, null pointer dereference optimizations, and unspecified evaluation order.

**Expected**: Signed overflow wraps around like unsigned; uninitialized variables just hold "garbage"; null checks after dereference work fine.

**Actual**: The compiler treats undefined behavior as "cannot happen" and optimizes accordingly. Signed overflow lets the compiler assume `x + 1 > x` is always true. After dereferencing a pointer, the compiler removes subsequent null checks. An uninitialized `bool` can be neither `true` nor `false`. Function argument evaluation order varies between compilers.

**Why**: The C++ standard gives the compiler license to assume UB never occurs. Modern optimizers exploit this aggressively: they propagate the assumption through the program, eliminating branches, reordering operations, and removing code that would only execute in UB scenarios. This is not "implementation-defined" behavior — it is truly undefined, meaning the compiler can emit any code whatsoever.

---

## 2. Move Semantics (`move_semantics.cpp`)

**What**: Explores `std::move` (which doesn't move), moved-from state, const blocking moves, RVO vs explicit move, and self-move-assignment.

**Expected**: `std::move(x)` moves x; moving from a const object fails to compile; adding `std::move` to return statements is always an optimization.

**Actual**: `std::move` is just a cast to rvalue reference — the actual move happens when the rvalue is consumed by a move constructor/assignment. `std::move` on a const object silently falls back to copying with no warning. Using `std::move` in a return statement *prevents* Named Return Value Optimization (NRVO), making the code slower than not using it.

**Why**: `std::move` is defined as `static_cast<T&&>(x)`. When applied to a `const T`, it produces `const T&&`. No move constructor accepts `const T&&` (it can't steal from const), so overload resolution selects the copy constructor (`const T&`). NRVO requires the compiler to construct the return value directly in the caller's memory; `std::move` forces construction of a local followed by a move, defeating this optimization.

---

## 3. Template Metaprogramming (`template_metaprogramming.cpp`)

**What**: Compile-time factorial computation, SFINAE (Substitution Failure Is Not An Error), `constexpr if` branch elimination, compile-time string processing with `static_assert`, template template parameters, and CRTP.

**Expected**: Templates are just a way to write generic code; `if constexpr` is syntax sugar for runtime branching; SFINAE is an obscure edge case.

**Actual**: Template recursion computes values at compile time — the binary contains literal constants with zero runtime cost. `if constexpr` eliminates branches so thoroughly that code in dead branches doesn't even need to be valid for the given type. SFINAE is the foundation of type-based overload selection: when a template substitution fails, the compiler silently removes that overload instead of producing an error. CRTP gives each derived class its own set of static members through distinct base class instantiations.

**Why**: C++ templates are Turing-complete — the compiler is essentially an interpreter during template instantiation. `if constexpr` was added in C++17 to replace verbose SFINAE patterns with readable branching. CRTP works because `Counter<Dog>` and `Counter<Cat>` are completely distinct types with separate static storage, even though they share the same template definition.

---

## 4. Object Slicing (`object_slicing.cpp`)

**What**: Passing a derived class by value to a base class parameter, storing derived objects in a `vector<Base>`, slicing via assignment, and slicing in exception handlers.

**Expected**: Passing a Dog to a function taking Animal preserves the Dog's virtual methods and data. `catch(BaseException e)` catches the full derived exception.

**Actual**: When a derived object is passed by value to a base type, the derived portion is silently chopped off. The object's vtable pointer is set to the base class vtable, so virtual dispatch calls the base version. Data members unique to the derived class are lost. The same happens with `vector<Animal>` (all elements become plain Animals) and `catch` by value.

**Why**: Pass-by-value invokes the base class copy constructor, which only knows about base class members. The new object IS a base object — its vtable points to the base class. This is by design: C++ value semantics mean each object has a fixed size determined by its declared type. Polymorphism requires indirection (references or pointers) because the runtime type may be larger than the static type.

---

## 5. RAII & Scope (`raii_scope.cpp`)

**What**: Destructor ordering (reverse of construction), scope guard pattern for transactional rollback, automatic cleanup during exception stack unwinding, nested scope lifetimes, reference lifetime extension for temporaries, and static vs automatic storage duration.

**Expected**: Destructors fire in some order; exception cleanup requires explicit try/catch; temporaries are destroyed immediately.

**Actual**: Destructors fire in strict reverse order of construction — guaranteed by the standard. During exception stack unwinding, all local RAII objects are automatically destroyed in reverse order, providing exception safety without explicit cleanup code. Binding a temporary to a `const&` extends the temporary's lifetime to match the reference's scope. Static local variables survive function returns and are destroyed at program exit, after `main()` returns.

**Why**: Reverse destruction order ensures that if object B was constructed after object A (and might depend on A), B is destroyed first while A is still valid. Stack unwinding is the mechanism that makes RAII exception-safe: the runtime walks up the call stack, destroying local objects as it goes. Lifetime extension is a special rule in the standard ([class.temporary]) that applies only to direct binding — it does not propagate through function calls.

---

## 6. Virtual Dispatch (`virtual_dispatch.cpp`)

**What**: Virtual function calls in constructors/destructors, the diamond inheritance problem (with and without virtual inheritance), the Non-Virtual Interface pattern with private virtual methods, and name hiding vs overriding.

**Expected**: Calling a virtual function in a base constructor dispatches to the derived override. Private virtual functions cannot be overridden. Overriding one overload of a function leaves the other overloads accessible.

**Actual**: During base class construction, the object's vtable points to the base class — virtual calls dispatch to the base version, not the derived override. This is because the derived class hasn't been constructed yet and its members are uninitialized. Private virtual functions CAN be overridden by derived classes (access control is checked at the call site, not the definition). Overriding one overload of a virtual function HIDES all other overloads with the same name in derived class scope.

**Why**: The vtable is updated as each constructor in the hierarchy runs. During `Base::Base()`, the vtable is Base's vtable. Only after `Derived::Derived()` begins does the vtable switch to Derived's. This prevents calling derived virtual functions that might access uninitialized derived members. Name hiding occurs because the compiler performs name lookup in the most derived scope first; finding a match there stops the search, hiding base class overloads.

---

## 7. Implicit Conversions (`implicit_conversions.cpp`)

**What**: Converting constructors, the `explicit` keyword, conversion chains (char -> int -> class), the bool conversion trap, narrowing conversions, implicit conversion operators, nullptr vs NULL vs 0, and the most vexing parse.

**Expected**: `bool b = "false"` is false. Brace initialization and parenthesis initialization are equivalent. `NULL` and `nullptr` behave identically.

**Actual**: `bool b = "false"` is `true` because the string literal is a non-null pointer, which converts to `true`. A single-argument constructor without `explicit` acts as an implicit conversion operator, allowing `MyString s = 42` to compile. `char c = 1000` silently truncates. `NULL` may match `int` overloads rather than pointer overloads. `std::vector<int> v(std::vector<int>())` is parsed as a function declaration, not a variable definition.

**Why**: C++ inherited C's implicit conversion rules and added converting constructors on top. The conversion from pointer to bool exists for C compatibility. The most vexing parse follows from the C++ grammar rule that "anything that could be a declaration IS a declaration." `NULL` is typically defined as `0` or `0L` (an integer), which is why `nullptr` (type `std::nullptr_t`) was introduced in C++11 to unambiguously represent null pointers.

---

## 8. Dangling References (`dangling_references.cpp`)

**What**: Returning a reference to a local variable, reference lifetime extension rules, dangling references in range-for loops over temporaries, lambda capture dangling, string_view dangling, and `auto&` with proxy objects like `vector<bool>`.

**Expected**: The compiler prevents returning references to locals. Lifetime extension works wherever a reference is involved. `string_view` is a safe, lightweight string reference. `auto&` always works correctly with containers.

**Actual**: The compiler only warns (doesn't error) about returning references to locals — the code compiles. Lifetime extension only works for DIRECT binding of a temporary to a const reference; it does NOT propagate through function calls (so `const T& ref = identity(make_temp())` is dangling). `for(auto& x : Temp().method_returning_ref())` is dangling because the temporary is destroyed before the loop body. Lambdas capturing locals by reference become dangling when the enclosing scope ends. `string_view` does not own its data and does not extend lifetimes. `vector<bool>::operator[]` returns a proxy object, not a real reference.

**Why**: C++ references are non-owning aliases — they never extend object lifetimes except in the narrow case of direct binding to a const reference (a special rule in [class.temporary]). `string_view` was designed as a lightweight, non-owning view and deliberately does not participate in lifetime extension. `vector<bool>` is a standard-mandated specialization that packs bits, making it impossible to return a `bool&` — it returns a proxy object instead, which interacts poorly with `auto&`.

---

*Compile each file with: `g++ -std=c++17 -o /tmp/cpp_exp file.cpp && /tmp/cpp_exp`*
