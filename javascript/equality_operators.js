// Equality Operators: == vs === vs Object.is
// JavaScript has three ways to compare values, each with different rules.

console.log("=== Equality Operators Deep Dive ===\n");

// 1. The basic difference
console.log("--- == (loose) vs === (strict) ---");
console.log("1 == '1':", 1 == "1");     // true — coerces string to number
console.log("1 === '1':", 1 === "1");   // false — different types

// 2. NaN — the value that isn't equal to itself
console.log("\n--- NaN: The Identity Crisis ---");
console.log("NaN == NaN:", NaN == NaN);           // false
console.log("NaN === NaN:", NaN === NaN);         // false
console.log("Object.is(NaN, NaN):", Object.is(NaN, NaN)); // true!

// Ways to detect NaN:
const x = NaN;
console.log("x !== x:", x !== x);                          // true — only NaN does this!
console.log("Number.isNaN(x):", Number.isNaN(x));          // true
console.log("isNaN('hello'):", isNaN("hello"));            // true — misleading! (coerces)
console.log("Number.isNaN('hello'):", Number.isNaN("hello")); // false — correct

// 3. null and undefined — the special couple
console.log("\n--- null and undefined ---");
console.log("null == undefined:", null == undefined);     // true (special spec rule)
console.log("null === undefined:", null === undefined);   // false
console.log("null == 0:", null == 0);                     // false
console.log("null == '':", null == "");                    // false
console.log("null == false:", null == false);              // false!
// null only loosely equals undefined and nothing else. Not 0, not "", not false.

console.log("undefined == 0:", undefined == 0);           // false
console.log("undefined == '':", undefined == "");          // false
console.log("undefined == false:", undefined == false);    // false

// 4. Object comparison — reference equality
console.log("\n--- Reference Equality ---");
const a = { x: 1 };
const b = { x: 1 };
const c = a;
console.log("a == b:", a == b);     // false — different objects
console.log("a === b:", a === b);   // false
console.log("a == c:", a == c);     // true — same reference
console.log("a === c:", a === c);   // true

console.log("[] == []:", [] == []);   // false — different array objects
console.log("{} === {}:", {} === {}); // false

// 5. Falsy values — all the things that are == false
console.log("\n--- Falsy Value Comparisons ---");
const falsyValues = [false, 0, -0, 0n, "", null, undefined, NaN];
console.log("All falsy values:", falsyValues.map(v => `${String(v)} (${typeof v})`));
console.log("\nWhich falsy values == false?");
falsyValues.forEach((val) => {
  const label = val === "" ? '""' : String(val);
  console.log(`  ${label.padEnd(12)} == false: ${val == false}`);
});
// null, undefined, and NaN are falsy but NOT == false!
// Only 0, -0, 0n, "", and false itself are == false.

// 6. Tricky coercions with ==
console.log("\n--- Tricky Coercions ---");
console.log("'0' == false:", "0" == false);       // true ("0" -> 0, false -> 0)
console.log("'' == false:", "" == false);          // true ("" -> 0, false -> 0)
console.log("'0' == '':", "0" == "");              // false (both strings, no coercion)
// Non-transitive! A == B and A == C, but B != C

console.log("\n'\\t' == 0:", "\t" == 0);           // true (whitespace -> 0)
console.log("'\\n' == 0:", "\n" == 0);             // true
console.log("' ' == 0:", " " == 0);               // true
// All whitespace-only strings coerce to 0!

console.log("\n[] == 0:", [] == 0);                // true ([] -> "" -> 0)
console.log("[1] == 1:", [1] == 1);                // true ([1] -> "1" -> 1)
console.log("['1'] == 1:", ["1"] == 1);            // true (["1"] -> "1" -> 1)
console.log("[1,2] == '1,2':", [1, 2] == "1,2");   // true ([1,2].toString() === "1,2")

// 7. Object.is — the "really really equal" comparison
console.log("\n--- Object.is: The Strictest ---");
console.log("=== thinks: -0 === +0:", -0 === +0);           // true
console.log("Object.is: -0, +0:", Object.is(-0, +0));       // false (correctly distinguishes)
console.log("=== thinks: NaN === NaN:", NaN === NaN);        // false
console.log("Object.is: NaN, NaN:", Object.is(NaN, NaN));   // true (correctly identifies)

// Summary: Object.is is like === except:
// 1. Object.is(NaN, NaN) is true (=== gives false)
// 2. Object.is(-0, +0) is false (=== gives true)
