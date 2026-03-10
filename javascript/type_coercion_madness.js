// Type Coercion Madness
// JavaScript's implicit type conversion produces some truly baffling results.

console.log("=== Type Coercion Madness ===\n");

// 1. Array + Array = String (empty string!)
console.log("[] + [] =", JSON.stringify([] + []));
// Both arrays are coerced to strings via .toString(), which gives "" + "" = ""

// 2. Array + Object = "[object Object]"
console.log("[] + {} =", JSON.stringify([] + {}));
// "" + "[object Object]" = "[object Object]"

// 3. Object + Array — depends on context!
// In a statement position, {} is parsed as an empty block, so it becomes +[] = 0
// But when used as an expression:
console.log("({} + []) =", JSON.stringify({} + []));
// "[object Object]"

// 4. The classic boolean coercions
console.log('\n--- Boolean Coercions ---');
console.log('"" == false:', "" == false);           // true
console.log('"0" == false:', "0" == false);         // true — "0" -> 0, false -> 0
console.log('"" == "0":', "" == "0");               // false — no coercion needed, both strings
// So == is NOT transitive! "" == false, "0" == false, but "" != "0"

// 5. null and undefined
console.log('\n--- null / undefined ---');
console.log("null == undefined:", null == undefined);   // true (special rule)
console.log("null === undefined:", null === undefined); // false
console.log("null == 0:", null == 0);                   // false (null only == undefined)
console.log("null == '':", null == "");                  // false

// 6. Adding booleans and strings
console.log('\n--- Arithmetic Surprises ---');
console.log("true + true + true =", true + true + true);  // 3
console.log("true - true =", true - true);                  // 0
console.log("'5' - 3 =", "5" - 3);                         // 2 (string coerced to number)
console.log("'5' + 3 =", "5" + 3);                         // "53" (number coerced to string!)

// 7. The infamous
console.log('\n--- The Infamous ---');
console.log("(!![]) =", !![]);                  // true — [] is truthy
console.log("([] == false) =", [] == false);    // true — [] -> "" -> 0, false -> 0
// So [] is truthy, but [] == false. Mind-bending.

console.log("([] == ![]) =", [] == ![]);
// ![] is false (because [] is truthy). Then [] == false, which is true (as shown above).
// An array equals its own negation!

// 8. typeof null
console.log('\n--- typeof Oddities ---');
console.log("typeof null =", typeof null);           // "object" — a famous 25-year-old bug
console.log("typeof NaN =", typeof NaN);             // "number" — Not a Number is a number
console.log("typeof typeof 1 =", typeof typeof 1);   // "string" — typeof always returns a string

// 9. The banana
console.log('\n--- The Banana ---');
console.log("('b' + 'a' + + 'a' + 'a').toLowerCase() =",
  ("b" + "a" + +"a" + "a").toLowerCase());
// + "a" is NaN (unary plus on non-numeric string), so "b"+"a"+"NaN"+"a" = "banana"
