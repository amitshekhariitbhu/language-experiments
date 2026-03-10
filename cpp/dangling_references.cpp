// dangling_references.cpp
// Demonstrates dangling references, reference lifetime extension,
// and related pitfalls in C++.

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <optional>

// --- 1. Returning a reference to a local variable ---
// The compiler warns about this, but it compiles!
const std::string& dangling_local() {
    std::string local = "I am a local string";
    return local;  // WARNING: returning reference to local variable!
    // local is destroyed when the function returns.
    // The returned reference refers to a destroyed object.
}

void dangling_local_demo() {
    std::cout << "=== Returning Reference to Local (DANGLING) ===\n";
    // We intentionally do NOT call dangling_local() to avoid actual UB.
    // Instead, we explain what would happen:
    std::cout << "  const string& ref = dangling_local();\n";
    std::cout << "  The local string is destroyed when the function returns.\n";
    std::cout << "  Using 'ref' afterwards is undefined behavior.\n";
    std::cout << "  It might print garbage, crash, or appear to 'work' (worst case!).\n\n";
}

// --- 2. Reference lifetime extension — when it works and when it doesn't ---
std::string make_string() {
    return "I am a temporary";
}

const std::string& identity(const std::string& s) {
    return s;  // Returns a reference to the parameter
}

void lifetime_extension_demo() {
    std::cout << "=== Reference Lifetime Extension ===\n";

    // WORKS: binding a temporary directly to a const reference extends its lifetime
    {
        const std::string& ref = make_string();
        // The temporary returned by make_string() lives as long as 'ref'
        std::cout << "  Direct binding: \"" << ref << "\" (lifetime extended!)\n";
    }

    // DOES NOT WORK: lifetime extension doesn't apply through function calls
    {
        const std::string& ref = identity(make_string());
        // The temporary was bound to identity's parameter, which is destroyed
        // when identity returns. 'ref' is now dangling!
        // We don't use 'ref' here to avoid UB; we just explain:
        std::cout << "  Through function: DANGLING! Lifetime extension doesn't propagate.\n";
    }

    // WORKS: direct binding in different forms
    {
        const std::string& r1 = std::string("direct temp");
        const std::string& r2 = "implicit conversion temp";  // const char* -> string temp
        std::cout << "  Direct temp binding: \"" << r1 << "\"\n";
        std::cout << "  Implicit conversion: \"" << r2 << "\"\n";
    }

    std::cout << "\n";
}

// --- 3. Dangling references in range-based for loops ---
std::vector<int> get_numbers() {
    return {1, 2, 3, 4, 5};
}

class Matrix {
    std::vector<std::vector<int>> data_;
public:
    Matrix() : data_{{1, 2}, {3, 4}} {}
    const std::vector<std::vector<int>>& rows() const { return data_; }
};

void range_loop_dangling() {
    std::cout << "=== Dangling References in Range-For Loops ===\n";

    // SAFE: the temporary vector lives for the duration of the for loop
    std::cout << "  Safe (temp lives for loop): ";
    for (int x : get_numbers()) {
        std::cout << x << " ";
    }
    std::cout << "\n";

    // DANGEROUS pattern (would be dangling):
    // for (auto& row : Matrix().rows()) { ... }
    // Matrix() creates a temporary, .rows() returns a reference to its internals.
    // The temporary Matrix is destroyed before the loop body runs!
    std::cout << "  DANGEROUS: for(auto& x : Temporary().member_ref()) — temp dies before loop!\n";

    // SAFE alternative:
    Matrix m;
    std::cout << "  Safe (named variable): ";
    for (const auto& row : m.rows()) {
        for (int x : row) std::cout << x << " ";
    }
    std::cout << "\n\n";
}

// --- 4. Lambda capture dangling ---
std::function<int()> make_counter_bad() {
    int count = 0;
    // Capturing by reference — but count is a local variable!
    return [&count]() { return ++count; };  // DANGLING after function returns
}

std::function<int()> make_counter_good() {
    int count = 0;
    // Capturing by value — lambda owns its own copy
    return [count]() mutable { return ++count; };  // Safe!
}

void lambda_dangling() {
    std::cout << "=== Lambda Capture Dangling ===\n";

    // Bad: captures local by reference
    // auto bad_counter = make_counter_bad();
    // bad_counter();  // UB: count no longer exists
    std::cout << "  Lambda capturing local by reference: DANGLING after function returns.\n";

    // Good: captures by value
    auto good_counter = make_counter_good();
    std::cout << "  Lambda capturing by value: " << good_counter()
              << ", " << good_counter() << ", " << good_counter() << " (safe!)\n\n";
}

// --- 5. String view dangling ---
#include <string_view>

std::string_view get_first_word(const std::string& s) {
    auto pos = s.find(' ');
    return s.substr(0, pos);  // Returns string_view into... a temporary!
    // Wait — substr returns a std::string, and string_view points into it.
    // The temporary string is destroyed, leaving a dangling string_view!
}

std::string_view get_first_word_safe(std::string_view s) {
    auto pos = s.find(' ');
    return s.substr(0, pos);  // string_view::substr returns string_view — no temp!
}

void string_view_dangling() {
    std::cout << "=== string_view Dangling ===\n";

    // This is safe because the original string outlives the view:
    std::string sentence = "hello world";
    std::string_view first = get_first_word_safe(sentence);
    std::cout << "  Safe string_view: \"" << first << "\"\n";

    // DANGEROUS: string_view pointing to a temporary
    // std::string_view bad = std::string("temp") + "orary";
    // The temporary string is destroyed; 'bad' is dangling!
    std::cout << "  DANGEROUS: string_view sv = string(\"temp\") + \"orary\";\n";
    std::cout << "  The temporary is destroyed, leaving sv dangling.\n";
    std::cout << "  string_view is a non-owning reference — it doesn't extend lifetime!\n\n";
}

// --- 6. auto& with proxy objects ---
void proxy_dangling() {
    std::cout << "=== auto& with Proxy Objects ===\n";

    std::vector<bool> bv = {true, false, true};

    // vector<bool> is special — it stores bits, not bools.
    // operator[] returns a PROXY OBJECT, not a bool&.
    auto val = bv[0];   // OK: copies the proxy/value
    // auto& ref = bv[0];  // DANGEROUS: reference to temporary proxy!
    // The proxy is a temporary that may be invalidated.

    std::cout << "  vector<bool> returns a proxy object, not a reference.\n";
    std::cout << "  auto& ref = bv[0] captures a reference to a temporary proxy!\n";
    std::cout << "  Use 'auto' (not 'auto&') with vector<bool> and similar proxies.\n";
    std::cout << "  bv[0] = " << (val ? "true" : "false") << "\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║   Dangling Reference Experiments                 ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    dangling_local_demo();
    lifetime_extension_demo();
    range_loop_dangling();
    lambda_dangling();
    string_view_dangling();
    proxy_dangling();

    std::cout << "=== Key Takeaway ===\n";
    std::cout << "References don't own objects — they just point at them.\n";
    std::cout << "Lifetime extension only works for direct binding to const ref.\n";
    std::cout << "When in doubt, copy. Copies are safe; references require vigilance.\n";

    return 0;
}
