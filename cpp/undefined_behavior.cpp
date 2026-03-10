// undefined_behavior.cpp
// Demonstrates several forms of undefined behavior in C++.
// NOTE: UB means the compiler can do ANYTHING — results vary by compiler/optimization level.

#include <iostream>
#include <limits>
#include <cstring>

// --- 1. Signed integer overflow is UB (unsigned wraps, signed does not) ---
void signed_overflow_demo() {
    std::cout << "=== Signed Integer Overflow (UB) ===\n";

    int max_val = std::numeric_limits<int>::max();
    std::cout << "INT_MAX = " << max_val << "\n";

    // Unsigned overflow is well-defined (wraps around modulo 2^N)
    unsigned int umax = std::numeric_limits<unsigned int>::max();
    unsigned int uwrap = umax + 1;
    std::cout << "UINT_MAX + 1 = " << uwrap << " (well-defined wrap to 0)\n";

    // Signed overflow is UB — the compiler may assume it never happens.
    // With optimizations, the compiler can eliminate branches based on this assumption.
    // We demonstrate the *assumption* rather than triggering the actual UB.
    std::cout << "Signed overflow is UB: compiler assumes (x + 1 > x) is always true for signed int.\n";
    std::cout << "This lets compilers optimize away overflow checks entirely!\n\n";
}

// --- 2. Strict aliasing violation ---
void strict_aliasing_demo() {
    std::cout << "=== Strict Aliasing Violation ===\n";

    float f = 3.14f;
    // The CORRECT way to type-pun in C++:
    unsigned int correct_bits;
    std::memcpy(&correct_bits, &f, sizeof(f));
    std::cout << "float 3.14 bits via memcpy (correct): 0x" << std::hex << correct_bits << std::dec << "\n";

    // The WRONG way (strict aliasing violation — UB):
    // unsigned int bad_bits = *reinterpret_cast<unsigned int*>(&f);
    // With -O2 -fstrict-aliasing, the compiler may assume the int* and float*
    // don't alias, and reorder or eliminate reads/writes.
    std::cout << "reinterpret_cast type punning violates strict aliasing — UB!\n";
    std::cout << "Use std::memcpy or std::bit_cast (C++20) instead.\n\n";
}

// --- 3. Reading uninitialized variables ---
void uninitialized_demo() {
    std::cout << "=== Uninitialized Variable Read (UB) ===\n";

    // Reading an uninitialized variable is UB. The value is not just
    // "garbage" — the compiler can assume it never happens and optimize
    // based on that assumption.
    int x; // uninitialized
    // Using volatile to prevent the compiler from optimizing away the read entirely
    volatile int sink;

    // We won't actually read x because even demonstrating UB can cause
    // unpredictable behavior. Instead, we show the concept:
    std::cout << "An uninitialized bool can be neither true nor false!\n";
    std::cout << "  bool b; if(b) ... else if(!b) ... — BOTH branches can be skipped.\n";
    std::cout << "  The compiler treats UB as 'cannot happen' and optimizes accordingly.\n\n";
}

// --- 4. Null pointer dereference and the optimizer ---
void null_deref_optimization() {
    std::cout << "=== Null Pointer Dereference & Optimizer ===\n";

    // If you dereference a pointer, the compiler assumes it's not null.
    // This means a later null check can be optimized away!
    //
    // void foo(int* p) {
    //     int x = *p;        // compiler now assumes p != nullptr
    //     if (p == nullptr)  // compiler removes this: "p can't be null, we just used it"
    //         handle_error();
    // }
    std::cout << "After dereferencing a pointer, the compiler assumes it's non-null.\n";
    std::cout << "Subsequent null checks may be silently removed by the optimizer.\n";
    std::cout << "This has caused real-world security vulnerabilities (Linux kernel CVE-2009-1897).\n\n";
}

// --- 5. Sequence point / evaluation order ---
void evaluation_order_demo() {
    std::cout << "=== Evaluation Order Surprises ===\n";

    // In C++17, some evaluation orders were finally specified.
    // But function argument evaluation order is STILL unspecified.
    int i = 0;
    auto show = [](int a, int b, int c) {
        std::cout << "  f(" << a << ", " << b << ", " << c << ")\n";
    };

    // The order of evaluation of arguments is unspecified.
    // Different compilers (or even different optimization levels) may evaluate
    // these in different orders.
    i = 0;
    show(++i, ++i, ++i);
    std::cout << "  Function argument evaluation order is unspecified (not UB in C++17, but unspecified).\n";
    std::cout << "  Different compilers give different results for f(++i, ++i, ++i).\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════╗\n";
    std::cout << "║   C++ Undefined Behavior Experiments         ║\n";
    std::cout << "╚══════════════════════════════════════════════╝\n\n";

    signed_overflow_demo();
    strict_aliasing_demo();
    uninitialized_demo();
    null_deref_optimization();
    evaluation_order_demo();

    std::cout << "=== Key Takeaway ===\n";
    std::cout << "UB is not 'implementation-defined' — it means the compiler can do ANYTHING.\n";
    std::cout << "Modern compilers actively exploit UB assumptions for optimization.\n";
    std::cout << "Use -fsanitize=undefined to catch UB at runtime.\n";

    return 0;
}
