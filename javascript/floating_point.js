// Floating Point Madness
// JavaScript uses IEEE 754 double-precision floating point for all numbers,
// leading to some notorious precision issues.

console.log("=== Floating Point Madness ===\n");

// 1. The classic
console.log("--- The Classic ---");
console.log("0.1 + 0.2 =", 0.1 + 0.2);
console.log("0.1 + 0.2 === 0.3:", 0.1 + 0.2 === 0.3); // false!
console.log("Difference:", 0.1 + 0.2 - 0.3);            // ~5.5e-17

// Fix using Number.EPSILON
const nearlyEqual = (a, b) => Math.abs(a - b) < Number.EPSILON;
console.log("Using EPSILON:", nearlyEqual(0.1 + 0.2, 0.3)); // true

// 2. Not all additions are broken
console.log("\n--- Some Work, Some Don't ---");
console.log("0.1 + 0.2 = 0.3?", 0.1 + 0.2 === 0.3);       // false
console.log("0.1 + 0.7 = 0.8?", 0.1 + 0.7 === 0.8);       // false (try it!)
console.log("0.2 + 0.4 = 0.6?", 0.2 + 0.4 === 0.6);       // false
console.log("0.5 + 0.25 = 0.75?", 0.5 + 0.25 === 0.75);   // true! (exact in binary)
console.log("0.1 + 0.4 = 0.5?", 0.1 + 0.4 === 0.5);       // true (errors cancel out)

// 3. Large number precision loss
console.log("\n--- Large Number Precision ---");
console.log("Number.MAX_SAFE_INTEGER:", Number.MAX_SAFE_INTEGER); // 9007199254740991
console.log("MAX_SAFE + 1:", Number.MAX_SAFE_INTEGER + 1);        // 9007199254740992
console.log("MAX_SAFE + 2:", Number.MAX_SAFE_INTEGER + 2);        // 9007199254740992 — SAME!
console.log("MAX_SAFE + 1 === MAX_SAFE + 2:",
  Number.MAX_SAFE_INTEGER + 1 === Number.MAX_SAFE_INTEGER + 2); // true!
// Beyond MAX_SAFE_INTEGER, JavaScript can only represent even numbers, then multiples of 4, etc.

console.log("\n9999999999999999 becomes:", 9999999999999999);
// Rounded to 10000000000000000! The literal itself gets rounded during parsing.

// 4. Special numeric values
console.log("\n--- Special Values ---");
console.log("1/0 =", 1 / 0);               // Infinity
console.log("-1/0 =", -1 / 0);             // -Infinity
console.log("0/0 =", 0 / 0);               // NaN
console.log("Infinity - Infinity =", Infinity - Infinity); // NaN

console.log("\nNaN === NaN:", NaN === NaN);   // false — NaN is not equal to itself!
console.log("NaN !== NaN:", NaN !== NaN);     // true
console.log("Number.isNaN(NaN):", Number.isNaN(NaN));         // true
console.log("isNaN('hello'):", isNaN("hello"));                // true — coerces to number first!
console.log("Number.isNaN('hello'):", Number.isNaN("hello")); // false — no coercion

// 5. Negative zero
console.log("\n--- Negative Zero ---");
const negZero = -0;
console.log("-0 === 0:", negZero === 0);                    // true — they're "equal"!
console.log("-0 === +0:", -0 === +0);                        // true
console.log("String(-0):", String(-0));                      // "0" — the sign is hidden!
console.log("JSON.stringify(-0):", JSON.stringify(-0));      // "0"
console.log("Object.is(-0, 0):", Object.is(-0, 0));          // false — Object.is knows!
console.log("Object.is(-0, -0):", Object.is(-0, -0));        // true
console.log("1/-0:", 1 / -0);                                // -Infinity (reveals the sign)
console.log("1/+0:", 1 / +0);                                // Infinity
console.log("Math.sign(-0):", Math.sign(-0));                // -0 (not -1!)

// 6. BigInt — the escape hatch
console.log("\n--- BigInt ---");
const big1 = 9007199254740991n; // MAX_SAFE_INTEGER as BigInt
const big2 = big1 + 1n;
const big3 = big1 + 2n;
console.log("BigInt MAX_SAFE + 1:", big2.toString());
console.log("BigInt MAX_SAFE + 2:", big3.toString());
console.log("big2 === big3:", big2 === big3); // false — BigInt handles this correctly!

// But BigInt and Number don't mix:
try {
  console.log(1n + 1);
} catch (e) {
  console.log("1n + 1 throws:", e.message);
  // Cannot mix BigInt and other types
}
console.log("1n == 1:", 1n == 1);   // true (loose equality works!)
console.log("1n === 1:", 1n === 1); // false (different types)
