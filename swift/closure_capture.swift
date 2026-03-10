// closure_capture.swift
// Explores capture lists, [weak self], reference cycles, and surprising capture semantics.

// import Foundation

// --- 1. Closures capture variables by reference (not value) ---

print("=== Closures Capture by Reference ===")

var counter = 0
let increment = { counter += 1 }

increment()
increment()
increment()
print("counter = \(counter)")  // 3 — the closure mutated the outer variable

// --- 2. Capture lists: capturing by value at creation time ---

print("\n=== Capture List: Capture by Value ===")

var value = 10
let captureByRef = { print("  By reference: \(value)") }
let captureByVal = { [value] in print("  By value (at capture time): \(value)") }

value = 99

captureByRef()  // 99 — sees the current value
captureByVal()  // 10 — froze the value at capture time

// --- 3. Loop capture gotcha ---

print("\n=== Loop Capture Gotcha ===")

var closures: [() -> Int] = []
for i in 0..<5 {
    closures.append { i }
}
// In Swift, each iteration of a for-in loop creates a new binding.
// So unlike some other languages, this actually works correctly!
print("Loop captures: \(closures.map { $0() })")  // [0, 1, 2, 3, 4] — no gotcha in Swift!

// Compare with a while loop where the variable IS shared:
var whileClosures: [() -> Int] = []
var j = 0
while j < 5 {
    whileClosures.append { j }  // captures j by reference
    j += 1
}
print("While captures: \(whileClosures.map { $0() })")  // [5, 5, 5, 5, 5] — all see final value!

// Fix with capture list:
var fixedClosures: [() -> Int] = []
var k = 0
while k < 5 {
    fixedClosures.append { [k] in k }  // captures k by value
    k += 1
}
print("Fixed captures: \(fixedClosures.map { $0() })")  // [0, 1, 2, 3, 4]

// --- 4. Reference cycle with closures ---

print("\n=== Reference Cycle with Closures ===")

class HTMLElement {
    let name: String
    let text: String?

    // This closure captures self strongly, creating a reference cycle!
    lazy var asHTML: () -> String = {
        if let text = self.text {
            return "<\(self.name)>\(text)</\(self.name)>"
        } else {
            return "<\(self.name) />"
        }
    }

    init(name: String, text: String? = nil) {
        self.name = name
        self.text = text
        print("  \(name) initialized")
    }

    deinit {
        print("  \(name) deinitialized")
    }
}

// Demonstrate the cycle
var heading: HTMLElement? = HTMLElement(name: "h1", text: "Welcome")
print("  HTML: \(heading!.asHTML())")
heading = nil  // deinit will NOT be called — reference cycle!
print("  (h1 was NOT deinitialized — memory leak!)")

// --- 5. Breaking the cycle with [unowned self] ---

print("\n=== Breaking Cycle with [unowned self] ===")

class FixedHTMLElement {
    let name: String
    let text: String?

    lazy var asHTML: () -> String = { [unowned self] in
        if let text = self.text {
            return "<\(self.name)>\(text)</\(self.name)>"
        } else {
            return "<\(self.name) />"
        }
    }

    init(name: String, text: String? = nil) {
        self.name = name
        self.text = text
        print("  \(name) initialized")
    }

    deinit {
        print("  \(name) deinitialized")
    }
}

var paragraph: FixedHTMLElement? = FixedHTMLElement(name: "p", text: "Hello")
print("  HTML: \(paragraph!.asHTML())")
paragraph = nil  // deinit IS called — no cycle!

// --- 6. Escaping vs non-escaping closures ---

print("\n=== Escaping vs Non-Escaping ===")

var savedClosures: [() -> Void] = []

func doSomethingNonEscaping(closure: () -> Void) {
    // This closure can't outlive the function — compiler guarantees it
    closure()
}

func doSomethingEscaping(closure: @escaping () -> Void) {
    // This closure is stored and can be called later
    savedClosures.append(closure)
}

doSomethingNonEscaping { print("  Non-escaping: runs immediately") }
doSomethingEscaping { print("  Escaping: runs later") }

print("  Calling saved closure:")
savedClosures.first?()

// --- 7. Autoclosure: lazy evaluation disguised as a value ---

print("\n=== @autoclosure: Lazy Evaluation ===")

func logIfTrue(_ condition: Bool, message: @autoclosure () -> String) {
    if condition {
        print("  LOG: \(message())")  // message() is only evaluated here
    }
}

var expensiveCallCount = 0
func expensiveComputation() -> String {
    expensiveCallCount += 1
    return "computed result #\(expensiveCallCount)"
}

logIfTrue(true, message: expensiveComputation())   // evaluated
logIfTrue(false, message: expensiveComputation())  // NOT evaluated — autoclosure deferred it
print("  Expensive computation called \(expensiveCallCount) time(s)")  // 1, not 2!

print("\nKey insight: for-in loops create a new binding per iteration (safe),")
print("but while loops share the variable (unsafe). Capture lists freeze values.")
