// access_control.swift
// Explores public/internal/fileprivate/private and their surprising scope rules.
// Note: In a single-file script, some distinctions collapse. We demonstrate what we can.

// import Foundation

// --- 1. private vs fileprivate ---
// In Swift, `private` restricts to the enclosing declaration (type + extensions in same file).
// `fileprivate` restricts to the file.

print("=== private vs fileprivate ===")

class BankAccount {
    private var balance: Double = 0  // only accessible within BankAccount (and its extensions in this file)
    fileprivate var accountNumber: String = "1234"  // accessible anywhere in this file

    func deposit(_ amount: Double) {
        balance += amount
    }

    func getBalance() -> Double {
        return balance
    }
}

// Extension in the same file CAN access private members!
// This changed in Swift 4 — previously private was truly restricted to the { } block.
extension BankAccount {
    func secretAudit() -> String {
        // This works because we're in the same file
        return "Balance is \(balance), account \(accountNumber)"
    }
}

let account = BankAccount()
account.deposit(1000)
print(account.secretAudit())

// --- 2. Struct memberwise initializer and access control ---

print("\n=== Memberwise Initializer Access Level ===")

struct Config {
    var host: String = "localhost"
    private var secret: String = "abc123"  // private property

    // Because 'secret' is private, the auto-generated memberwise initializer
    // becomes private too! You can only use the default values.

    // We need an explicit public init if we want external initialization
    init(host: String) {
        self.host = host
        // secret uses its default value
    }

    func describe() -> String {
        return "host=\(host), secret=\(secret)"
    }
}

let config = Config(host: "example.com")
// Config(host: "example.com", secret: "xyz")  // ERROR: 'secret' is inaccessible
print(config.describe())

// --- 3. private(set): read-only from outside, writable from inside ---

print("\n=== private(set): External Read-Only ===")

struct Temperature {
    private(set) var celsius: Double  // readable from outside, writable only inside

    init(celsius: Double) {
        self.celsius = celsius
    }

    var fahrenheit: Double {
        get { celsius * 9.0 / 5.0 + 32.0 }
        set { celsius = (newValue - 32.0) * 5.0 / 9.0 }
    }
}

var temp = Temperature(celsius: 100)
print("Celsius: \(temp.celsius)")      // readable
print("Fahrenheit: \(temp.fahrenheit)") // 212.0
temp.fahrenheit = 32                    // This sets celsius internally
print("After setting 32°F: \(temp.celsius)°C")  // 0.0
// temp.celsius = 50  // ERROR: setter is private

// --- 4. Access control and protocol conformance ---

print("\n=== Access Control with Protocols ===")

protocol Identifiable {
    var id: String { get }
}

class User: Identifiable {
    // Protocol requires 'id' to be at least as accessible as the conformance.
    // If the class were public, 'id' must be public too.
    let id: String
    private let password: String

    init(id: String, password: String) {
        self.id = id
        self.password = password
    }
}

let user = User(id: "alice", password: "secret")
print("User id: \(user.id)")
// print(user.password)  // ERROR: 'password' is private

// --- 5. fileprivate and type nesting ---

print("\n=== fileprivate in Nested Types ===")

struct OuterType {
    fileprivate struct InnerType {
        fileprivate var secret = "inner secret"

        func reveal() -> String { return secret }
    }

    private var inner = InnerType()

    func exposeInner() -> String {
        return inner.reveal()
    }
}

let outer = OuterType()
print(outer.exposeInner())

// Because we're in the same file, we CAN access InnerType directly:
fileprivate let inner = OuterType.InnerType()
print("Direct access to fileprivate inner: \(inner.reveal())")

// --- 6. Access levels summary ---

print("\n=== Access Level Hierarchy ===")
print("open       — visible everywhere, subclassable/overridable outside module")
print("public     — visible everywhere, NOT subclassable outside module")
print("internal   — visible within the module (DEFAULT)")
print("fileprivate — visible within the source file")
print("private    — visible within the enclosing declaration (+ same-file extensions)")

// --- 7. The surprising part: private in extensions ---

print("\n=== Surprise: private accessible in same-file extensions ===")

class SecretKeeper {
    private var secrets: [String] = ["the cake is a lie"]
}

extension SecretKeeper {
    func addSecret(_ s: String) {
        secrets.append(s)  // Accessing private property from extension — works in same file!
    }

    func tellSecrets() -> String {
        return secrets.joined(separator: ", ")
    }
}

let keeper = SecretKeeper()
keeper.addSecret("swift is fun")
print("Secrets: \(keeper.tellSecrets())")

print("\nKey insight: Swift's 'private' allows access from same-file extensions,")
print("making it much more permissive than most developers expect.")
