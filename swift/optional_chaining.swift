// optional_chaining.swift
// Explores Optional unwrapping, nil coalescing, optional chaining, and surprising edge cases.

// import Foundation

// --- 1. Optional is just an enum ---
// Swift's Optional<T> is literally: enum Optional<T> { case none; case some(T) }

print("=== Optional is an enum ===")

let x: Int? = 42
switch x {
case .none:
    print("x is nil")
case .some(let value):
    print("x contains \(value)")
}

// --- 2. Nested optionals: Optional<Optional<Int>> ---
// This is a real thing and causes subtle bugs.

print("\n=== Nested Optionals (Double-wrapped) ===")

let dict: [String: Int?] = ["a": 1, "b": nil, "c": 3]

// dict["a"] returns Int?? (Optional<Optional<Int>>)
// because the dictionary lookup itself is optional, AND the value is optional.

let valueA = dict["a"]   // Optional(Optional(1)) — type is Int??
let valueB = dict["b"]   // Optional(nil)         — key exists, value is nil
let valueD = dict["d"]   // nil                   — key doesn't exist

print("dict[\"a\"] = \(String(describing: valueA))")  // Optional(Optional(1))
print("dict[\"b\"] = \(String(describing: valueB))")  // Optional(Optional(nil))
print("dict[\"d\"] = \(String(describing: valueD))")  // nil

// The surprising part: both "b" and "d" look nil-ish, but they're different!
if let innerB = dict["b"] {
    print("Key 'b' exists. Inner value: \(String(describing: innerB))")  // Inner value: nil
} else {
    print("Key 'b' does not exist")
}

if let innerD = dict["d"] {
    print("Key 'd' exists. Inner value: \(String(describing: innerD))")
} else {
    print("Key 'd' does not exist")  // This prints
}

// --- 3. Nil coalescing with different types ---

print("\n=== Nil Coalescing Chains ===")

let a: Int? = nil
let b: Int? = nil
let c: Int? = 7

let result = a ?? b ?? c ?? 0
print("a ?? b ?? c ?? 0 = \(result)")  // 7

// --- 4. Optional chaining returns Optional ---

print("\n=== Optional Chaining ===")

class Company {
    var ceo: Person?
}

class Person {
    var name: String
    var address: Address?
    init(name: String) { self.name = name }
}

class Address {
    var street: String
    var zip: String
    init(street: String, zip: String) { self.street = street; self.zip = zip }

    func formatted() -> String {
        return "\(street), \(zip)"
    }
}

let company = Company()
// No CEO set — entire chain short-circuits to nil
let street = company.ceo?.address?.street
print("Street (no CEO): \(String(describing: street))")  // nil

company.ceo = Person(name: "Alice")
company.ceo?.address = Address(street: "123 Main St", zip: "94105")

let formatted = company.ceo?.address?.formatted()
print("Formatted: \(String(describing: formatted))")  // Optional("123 Main St, 94105")
// Note: formatted() returns String, but through optional chaining it becomes String?

// --- 5. Optional map and flatMap ---

print("\n=== Optional map vs flatMap ===")

let maybeString: String? = "42"

// .map applies a transform if the value exists
let mapped = maybeString.map { Int($0) }
print("map result: \(String(describing: mapped))")  // Optional(Optional(42)) — nested!

// .flatMap flattens the nesting
let flatMapped = maybeString.flatMap { Int($0) }
print("flatMap result: \(String(describing: flatMapped))")  // Optional(42) — flat!

// --- 6. Comparing optionals: a gotcha ---

print("\n=== Comparing Optionals ===")

let p: Int? = nil
let q: Int? = nil
print("nil == nil: \(p == q)")  // true — Equatable works on Optional<Int>

// But ordering comparisons were removed in Swift 3+ for safety.
// let lessThan = p < q  // This would be a compiler error in modern Swift.

// The surprising part: Optional conforms to Equatable when Wrapped does,
// allowing nil == nil to be true, which is philosophically debatable.

print("\nKey insight: Optional<Optional<T>> is a real type that causes subtle bugs.")
print("Dictionary lookups on [Key: Value?] return Value?? — double-wrapped optionals.")
