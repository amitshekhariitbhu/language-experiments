// generics_constraints.swift
// Explores where clauses, associated types, type erasure, and conditional conformance.

// import Foundation

// --- 1. Basic generics with constraints ---

print("=== Generic Functions with Constraints ===")

func findIndex<T: Equatable>(of value: T, in array: [T]) -> Int? {
    for (index, element) in array.enumerated() {
        if element == value {
            return index
        }
    }
    return nil
}

print("Index of 'c' in [a,b,c,d]: \(String(describing: findIndex(of: "c", in: ["a","b","c","d"])))")
print("Index of 42 in [10,20,42,50]: \(String(describing: findIndex(of: 42, in: [10,20,42,50])))")

// --- 2. Where clauses for complex constraints ---

print("\n=== Where Clauses ===")

func allEqual<C: Collection>(_ collection: C) -> Bool where C.Element: Equatable {
    guard let first = collection.first else { return true }
    return collection.allSatisfy { $0 == first }
}

print("[1,1,1] all equal: \(allEqual([1, 1, 1]))")       // true
print("[1,2,1] all equal: \(allEqual([1, 2, 1]))")       // false
print("[\"a\",\"a\"] all equal: \(allEqual(["a", "a"]))") // true

// --- 3. Associated types in protocols ---

print("\n=== Associated Types ===")

protocol Stack {
    associatedtype Element
    mutating func push(_ item: Element)
    mutating func pop() -> Element?
    var count: Int { get }
}

struct IntStack: Stack {
    // The compiler infers Element = Int from the push/pop signatures
    private var items: [Int] = []

    mutating func push(_ item: Int) {
        items.append(item)
    }

    mutating func pop() -> Int? {
        return items.popLast()
    }

    var count: Int { items.count }
}

struct GenericStack<T>: Stack {
    private var items: [T] = []

    mutating func push(_ item: T) {
        items.append(item)
    }

    mutating func pop() -> T? {
        return items.popLast()
    }

    var count: Int { items.count }
}

var stack = GenericStack<String>()
stack.push("hello")
stack.push("world")
print("Popped: \(stack.pop() ?? "nil"), remaining: \(stack.count)")

// --- 4. Type erasure with AnyHashable ---

print("\n=== Type Erasure: AnyHashable ===")

// AnyHashable wraps any Hashable type, erasing the specific type.
// This is how you put mixed types into a Set or Dictionary key.

let anySet: Set<AnyHashable> = [1, "hello", 3.14, true]
print("Mixed set: \(anySet)")
print("Contains 1: \(anySet.contains(1))")
print("Contains \"hello\": \(anySet.contains("hello"))")

// The surprise: AnyHashable preserves the underlying type for equality
let a: AnyHashable = 1       // Int
let b: AnyHashable = 1.0     // Double
print("\nAnyHashable(1) == AnyHashable(1.0): \(a == b)")  // false! Different underlying types

// But within the same type, it works as expected:
let c: AnyHashable = 42
let d: AnyHashable = 42
print("AnyHashable(42) == AnyHashable(42): \(c == d)")  // true

// --- 5. Conditional conformance ---

print("\n=== Conditional Conformance ===")

// Array<T> conforms to Equatable ONLY when T conforms to Equatable
// This is conditional conformance.

struct Wrapper<T> {
    let value: T
}

// Make Wrapper equatable only when T is
extension Wrapper: Equatable where T: Equatable {
    static func == (lhs: Wrapper, rhs: Wrapper) -> Bool {
        return lhs.value == rhs.value
    }
}

// Make Wrapper hashable only when T is
extension Wrapper: Hashable where T: Hashable {
    func hash(into hasher: inout Hasher) {
        hasher.combine(value)
    }
}

let w1 = Wrapper(value: 42)
let w2 = Wrapper(value: 42)
print("Wrapper(42) == Wrapper(42): \(w1 == w2)")  // true — because Int: Equatable

// Can even use Wrapper<Int> in a Set because of conditional Hashable conformance
let wrapperSet: Set<Wrapper<Int>> = [Wrapper(value: 1), Wrapper(value: 2), Wrapper(value: 1)]
print("Set of wrappers: \(wrapperSet.count) unique values")  // 2

// --- 6. Generic specialization with overloading ---

print("\n=== Generic Specialization ===")

func describe<T>(_ value: T) -> String {
    return "Generic: \(value)"
}

func describe(_ value: Int) -> String {
    return "Specialized for Int: \(value)"
}

func describe(_ value: String) -> String {
    return "Specialized for String: \(value)"
}

print(describe(42))        // "Specialized for Int: 42"
print(describe("hello"))   // "Specialized for String: hello"
print(describe(3.14))      // "Generic: 3.14"

// The surprise: the compiler picks the most specific overload at compile time
func callDescribe<T>(_ value: T) -> String {
    return describe(value)  // Always calls the generic version!
}

print(callDescribe(42))    // "Generic: 42" — NOT the Int specialization!
// Because inside callDescribe, the compiler only knows T, not that it's Int.

// --- 7. Opaque return types (some Protocol) ---

print("\n=== Opaque Return Types (some) ===")

protocol Shape {
    func area() -> Double
}

struct Circle: Shape {
    let radius: Double
    func area() -> Double { .pi * radius * radius }
}

struct Square: Shape {
    let side: Double
    func area() -> Double { side * side }
}

// `some Shape` means "a specific Shape type, but the caller doesn't know which"
// The compiler DOES know the concrete type — enabling optimizations.
func makeShape() -> some Shape {
    return Circle(radius: 5)
    // Can't sometimes return Circle, sometimes Square — must be consistent
}

let shape = makeShape()
print("Shape area: \(shape.area())")

print("\nKey insight: Generic overload resolution happens at compile time based on static type.")
print("A function calling describe<T>(T) always uses the generic version, even if T is Int.")
