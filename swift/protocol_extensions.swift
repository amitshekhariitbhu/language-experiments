// protocol_extensions.swift
// Explores protocol-oriented programming, default implementations,
// and the surprising static vs dynamic dispatch behavior in extensions.

// import Foundation

// --- 1. Protocol with default implementation ---

print("=== Default Implementation in Protocol Extension ===")

protocol Greetable {
    func greet() -> String
}

extension Greetable {
    func greet() -> String {
        return "Hello, I'm a Greetable!"
    }
}

struct Dog: Greetable {
    // Uses the default implementation — no need to implement greet()
}

struct Cat: Greetable {
    func greet() -> String {
        return "Meow! I'm a Cat!"
    }
}

print(Dog().greet())  // "Hello, I'm a Greetable!"
print(Cat().greet())  // "Meow! I'm a Cat!"

// --- 2. THE BIG GOTCHA: Static dispatch in extensions ---
// Methods defined ONLY in the extension (not in the protocol requirement)
// use STATIC dispatch, not dynamic dispatch.

print("\n=== Static vs Dynamic Dispatch (The Gotcha) ===")

protocol Describable {
    func dynamicMethod() -> String     // Declared in protocol — uses dynamic dispatch
}

extension Describable {
    func dynamicMethod() -> String {   // Default for the protocol requirement
        return "Describable (default)"
    }

    func staticMethod() -> String {    // ONLY in extension — uses static dispatch!
        return "Describable (extension-only)"
    }
}

struct Bird: Describable {
    func dynamicMethod() -> String {
        return "Bird (overridden)"
    }

    func staticMethod() -> String {
        return "Bird (overridden)"
    }
}

let bird = Bird()
let describable: Describable = bird

print("bird.dynamicMethod()        = \(bird.dynamicMethod())")        // "Bird (overridden)"
print("describable.dynamicMethod() = \(describable.dynamicMethod())") // "Bird (overridden)" ✓ dynamic dispatch

print("bird.staticMethod()         = \(bird.staticMethod())")         // "Bird (overridden)"
print("describable.staticMethod()  = \(describable.staticMethod())")  // "Describable (extension-only)" ← SURPRISE!

// When called through the protocol type, staticMethod() ignores Bird's override
// because it was never declared in the protocol — the compiler resolves it at compile time.

// --- 3. Protocol composition ---

print("\n=== Protocol Composition ===")

protocol HasName {
    var name: String { get }
}

protocol HasAge {
    var age: Int { get }
}

// You can require both protocols without creating a new one
func introduce(_ entity: HasName & HasAge) {
    print("\(entity.name), age \(entity.age)")
}

struct Student: HasName, HasAge {
    let name: String
    let age: Int
}

introduce(Student(name: "Alice", age: 22))

// --- 4. Protocol extension with constraints ---

print("\n=== Constrained Protocol Extensions ===")

protocol Container {
    associatedtype Item
    var items: [Item] { get }
}

extension Container where Item: Numeric {
    func sum() -> Item {
        return items.reduce(0, +)
    }
}

extension Container where Item == String {
    func joined() -> String {
        return items.joined(separator: ", ")
    }
}

struct NumberBox: Container {
    let items: [Int]
}

struct StringBox: Container {
    let items: [String]
}

print("Sum: \(NumberBox(items: [1, 2, 3, 4, 5]).sum())")          // 15
print("Joined: \(StringBox(items: ["a", "b", "c"]).joined())")    // "a, b, c"
// NumberBox doesn't have .joined(), StringBox doesn't have .sum() — type-safe!

// --- 5. Retroactive conformance: extending types you don't own ---

print("\n=== Retroactive Conformance ===")

protocol Reversible {
    func reversed() -> Self
}

// Make Int conform to our custom protocol
extension Int: Reversible {
    func reversed() -> Int {
        let s = String(String(abs(self)).reversed())
        let result = Int(s)!
        return self < 0 ? -result : result
    }
}

print("12345.reversed() = \(12345.reversed())")   // 54321
print("(-678).reversed() = \((-678).reversed())")  // -876

print("\nKey insight: Methods only in a protocol extension (not declared in the protocol)")
print("use STATIC dispatch. Overrides are silently ignored when called through the protocol type.")
