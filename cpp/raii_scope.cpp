// raii_scope.cpp
// Demonstrates RAII (Resource Acquisition Is Initialization),
// destructor ordering, and scope-based cleanup.

#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <vector>
#include <functional>

// --- A resource tracker that logs construction/destruction ---
class Resource {
    std::string name_;
public:
    Resource(std::string name) : name_(std::move(name)) {
        std::cout << "  [+] Resource '" << name_ << "' acquired\n";
    }
    ~Resource() {
        std::cout << "  [-] Resource '" << name_ << "' released\n";
    }
    // Prevent copying to make ownership clear
    Resource(const Resource&) = delete;
    Resource& operator=(const Resource&) = delete;
    Resource(Resource&&) = default;
    Resource& operator=(Resource&&) = default;

    const std::string& name() const { return name_; }
};

// --- 1. Destruction order is reverse of construction ---
void destruction_order() {
    std::cout << "=== Destruction Order (Reverse of Construction) ===\n";
    std::cout << "Constructing A, B, C in order:\n";
    Resource a("A");
    Resource b("B");
    Resource c("C");
    std::cout << "About to leave scope — watch the destruction order:\n";
    // Destructors fire in reverse order: C, B, A
    // This is GUARANTEED by the standard and is crucial for correctness.
    // If B depends on A, B is destroyed first, so A is still valid.
}

// --- 2. RAII scope guard ---
class ScopeGuard {
    std::function<void()> cleanup_;
    bool active_ = true;
public:
    ScopeGuard(std::function<void()> cleanup) : cleanup_(std::move(cleanup)) {}
    ~ScopeGuard() {
        if (active_) cleanup_();
    }
    void dismiss() { active_ = false; }  // Cancel the cleanup
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

void scope_guard_demo() {
    std::cout << "\n=== RAII Scope Guard ===\n";

    // Simulate a multi-step operation that needs rollback on failure
    bool step1_done = false;
    bool step2_done = false;

    std::cout << "Starting multi-step operation:\n";

    // Step 1
    step1_done = true;
    std::cout << "  Step 1 completed\n";
    ScopeGuard guard1([&]() {
        std::cout << "  [ROLLBACK] Undoing step 1\n";
        step1_done = false;
    });

    // Step 2
    step2_done = true;
    std::cout << "  Step 2 completed\n";
    ScopeGuard guard2([&]() {
        std::cout << "  [ROLLBACK] Undoing step 2\n";
        step2_done = false;
    });

    // If everything succeeds, dismiss the guards
    std::cout << "  All steps succeeded — dismissing guards\n";
    guard2.dismiss();
    guard1.dismiss();
    std::cout << "  (No rollback will occur)\n";
}

// --- 3. Exception safety via RAII ---
void exception_safety() {
    std::cout << "\n=== RAII Exception Safety ===\n";
    try {
        Resource r1("DB Connection");
        Resource r2("File Handle");
        Resource r3("Network Socket");

        std::cout << "  About to throw an exception...\n";
        throw std::runtime_error("Something went wrong!");

        // r3, r2, r1 are never explicitly cleaned up, but their
        // destructors run automatically during stack unwinding.
    } catch (const std::exception& e) {
        std::cout << "  Caught: " << e.what() << "\n";
        std::cout << "  (All resources were automatically cleaned up!)\n";
    }
}

// --- 4. Nested scope lifetime ---
void nested_scopes() {
    std::cout << "\n=== Nested Scope Lifetimes ===\n";
    Resource outer("outer");
    {
        Resource inner1("inner1");
        {
            Resource innermost("innermost");
            std::cout << "  -- innermost scope --\n";
        } // innermost destroyed here
        std::cout << "  -- after innermost scope --\n";
    } // inner1 destroyed here
    std::cout << "  -- after inner scope --\n";
} // outer destroyed here

// --- 5. Temporary lifetime and the "most vexing parse" ---
void temporary_lifetime() {
    std::cout << "\n=== Temporary Object Lifetime ===\n";

    // Temporaries are destroyed at the end of the FULL EXPRESSION (the semicolon)
    std::cout << "  Creating temporary in expression:\n";
    std::string result = std::string("temp1") + " + " + std::string("temp2");
    std::cout << "  Result: " << result << "\n";
    std::cout << "  (All temporaries destroyed at the semicolon)\n";

    // But binding a temporary to a const reference extends its lifetime!
    std::cout << "\n  Lifetime extension via const reference:\n";
    {
        const Resource& ref = Resource("extended");
        std::cout << "  ref is still valid: '" << ref.name() << "'\n";
        std::cout << "  About to leave scope...\n";
    } // temporary destroyed HERE, not at the semicolon above
}

// --- 6. Static vs automatic storage duration ---
class StaticTracker {
    std::string name_;
public:
    StaticTracker(std::string name) : name_(std::move(name)) {
        std::cout << "  [STATIC +] '" << name_ << "' constructed\n";
    }
    ~StaticTracker() {
        std::cout << "  [STATIC -] '" << name_ << "' destroyed\n";
    }
};

void static_vs_auto() {
    std::cout << "\n=== Static vs Automatic Storage ===\n";

    // Static locals are constructed once and destroyed at program exit
    static StaticTracker s("static-local");
    Resource a("automatic");

    std::cout << "  Function returning — automatic destroyed, static survives\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   RAII & Scope Experiments               ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    destruction_order();
    scope_guard_demo();
    exception_safety();
    nested_scopes();
    temporary_lifetime();
    static_vs_auto();

    std::cout << "\n=== Program Exit ===\n";
    std::cout << "Static objects destroyed now (after main returns):\n";

    return 0;
}
