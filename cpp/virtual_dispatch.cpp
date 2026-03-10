// virtual_dispatch.cpp
// Demonstrates surprising virtual dispatch behaviors:
// virtual calls in constructors/destructors, diamond inheritance, and more.

#include <iostream>
#include <string>
#include <memory>

// --- 1. Virtual dispatch does NOT work as expected in constructors ---
class Base {
public:
    Base() {
        std::cout << "  Base::Base() — calling identify():\n";
        identify();  // Does NOT call Derived::identify()!
        // During Base construction, the object IS a Base.
        // The vtable points to Base's methods.
    }

    virtual void identify() {
        std::cout << "    -> I am Base\n";
    }

    virtual ~Base() {
        std::cout << "  Base::~Base() — calling identify():\n";
        identify();  // Also calls Base::identify(), not Derived!
    }
};

class Derived : public Base {
    std::string data_;
public:
    Derived(std::string data) : Base(), data_(std::move(data)) {
        std::cout << "  Derived::Derived() — calling identify():\n";
        identify();  // NOW it calls Derived::identify()
    }

    void identify() override {
        std::cout << "    -> I am Derived with data: '" << data_ << "'\n";
    }

    ~Derived() override {
        std::cout << "  Derived::~Derived() — calling identify():\n";
        identify();  // Calls Derived::identify()
    }
};

void constructor_dispatch() {
    std::cout << "=== Virtual Dispatch in Constructors/Destructors ===\n";
    std::cout << "Creating Derived:\n";
    {
        Derived d("hello");
    }
    std::cout << "Key: During Base ctor/dtor, the object IS a Base — derived parts don't exist yet/anymore.\n\n";
}

// --- 2. The Diamond Problem ---
class Animal {
public:
    std::string name;
    Animal(std::string n) : name(std::move(n)) {
        std::cout << "  Animal('" << name << "') constructed\n";
    }
    virtual ~Animal() = default;
};

// WITHOUT virtual inheritance: two copies of Animal
class DogNonVirtual : public Animal {
public:
    DogNonVirtual(std::string n) : Animal(std::move(n)) {}
};

class PetNonVirtual : public Animal {
public:
    PetNonVirtual(std::string n) : Animal(std::move(n)) {}
};

// This creates TWO Animal subobjects — ambiguous!
class PetDogNonVirtual : public DogNonVirtual, public PetNonVirtual {
public:
    PetDogNonVirtual(std::string n)
        : DogNonVirtual(n + "(Dog)"), PetNonVirtual(n + "(Pet)") {}
};

// WITH virtual inheritance: one shared copy of Animal
class DogVirtual : virtual public Animal {
public:
    DogVirtual(std::string n) : Animal(std::move(n)) {}
};

class PetVirtual : virtual public Animal {
public:
    PetVirtual(std::string n) : Animal(std::move(n)) {}
};

class PetDogVirtual : public DogVirtual, public PetVirtual {
public:
    // With virtual inheritance, the MOST DERIVED class must initialize the virtual base
    PetDogVirtual(std::string n) : Animal(n), DogVirtual(n), PetVirtual(n) {}
};

void diamond_problem() {
    std::cout << "=== The Diamond Problem ===\n";

    std::cout << "\nWithout virtual inheritance (TWO Animal subobjects):\n";
    PetDogNonVirtual pd1("Rex");
    // pd1.name; // ERROR: ambiguous — which Animal's name?
    std::cout << "  DogNonVirtual::name = " << pd1.DogNonVirtual::name << "\n";
    std::cout << "  PetNonVirtual::name = " << pd1.PetNonVirtual::name << "\n";
    std::cout << "  sizeof(PetDogNonVirtual) = " << sizeof(PetDogNonVirtual) << "\n";

    std::cout << "\nWith virtual inheritance (ONE shared Animal subobject):\n";
    PetDogVirtual pd2("Rex");
    std::cout << "  name = " << pd2.name << " (unambiguous!)\n";
    std::cout << "  sizeof(PetDogVirtual) = " << sizeof(PetDogVirtual)
              << " (larger due to vptr overhead)\n\n";
}

// --- 3. Non-virtual interface (NVI) pattern ---
class Formatter {
public:
    // Public non-virtual interface
    std::string format(const std::string& data) {
        return prefix() + transform(data) + suffix();
    }
    virtual ~Formatter() = default;

private:
    // Private virtual functions — derived classes override these
    virtual std::string prefix() { return "["; }
    virtual std::string transform(const std::string& s) { return s; }
    virtual std::string suffix() { return "]"; }
};

class HTMLFormatter : public Formatter {
private:
    std::string prefix() override { return "<b>"; }
    std::string transform(const std::string& s) override { return s; }
    std::string suffix() override { return "</b>"; }
};

class ShoutFormatter : public Formatter {
private:
    std::string prefix() override { return ">>> "; }
    std::string transform(const std::string& s) override {
        std::string result = s;
        for (char& c : result) c = std::toupper(c);
        return result;
    }
    std::string suffix() override { return " <<<"; }
};

void nvi_demo() {
    std::cout << "=== Non-Virtual Interface (NVI) Pattern ===\n";
    std::cout << "Private virtual methods CAN be overridden!\n";
    std::cout << "Access control is checked at the call site, not the override.\n\n";

    Formatter base;
    HTMLFormatter html;
    ShoutFormatter shout;

    std::string data = "hello world";
    std::cout << "  Base:  " << base.format(data) << "\n";
    std::cout << "  HTML:  " << html.format(data) << "\n";
    std::cout << "  Shout: " << shout.format(data) << "\n\n";
}

// --- 4. Hiding vs overriding ---
class Parent {
public:
    virtual void greet() { std::cout << "  Parent::greet()\n"; }
    virtual void greet(int n) { std::cout << "  Parent::greet(" << n << ")\n"; }
};

class Child : public Parent {
public:
    // This HIDES Parent::greet(int)! It doesn't override it.
    void greet() override { std::cout << "  Child::greet()\n"; }
    // Parent::greet(int) is now hidden in Child's scope
};

void hiding_vs_overriding() {
    std::cout << "=== Hiding vs Overriding ===\n";
    Child c;
    Parent* p = &c;

    c.greet();       // OK: Child::greet()
    // c.greet(42);  // ERROR: hidden! Child::greet() hides all Parent::greet overloads
    p->greet(42);    // OK through base pointer: Parent::greet(int)

    std::cout << "Via Child object:\n";
    c.greet();
    std::cout << "Via Parent pointer (can still call greet(int)):\n";
    p->greet(42);
    std::cout << "Overriding one overload HIDES all others with the same name!\n\n";
}

int main() {
    std::cout << "╔══════════════════════════════════════════════════╗\n";
    std::cout << "║   Virtual Dispatch Experiments                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════╝\n\n";

    constructor_dispatch();
    diamond_problem();
    nvi_demo();
    hiding_vs_overriding();

    return 0;
}
