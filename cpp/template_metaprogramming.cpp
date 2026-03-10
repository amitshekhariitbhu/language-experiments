// template_metaprogramming.cpp
// Demonstrates compile-time computation, SFINAE, and constexpr surprises.

#include <iostream>
#include <type_traits>
#include <array>
#include <string>

// --- 1. Compile-time factorial via template recursion ---
// The compiler literally computes this at compile time.
// The result is baked into the binary as a constant.
template <int N>
struct Factorial {
    static constexpr long long value = N * Factorial<N - 1>::value;
};

template <>
struct Factorial<0> {
    static constexpr long long value = 1;
};

void compile_time_computation() {
    std::cout << "=== Compile-Time Computation ===\n";
    // These are computed by the COMPILER, not at runtime.
    // The assembly will contain the literal values.
    std::cout << "Factorial<0>  = " << Factorial<0>::value << "\n";
    std::cout << "Factorial<5>  = " << Factorial<5>::value << "\n";
    std::cout << "Factorial<12> = " << Factorial<12>::value << "\n";
    std::cout << "Factorial<20> = " << Factorial<20>::value << "\n";
    std::cout << "All computed at compile time — zero runtime cost!\n\n";
}

// --- 2. SFINAE — Substitution Failure Is Not An Error ---
// The compiler tries to substitute template arguments. If it fails,
// it silently discards that overload instead of giving an error.

// This function exists only for types that have a .size() method
template <typename T>
auto describe(const T& t) -> decltype(t.size(), std::string{}) {
    return "Has .size() = " + std::to_string(t.size());
}

// This function exists only for arithmetic types (int, float, etc.)
template <typename T>
std::enable_if_t<std::is_arithmetic_v<T>, std::string>
describe(const T& t) {
    return "Arithmetic value = " + std::to_string(t);
}

// Fallback for everything else
std::string describe(...) {
    return "Unknown type";
}

void sfinae_demo() {
    std::cout << "=== SFINAE (Substitution Failure Is Not An Error) ===\n";
    std::string s = "hello";
    std::vector<int> v = {1, 2, 3};
    int i = 42;
    double d = 3.14;

    std::cout << "std::string:    " << describe(s) << "\n";
    std::cout << "std::vector:    " << describe(v) << "\n";
    std::cout << "int:            " << describe(i) << "\n";
    std::cout << "double:         " << describe(d) << "\n";
    std::cout << "The compiler tries each overload; failures are silently ignored.\n\n";
}

// --- 3. constexpr if — compile-time branch elimination ---
template <typename T>
std::string type_info(const T& val) {
    if constexpr (std::is_integral_v<T>) {
        // This branch is DISCARDED at compile time for non-integral types.
        // The code inside doesn't even need to be valid for other types!
        return "Integral: " + std::to_string(val) +
               (val % 2 == 0 ? " (even)" : " (odd)");
    } else if constexpr (std::is_floating_point_v<T>) {
        return "Float: " + std::to_string(val);
    } else if constexpr (std::is_same_v<T, std::string>) {
        // val.size() would be an error for int, but this branch
        // is completely eliminated when T is int.
        return "String of length " + std::to_string(val.size());
    } else {
        return "Other type";
    }
}

void constexpr_if_demo() {
    std::cout << "=== constexpr if — Compile-Time Branch Elimination ===\n";
    std::cout << type_info(42) << "\n";
    std::cout << type_info(3.14) << "\n";
    std::cout << type_info(std::string("hello")) << "\n";
    std::cout << "Dead branches are removed at compile time, not runtime.\n";
    std::cout << "Code in eliminated branches doesn't even need to compile!\n\n";
}

// --- 4. Compile-time string processing ---
constexpr int count_vowels(const char* s) {
    int count = 0;
    while (*s) {
        char c = *s | 32; // tolower
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
            ++count;
        ++s;
    }
    return count;
}

void constexpr_string_demo() {
    std::cout << "=== Compile-Time String Processing ===\n";
    constexpr int v1 = count_vowels("Hello World");
    constexpr int v2 = count_vowels("AEIOU aeiou");
    constexpr int v3 = count_vowels("rhythm");

    // These are computed at compile time!
    static_assert(v1 == 3, "Should have 3 vowels");
    static_assert(v2 == 10, "Should have 10 vowels");
    static_assert(v3 == 0, "Should have 0 vowels");

    std::cout << "Vowels in 'Hello World': " << v1 << "\n";
    std::cout << "Vowels in 'AEIOU aeiou': " << v2 << "\n";
    std::cout << "Vowels in 'rhythm':      " << v3 << "\n";
    std::cout << "All validated at compile time with static_assert!\n\n";
}

// --- 5. Template template parameters — templates of templates ---
template <template <typename, typename> class Container, typename T>
void fill_and_print(const std::string& name) {
    Container<T, std::allocator<T>> c;
    for (int i = 0; i < 5; ++i)
        c.push_back(static_cast<T>(i * 1.5));
    std::cout << name << ": ";
    for (const auto& x : c)
        std::cout << x << " ";
    std::cout << "\n";
}

void template_template_demo() {
    std::cout << "=== Template Template Parameters ===\n";
    fill_and_print<std::vector, int>("vector<int>");
    fill_and_print<std::vector, double>("vector<double>");
    std::cout << "Templates can take other templates as parameters!\n\n";
}

// --- 6. The Curiously Recurring Template Pattern (CRTP) ---
template <typename Derived>
struct Counter {
    static inline int created = 0;
    static inline int alive = 0;

    Counter() { ++created; ++alive; }
    ~Counter() { --alive; }
};

struct Dog : Counter<Dog> {};   // Each type gets its OWN static counters
struct Cat : Counter<Cat> {};   // because Counter<Dog> and Counter<Cat> are different types

void crtp_demo() {
    std::cout << "=== CRTP (Curiously Recurring Template Pattern) ===\n";
    {
        Dog d1, d2, d3;
        Cat c1, c2;
        std::cout << "Dogs created: " << Counter<Dog>::created
                  << ", alive: " << Counter<Dog>::alive << "\n";
        std::cout << "Cats created: " << Counter<Cat>::created
                  << ", alive: " << Counter<Cat>::alive << "\n";
    }
    std::cout << "After scope:\n";
    std::cout << "Dogs created: " << Counter<Dog>::created
              << ", alive: " << Counter<Dog>::alive << "\n";
    std::cout << "Cats created: " << Counter<Cat>::created
              << ", alive: " << Counter<Cat>::alive << "\n";
    std::cout << "Each derived class gets its own static counters via CRTP.\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║   Template Metaprogramming Experiments           ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    compile_time_computation();
    sfinae_demo();
    constexpr_if_demo();
    constexpr_string_demo();
    template_template_demo();
    crtp_demo();

    return 0;
}
