// move_semantics.cpp
// Demonstrates surprising behaviors of move semantics in C++.

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>

// --- 1. std::move does NOT move anything ---
void move_doesnt_move() {
    std::cout << "=== std::move Does NOT Move ===\n";

    std::string s = "Hello, World!";
    // std::move is just a CAST to rvalue reference. It does not move anything.
    // The actual move happens when the rvalue reference is used to initialize
    // or assign to another object.
    std::string&& rref = std::move(s);

    // s is NOT moved from yet! rref is just an rvalue reference to s.
    std::cout << "After std::move but before using the rref:\n";
    std::cout << "  s = \"" << s << "\" (still intact!)\n";
    std::cout << "  rref = \"" << rref << "\" (same object)\n";
    std::cout << "  &s == &rref? " << (&s == &rref ? "YES" : "NO") << "\n";

    // NOW the move happens — when we construct from the rvalue reference
    std::string t = std::move(s);
    std::cout << "After constructing t from std::move(s):\n";
    std::cout << "  s = \"" << s << "\" (moved-from, likely empty)\n";
    std::cout << "  t = \"" << t << "\"\n\n";
}

// --- 2. Moved-from objects are valid but unspecified ---
void moved_from_state() {
    std::cout << "=== Moved-From State ===\n";

    std::string s = "I will be moved";
    std::string t = std::move(s);

    // s is now in a "valid but unspecified" state.
    // You CAN still use it — but only operations without preconditions.
    // Safe: assign to it, call .size(), .empty(), clear(), destructor
    // Unsafe: assume any particular value

    std::cout << "Moved-from string:\n";
    std::cout << "  s.size() = " << s.size() << " (valid call, value unspecified)\n";
    std::cout << "  s.empty() = " << (s.empty() ? "true" : "false") << "\n";

    // You can reuse it after assigning a new value
    s = "Reborn!";
    std::cout << "  After reassignment: s = \"" << s << "\"\n\n";

    // Vector moved-from state
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::vector<int> w = std::move(v);
    std::cout << "Moved-from vector:\n";
    std::cout << "  v.size() = " << v.size() << " (typically 0, but not guaranteed)\n";
    std::cout << "  w.size() = " << w.size() << "\n\n";
}

// --- 3. const prevents moving (silently copies instead!) ---
void const_blocks_move() {
    std::cout << "=== const Silently Prevents Moving ===\n";

    const std::string cs = "I am const";
    // std::move on a const object produces const T&&.
    // There is no constructor taking const T&& (it can't steal resources from const).
    // So the COPY constructor (which takes const T&) is selected instead.
    // No warning, no error — just a silent copy!
    std::string t = std::move(cs);

    std::cout << "std::move on const string:\n";
    std::cout << "  cs = \"" << cs << "\" (unchanged — was copied, not moved!)\n";
    std::cout << "  t  = \"" << t << "\"\n";
    std::cout << "  std::move(const_object) silently falls back to copying.\n\n";
}

// --- 4. Return value optimization vs std::move ---
struct Tracer {
    std::string name;
    Tracer(std::string n) : name(std::move(n)) {
        std::cout << "  [Tracer '" << name << "' constructed]\n";
    }
    Tracer(const Tracer& o) : name(o.name) {
        std::cout << "  [Tracer '" << name << "' COPIED]\n";
    }
    Tracer(Tracer&& o) noexcept : name(std::move(o.name)) {
        std::cout << "  [Tracer '" << name << "' MOVED]\n";
    }
    ~Tracer() {
        std::cout << "  [Tracer '" << name << "' destroyed]\n";
    }
};

Tracer good_return() {
    Tracer t("good");
    return t;  // NRVO (Named Return Value Optimization) elides the copy/move entirely
}

Tracer bad_return() {
    Tracer t("bad");
    return std::move(t);  // PREVENTS NRVO! Forces a move instead of elision.
}

void rvo_vs_move() {
    std::cout << "=== Return Value: Don't std::move! ===\n";
    std::cout << "Without std::move (NRVO can apply):\n";
    {
        Tracer t = good_return();
        std::cout << "  Got: " << t.name << "\n";
    }
    std::cout << "\nWith std::move (NRVO defeated):\n";
    {
        Tracer t = bad_return();
        std::cout << "  Got: " << t.name << "\n";
    }
    std::cout << "\n";
}

// --- 5. Self-move-assignment ---
void self_move() {
    std::cout << "=== Self-Move-Assignment ===\n";

    std::string s = "test";
    s = std::move(s);  // Self-move-assignment!
    // The standard says this leaves s in a valid but unspecified state.
    // Many implementations handle it gracefully, but it's not guaranteed.
    std::cout << "After s = std::move(s): s = \"" << s << "\"\n";
    std::cout << "Self-move is valid but unspecified — avoid it.\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   Move Semantics Experiments             ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    move_doesnt_move();
    moved_from_state();
    const_blocks_move();
    rvo_vs_move();
    self_move();

    return 0;
}
