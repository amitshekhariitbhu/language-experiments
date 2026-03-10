// enum_power.swift
// Explores associated values, raw values, recursive enums, and pattern matching.

// import Foundation

// --- 1. Enums with associated values ---

print("=== Enums with Associated Values ===")

enum NetworkResult {
    case success(data: String, statusCode: Int)
    case failure(error: String)
    case loading(progress: Double)
}

let result: NetworkResult = .success(data: "{\"name\": \"Alice\"}", statusCode: 200)

// Pattern matching with associated values
switch result {
case .success(let data, let statusCode) where statusCode == 200:
    print("OK: \(data)")
case .success(_, let statusCode):
    print("Unexpected status: \(statusCode)")
case .failure(let error):
    print("Error: \(error)")
case .loading(let progress):
    print("Loading: \(Int(progress * 100))%")
}

// --- 2. Enum with raw values and CaseIterable ---

print("\n=== Raw Values and CaseIterable ===")

enum Planet: Int, CaseIterable {
    case mercury = 1, venus, earth, mars, jupiter, saturn, uranus, neptune
}

print("Earth is planet #\(Planet.earth.rawValue)")  // 3
print("Planet from raw value 5: \(Planet(rawValue: 5)!)")  // jupiter

print("All planets:")
for planet in Planet.allCases {
    print("  \(planet) = \(planet.rawValue)")
}

// --- 3. Recursive enums (indirect) ---
// This is how you build tree structures with enums

print("\n=== Recursive Enums (indirect) ===")

indirect enum ArithExpr {
    case number(Int)
    case add(ArithExpr, ArithExpr)
    case multiply(ArithExpr, ArithExpr)
}

func evaluate(_ expr: ArithExpr) -> Int {
    switch expr {
    case .number(let n):
        return n
    case .add(let left, let right):
        return evaluate(left) + evaluate(right)
    case .multiply(let left, let right):
        return evaluate(left) * evaluate(right)
    }
}

// (2 + 3) * 4
let expression = ArithExpr.multiply(
    .add(.number(2), .number(3)),
    .number(4)
)
print("(2 + 3) * 4 = \(evaluate(expression))")  // 20

// --- 4. Enums with methods and computed properties ---

print("\n=== Enums with Methods ===")

enum Suit: String, CaseIterable {
    case hearts = "♥️"
    case diamonds = "♦️"
    case clubs = "♣️"
    case spades = "♠️"

    var color: String {
        switch self {
        case .hearts, .diamonds: return "Red"
        case .clubs, .spades: return "Black"
        }
    }

    var isRed: Bool { color == "Red" }
}

for suit in Suit.allCases {
    print("\(suit.rawValue) \(suit) is \(suit.color)")
}

// --- 5. Pattern matching power: tuples, ranges, and where ---

print("\n=== Advanced Pattern Matching ===")

let coordinates: [(Int, Int)] = [(0, 0), (1, 0), (0, 5), (3, 3), (-1, -1)]

for point in coordinates {
    switch point {
    case (0, 0):
        print("\(point) is the origin")
    case (let x, 0):
        print("\(point) is on the x-axis at x=\(x)")
    case (0, let y):
        print("\(point) is on the y-axis at y=\(y)")
    case let (x, y) where x == y:
        print("\(point) is on the diagonal")
    case let (x, y) where x < 0 || y < 0:
        print("\(point) is in negative space")
    default:
        print("\(point) is somewhere else")
    }
}

// --- 6. if-case and for-case: pattern matching outside switch ---

print("\n=== Pattern Matching Outside Switch ===")

let results: [NetworkResult] = [
    .success(data: "A", statusCode: 200),
    .loading(progress: 0.5),
    .failure(error: "timeout"),
    .success(data: "B", statusCode: 201),
    .loading(progress: 0.9),
]

// for-case: filter and extract in one step
print("Successes only:")
for case .success(let data, let code) in results {
    print("  data=\(data), code=\(code)")
}

// if-case with a single value
let singleResult = NetworkResult.loading(progress: 0.75)
if case .loading(let p) = singleResult {
    print("Loading at \(Int(p * 100))%")
}

// --- 7. Enums can conform to protocols ---

print("\n=== Enum Conforming to CustomStringConvertible ===")

enum HTTPMethod: String, CustomStringConvertible {
    case get, post, put, delete, patch

    var description: String {
        return rawValue.uppercased()
    }
}

print("Method: \(HTTPMethod.post)")  // "POST" — uses description

print("\nKey insight: Swift enums are algebraic data types, not just integer constants.")
print("Recursive (indirect) enums let you build trees and expression languages.")
