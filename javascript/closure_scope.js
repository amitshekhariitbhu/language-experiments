// Closures, Scope, and Hoisting
// JavaScript's scoping rules have historically been a source of confusion,
// especially the difference between var and let/const.

console.log("=== Closures, Scope, and Hoisting ===\n");

// 1. The classic loop closure bug with `var`
console.log("--- The Classic var Loop Bug ---");
const fnsVar = [];
for (var i = 0; i < 5; i++) {
  fnsVar.push(() => i);
}
console.log("var loop results:", fnsVar.map((f) => f()));
// [5, 5, 5, 5, 5] — all 5! `var` is function-scoped, so all closures share the same `i`.

// 2. Fixed with `let`
const fnsLet = [];
for (let j = 0; j < 5; j++) {
  fnsLet.push(() => j);
}
console.log("let loop results:", fnsLet.map((f) => f()));
// [0, 1, 2, 3, 4] — `let` creates a new binding per iteration.

// 3. Hoisting: var vs let vs function
console.log("\n--- Hoisting ---");

// var is hoisted and initialized to undefined
console.log("hoistedVar before declaration:", hoistedVar); // undefined (not an error!)
var hoistedVar = "hello";
console.log("hoistedVar after declaration:", hoistedVar);  // "hello"

// let/const are hoisted but NOT initialized — "Temporal Dead Zone"
try {
  console.log(hoistedLet); // ReferenceError!
} catch (e) {
  console.log("hoistedLet before declaration:", e.message);
}
let hoistedLet = "world";

// 4. Function declarations are fully hoisted (both name and body)
console.log("\n--- Function Hoisting ---");
console.log("hoistedFn():", hoistedFn()); // Works! Returns "I'm hoisted"
function hoistedFn() {
  return "I'm hoisted";
}

// Function expressions are NOT fully hoisted
try {
  console.log("exprFn():", exprFn());
} catch (e) {
  console.log("exprFn before declaration:", e.message);
}
var exprFn = function () {
  return "I'm a function expression";
};
// exprFn is hoisted as `var`, so it's `undefined`, and calling undefined() throws.

// 5. Closures capture variables by reference, not value
console.log("\n--- Closure By Reference ---");
function makeCounter() {
  let count = 0;
  return {
    increment: () => ++count,
    decrement: () => --count,
    getCount: () => count,
  };
}
const counter = makeCounter();
counter.increment();
counter.increment();
counter.increment();
counter.decrement();
console.log("Counter value:", counter.getCount()); // 2
// Both increment and decrement share the same `count` variable.

// 6. IIFE — the old-school closure trick
console.log("\n--- IIFE Closure Pattern ---");
const fnsIIFE = [];
for (var k = 0; k < 5; k++) {
  fnsIIFE.push(
    (function (captured) {
      return () => captured;
    })(k)
  );
}
console.log("IIFE loop results:", fnsIIFE.map((f) => f()));
// [0, 1, 2, 3, 4] — each IIFE captures a snapshot of k.

// 7. Block scoping surprises
console.log("\n--- Block Scoping ---");
{
  var blockVar = "I escape blocks!";
  let blockLet = "I stay in blocks!";
}
console.log("blockVar outside {}:", blockVar);       // "I escape blocks!"
try {
  // blockLet is not defined here
  eval("blockLet");
} catch (e) {
  console.log("blockLet outside {}:", e.message);     // not defined
}

// 8. Closure over loop variable — a subtle gotcha
console.log("\n--- Stale Closure ---");
function createActions() {
  let actions = [];
  let value = 0;

  actions.push(() => {
    console.log("  Action captured value:", value);
  });

  value = 42; // Change AFTER closure creation

  actions.push(() => {
    console.log("  Action captured value:", value);
  });

  return actions;
}
const actions = createActions();
console.log("Both actions see the FINAL value (42):");
actions[0](); // 42 — not 0! Closures capture the variable, not the value.
actions[1](); // 42
