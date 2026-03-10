// object_slicing.cpp
// Demonstrates object slicing — one of C++'s most insidious pitfalls.
// When a derived object is passed by value to a base parameter,
// the derived part is silently chopped off.

#include <iostream>
#include <string>
#include <vector>
#include <memory>

// --- Base and Derived classes ---
class Animal {
public:
    std::string name;
    Animal(std::string n) : name(std::move(n)) {}

    virtual std::string speak() const { return name + " says: ..."; }
    virtual ~Animal() = default;

    // Copy constructor — this is what enables slicing
    Animal(const Animal& other) : name(other.name) {
        std::cout << "  [Animal copy ctor called for '" << name << "']\n";
    }
    Animal& operator=(const Animal&) = default;
};

class Dog : public Animal {
public:
    std::string trick;
    Dog(std::string n, std::string t) : Animal(std::move(n)), trick(std::move(t)) {}

    std::string speak() const override {
        return name + " says: Woof! (knows trick: " + trick + ")";
    }
};

class Cat : public Animal {
public:
    int lives;
    Cat(std::string n, int l) : Animal(std::move(n)), lives(l) {}

    std::string speak() const override {
        return name + " says: Meow! (has " + std::to_string(lives) + " lives)";
    }
};

// --- 1. Slicing by passing by value ---
void describe_animal_by_value(Animal a) {
    // 'a' is a BASE Animal — the Dog/Cat part has been sliced off!
    // Virtual dispatch goes to Animal::speak(), not Dog/Cat::speak()
    std::cout << "  By value: " << a.speak() << "\n";
}

void describe_animal_by_ref(const Animal& a) {
    // Reference preserves the dynamic type — virtual dispatch works correctly
    std::cout << "  By ref:   " << a.speak() << "\n";
}

void slicing_demo() {
    std::cout << "=== Object Slicing: Pass by Value vs Reference ===\n";
    Dog rex("Rex", "roll over");

    std::cout << "Original: " << rex.speak() << "\n";
    describe_animal_by_ref(rex);   // Works correctly
    describe_animal_by_value(rex); // SLICED — loses Dog data and virtual dispatch
    std::cout << "\n";
}

// --- 2. Slicing in containers ---
void container_slicing() {
    std::cout << "=== Slicing in Containers ===\n";

    // WRONG: storing by value slices everything to Animal
    std::vector<Animal> bad_zoo;
    bad_zoo.push_back(Dog("Buddy", "shake"));
    bad_zoo.push_back(Cat("Whiskers", 9));

    std::cout << "vector<Animal> (SLICED):\n";
    for (const auto& a : bad_zoo)
        std::cout << "  " << a.speak() << "\n";

    // CORRECT: store pointers (or smart pointers) to preserve polymorphism
    std::vector<std::unique_ptr<Animal>> good_zoo;
    good_zoo.push_back(std::make_unique<Dog>("Buddy", "shake"));
    good_zoo.push_back(std::make_unique<Cat>("Whiskers", 9));

    std::cout << "\nvector<unique_ptr<Animal>> (CORRECT):\n";
    for (const auto& a : good_zoo)
        std::cout << "  " << a->speak() << "\n";
    std::cout << "\n";
}

// --- 3. Slicing in assignment ---
void assignment_slicing() {
    std::cout << "=== Slicing via Assignment ===\n";

    Dog rex("Rex", "fetch");
    Animal generic("Generic");

    std::cout << "Before: generic.speak() = " << generic.speak() << "\n";

    generic = rex; // SLICES! Only copies the Animal part.
    std::cout << "After generic = rex:\n";
    std::cout << "  generic.speak() = " << generic.speak() << "\n";
    std::cout << "  generic.name = \"" << generic.name << "\"\n";
    std::cout << "  (Dog's trick is lost, virtual table still points to Animal)\n\n";
}

// --- 4. Slicing in exception handling ---
void exception_slicing() {
    std::cout << "=== Slicing in Exception Handling ===\n";

    class BaseException : public std::exception {
    public:
        virtual const char* what() const noexcept override {
            return "BaseException";
        }
    };

    class DerivedException : public BaseException {
    public:
        const char* what() const noexcept override {
            return "DerivedException (detailed info)";
        }
    };

    // WRONG: catch by value — slices the exception!
    try {
        throw DerivedException();
    } catch (BaseException e) { // BY VALUE — slicing!
        std::cout << "  Caught by value: " << e.what() << "\n";
    }

    // CORRECT: catch by reference
    try {
        throw DerivedException();
    } catch (const BaseException& e) { // BY REFERENCE — correct
        std::cout << "  Caught by ref:   " << e.what() << "\n";
    }
    std::cout << "\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n";
    std::cout << "║   Object Slicing Experiments             ║\n";
    std::cout << "╚══════════════════════════════════════════╝\n\n";

    slicing_demo();
    container_slicing();
    assignment_slicing();
    exception_slicing();

    std::cout << "=== Key Takeaway ===\n";
    std::cout << "Always use references or pointers for polymorphic objects.\n";
    std::cout << "Consider making base classes non-copyable to prevent slicing.\n";

    return 0;
}
