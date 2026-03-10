// implicit_conversions.cpp
// Demonstrates C++'s implicit conversion pitfalls:
// converting constructors, explicit keyword, narrowing conversions, and more.

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

// --- 1. Converting constructors — silent type coercion ---
class Meters {
    double value_;
public:
    // This constructor IMPLICITLY converts double to Meters!
    Meters(double v) : value_(v) {}
    double value() const { return value_; }
};

class SafeMeters {
    double value_;
public:
    // 'explicit' prevents implicit conversion
    explicit SafeMeters(double v) : value_(v) {}
    double value() const { return value_; }
};

void print_distance(Meters m) {
    std::cout << "  Distance: " << m.value() << " meters\n";
}

void print_safe_distance(SafeMeters m) {
    std::cout << "  Distance: " << m.value() << " meters\n";
}

void converting_constructors() {
    std::cout << "=== Converting Constructors ===\n";

    // Implicit conversion — compiles silently!
    print_distance(42.5);  // 42.5 implicitly converts to Meters(42.5)
    print_distance(0);     // 0 implicitly converts to Meters(0.0)

    Meters m = 100;  // Implicit conversion in initialization
    std::cout << "  Meters m = 100 works: " << m.value() << "\n";

    // With explicit, these would fail:
    // print_safe_distance(42.5);  // ERROR
    // SafeMeters sm = 100;        // ERROR
    print_safe_distance(SafeMeters(42.5));  // Must be explicit
    std::cout << "  'explicit' forces callers to be intentional.\n\n";
}

// --- 2. Conversion chains and surprising overload resolution ---
class MyString {
public:
    MyString(const char* s) {  // Implicit from const char*
        std::cout << "  MyString from const char*: \"" << s << "\"\n";
    }
    MyString(int n) {  // Implicit from int — probably a bug!
        std::cout << "  MyString from int: " << n << " (accidental?)\n";
    }
};

void conversion_chains() {
    std::cout << "=== Surprising Conversion Chains ===\n";

    MyString s1 = "hello";   // OK: const char* -> MyString
    MyString s2 = 42;        // Compiles! int -> MyString (probably unintended)
    MyString s3 = 'A';       // char -> int -> MyString (double conversion!)

    std::cout << "  Character 'A' became int " << (int)'A' << " then MyString.\n\n";
}

// --- 3. Bool conversion trap ---
void bool_trap() {
    std::cout << "=== The Bool Conversion Trap ===\n";

    // Pointers implicitly convert to bool
    const char* str = "hello";
    if (str) std::cout << "  Pointer is truthy (non-null)\n";

    // This compiles but does something unexpected:
    std::string s = "hello";
    // s == "world" compares strings, but:
    // s > 0 would compare after bool conversion!

    // The most devious example:
    std::cout << "  std::cout << true gives: " << true << "\n";  // prints 1, not "true"

    // String literals are implicitly convertible to bool!
    bool b = "false";  // The STRING "false" converts to... true!
    std::cout << "  bool b = \"false\" is: " << (b ? "true" : "false")
              << " (string literal is non-null pointer!)\n\n";
}

// --- 4. Narrowing conversions ---
void narrowing_conversions() {
    std::cout << "=== Narrowing Conversions ===\n";

    // Old-style initialization allows narrowing — silent data loss
    int big = 1000;
    char c = big;  // Narrowing! 1000 doesn't fit in char. Silent truncation.
    std::cout << "  int 1000 -> char: " << (int)c << " (silently truncated!)\n";

    double pi = 3.14159;
    int pi_int = pi;  // Narrowing! Fractional part silently lost.
    std::cout << "  double 3.14159 -> int: " << pi_int << " (fraction lost)\n";

    // Brace initialization {} catches narrowing!
    // int bad{3.14};  // ERROR: narrowing conversion
    // char bad2{1000}; // ERROR: narrowing conversion
    int ok{42};      // OK: 42 fits in int
    std::cout << "  Brace initialization {} catches narrowing at compile time.\n";

    // But this is fine because the compiler knows 42 fits in char:
    char ok_char{42};  // OK: compiler knows 42 fits
    std::cout << "  char c{42} is OK: compiler verifies it fits.\n\n";
}

// --- 5. Implicit conversion operators ---
class SmartBool {
    bool value_;
public:
    SmartBool(bool v) : value_(v) {}

    // Implicit conversion to bool — enables surprising arithmetic
    operator bool() const { return value_; }
};

class SafeBool {
    bool value_;
public:
    SafeBool(bool v) : value_(v) {}

    // Explicit conversion — only allows direct bool contexts (if, while, &&, ||, !)
    explicit operator bool() const { return value_; }
};

void conversion_operators() {
    std::cout << "=== Conversion Operator Pitfalls ===\n";

    SmartBool a(true), b(false);
    // This compiles because bool converts to int for arithmetic!
    int sum = a + b;  // true(1) + false(0) = 1
    std::cout << "  SmartBool(true) + SmartBool(false) = " << sum << " (bool -> int -> add)\n";

    // With explicit operator bool, arithmetic is prevented:
    SafeBool sa(true), sb(false);
    // int bad = sa + sb;  // ERROR with explicit
    if (sa) std::cout << "  SafeBool works in if-statements (contextual conversion)\n";
    std::cout << "  But SafeBool(true) + SafeBool(false) won't compile with 'explicit'.\n\n";
}

// --- 6. The nullptr surprise ---
void nullptr_surprise() {
    std::cout << "=== nullptr vs 0 vs NULL ===\n";

    // Three ways to express "no pointer" — but they're different types!
    auto a = 0;        // int
    auto b = NULL;     // implementation-defined: might be int, might be long
    auto c = nullptr;  // std::nullptr_t

    std::cout << "  Type of 0:       int\n";
    std::cout << "  Type of NULL:    implementation-defined (often long)\n";
    std::cout << "  Type of nullptr: std::nullptr_t\n";

    // This matters for overload resolution:
    auto overloaded = [](int) { std::cout << "  -> called with int\n"; };
    auto overloaded_ptr = [](int*) { std::cout << "  -> called with int*\n"; };

    // overloaded(NULL);     // Might call int version! Ambiguous on some compilers.
    // overloaded(nullptr);  // Would call int* version if available.
    std::cout << "  NULL can match int overloads; nullptr always matches pointer overloads.\n\n";
}

// --- 7. Most vexing parse ---
void most_vexing_parse() {
    std::cout << "=== Most Vexing Parse ===\n";

    // This looks like it creates a vector<int> initialized from another vector...
    // std::vector<int> v(std::vector<int>());
    // But the compiler parses it as a FUNCTION DECLARATION:
    //   v is a function that takes a function-pointer (returning vector<int>)
    //   and returns a vector<int>.

    // Brace initialization fixes this:
    std::vector<int> v{std::vector<int>()};  // Actually creates a vector
    std::cout << "  std::vector<int> v(std::vector<int>()) is a FUNCTION DECLARATION!\n";
    std::cout << "  Use braces {} to avoid: std::vector<int> v{std::vector<int>()}\n";
    std::cout << "  v.size() = " << v.size() << " (actually a vector now)\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║   Implicit Conversion Experiments                ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    converting_constructors();
    conversion_chains();
    bool_trap();
    narrowing_conversions();
    conversion_operators();
    nullptr_surprise();
    most_vexing_parse();

    return 0;
}
