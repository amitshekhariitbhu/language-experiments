// value_vs_reference.swift
// Explores struct (value) vs class (reference) semantics, and Swift's copy-on-write optimization for arrays.

// import Foundation

// --- 1. Struct vs Class: the basics ---

struct PointStruct {
    var x: Int
    var y: Int
}

class PointClass {
    var x: Int
    var y: Int
    init(x: Int, y: Int) { self.x = x; self.y = y }
}

print("=== Struct (Value Type) vs Class (Reference Type) ===")

var s1 = PointStruct(x: 1, y: 2)
var s2 = s1          // copies the value
s2.x = 99
print("Struct: s1.x = \(s1.x), s2.x = \(s2.x)")
// s1.x is still 1 — independent copy

var c1 = PointClass(x: 1, y: 2)
var c2 = c1          // copies the reference
c2.x = 99
print("Class:  c1.x = \(c1.x), c2.x = \(c2.x)")
// c1.x is now 99 — both variables point to the same object

// --- 2. Mutating a struct inside a function ---

print("\n=== Structs are passed by value into functions ===")

func tryToMutate(_ point: PointStruct) {
    // point.x = 42  // ERROR: 'point' is a 'let' constant
    var local = point
    local.x = 42
    print("Inside function: local.x = \(local.x)")
}

let original = PointStruct(x: 10, y: 20)
tryToMutate(original)
print("After function:  original.x = \(original.x)")  // still 10

// --- 3. Copy-on-Write (CoW) for Arrays ---
// Arrays are structs, but Swift doesn't copy the underlying buffer
// until one of the copies is mutated. This is an optimization.

print("\n=== Copy-on-Write for Arrays ===")

var arr1 = [1, 2, 3, 4, 5]
var arr2 = arr1  // no copy yet — they share the same buffer

// We can't directly observe the buffer address from pure Swift easily,
// but we can demonstrate the *effect* of CoW by timing or by identity.

print("Before mutation:")
print("  arr1: \(arr1)")
print("  arr2: \(arr2)")

arr2.append(6)  // NOW the buffer is copied (copy-on-write triggers)

print("After mutating arr2:")
print("  arr1: \(arr1)")  // [1, 2, 3, 4, 5] — unaffected
print("  arr2: \(arr2)")  // [1, 2, 3, 4, 5, 6]

// --- 4. Surprising: A struct containing a class reference ---
// The struct is copied by value, but the class inside is still shared!

print("\n=== Struct containing a class reference (shallow copy trap) ===")

class Engine {
    var horsepower: Int
    init(hp: Int) { self.horsepower = hp }
}

struct Car {
    var name: String
    var engine: Engine  // reference type inside a value type!
}

var car1 = Car(name: "Tesla", engine: Engine(hp: 400))
var car2 = car1  // struct is copied, but engine reference is shared!

car2.name = "Rivian"
car2.engine.horsepower = 800

print("car1: \(car1.name), \(car1.engine.horsepower) hp")
print("car2: \(car2.name), \(car2.engine.horsepower) hp")
// car1.name is still "Tesla" (String is a value type)
// BUT car1.engine.horsepower is 800 — the Engine object is shared!

print("\nKey insight: Embedding a class inside a struct creates a shallow copy trap.")
print("The struct copies by value, but its class properties still share references.")
