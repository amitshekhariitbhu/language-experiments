// string_unicode.swift
// Explores Character vs Unicode.Scalar, grapheme clusters, and surprising emoji counting.

// import Foundation

// --- 1. String.count vs UTF-8/UTF-16 lengths ---

print("=== String Lengths: count vs encoding lengths ===")

let ascii = "Hello"
let emoji = "Hello 👋"
let flag = "🇺🇸"
let family = "👨‍👩‍👧‍👦"

func showLengths(_ label: String, _ str: String) {
    print("  \"\(str)\"")
    print("    .count (Characters):  \(str.count)")
    print("    .utf8.count:          \(str.utf8.count)")
    print("    .utf16.count:         \(str.utf16.count)")
    print("    .unicodeScalars.count: \(str.unicodeScalars.count)")
}

showLengths("ASCII", ascii)
showLengths("With emoji", emoji)
showLengths("Flag", flag)
showLengths("Family", family)

// The flag emoji 🇺🇸 is TWO Unicode scalars (regional indicators U+1F1FA U+1F1F8)
// but Swift counts it as ONE Character (grapheme cluster).

// The family emoji 👨‍👩‍👧‍👦 is SEVEN Unicode scalars (4 people + 3 ZWJ)
// but Swift counts it as ONE Character!

// --- 2. Grapheme clusters: combining characters ---

print("\n=== Grapheme Clusters: Combining Characters ===")

let eAccent1 = "é"       // U+00E9 (precomposed)
let eAccent2 = "e\u{0301}"  // e + combining acute accent (decomposed)

print("Precomposed \"é\": scalars = \(eAccent1.unicodeScalars.count), chars = \(eAccent1.count)")
print("Decomposed \"é\": scalars = \(eAccent2.unicodeScalars.count), chars = \(eAccent2.count)")
print("Are they equal? \(eAccent1 == eAccent2)")  // true! Swift normalizes for comparison

// This is huge: two strings with different byte representations are considered equal
// because Swift compares at the grapheme cluster level (canonical equivalence).

// --- 3. String indexing is not O(1) ---

print("\n=== String Indexing: No Integer Subscript ===")

let greeting = "Hello, 世界!"

// greeting[0]  // ERROR: String doesn't support integer subscript
// This is because characters have variable width — you can't jump to position N.

let index = greeting.index(greeting.startIndex, offsetBy: 7)
print("Character at position 7: \(greeting[index])")  // 世

// Iterating is fine — it's O(n) total:
for (i, char) in greeting.enumerated() {
    print("  [\(i)] \(char) — is ASCII: \(char.isASCII)")
}

// --- 4. Character vs Unicode.Scalar ---

print("\n=== Character vs Unicode.Scalar ===")

let sparkle: Character = "✨"
print("Character: \(sparkle)")
print("Scalars in Character:")
for scalar in sparkle.unicodeScalars {
    print("  U+\(String(scalar.value, radix: 16, uppercase: true)) (\(scalar))")
}

// A Character can contain MULTIPLE scalars
let skinToneWave: Character = "👋🏽"
print("\nCharacter: \(skinToneWave)")
print("Scalars in Character:")
for scalar in skinToneWave.unicodeScalars {
    print("  U+\(String(scalar.value, radix: 16, uppercase: true)) (\(scalar))")
}
// 👋🏽 = U+1F44B (waving hand) + U+1F3FD (medium skin tone modifier) = 1 Character, 2 scalars

// --- 5. Substring and memory sharing ---

print("\n=== Substring: Shared Memory ===")

let fullString = "Hello, Swift World!"
let commaIndex = fullString.firstIndex(of: ",")!
let sub = fullString[..<commaIndex]  // This is a Substring, not a String

print("Type of sub: \(type(of: sub))")  // Substring
print("Value: \(sub)")

// Substring shares memory with the original String.
// This is efficient but means the ENTIRE original string stays in memory
// as long as the Substring exists.

let ownedString = String(sub)  // Creates an independent copy
print("Type of ownedString: \(type(of: ownedString))")  // String

// --- 6. Multi-scalar emoji equality ---

print("\n=== Emoji Equality Deep Dive ===")

let thumbsUp1 = "👍"
let thumbsUp2 = "👍🏻"
print("👍 == 👍🏻: \(thumbsUp1 == thumbsUp2)")  // false — different grapheme clusters

let combined = "👨‍💻"  // man + ZWJ + computer
print("👨‍💻 character count: \(combined.count)")  // 1
print("👨‍💻 scalar count: \(combined.unicodeScalars.count)")  // 3 (man + ZWJ + computer)

// --- 7. String interpolation and Unicode escapes ---

print("\n=== Unicode Escapes ===")

let pi = "\u{03C0}"       // π
let smiley = "\u{1F600}"  // 😀
let null = "\u{0000}"     // null character

print("Pi: \(pi)")
print("Smiley: \(smiley)")
print("Null character length: \(null.count)")  // 1 — it's a valid Character!

// --- 8. The big surprise: zalgo text ---

print("\n=== Extreme Case: Stacked Combining Marks ===")

// You can stack an absurd number of combining marks on a single base character
let zalgo = "a\u{0300}\u{0301}\u{0302}\u{0303}\u{0304}\u{0305}\u{0306}\u{0307}"
print("Zalgo text: \(zalgo)")
print("Character count: \(zalgo.count)")           // 1! All those marks form ONE grapheme cluster
print("Scalar count: \(zalgo.unicodeScalars.count)") // 9 (1 base + 8 combining)
print("UTF-8 bytes: \(zalgo.utf8.count)")            // many bytes for "one character"

print("\nKey insight: Swift's String is grapheme-cluster based, not byte/codepoint based.")
print("Two strings with completely different byte representations can be ==.")
print("Emoji like 👨‍👩‍👧‍👦 (7 scalars, 25 UTF-8 bytes) counts as 1 Character.")
