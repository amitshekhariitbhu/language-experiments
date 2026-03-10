// Generators and Iterators
// Generators are functions that can pause execution and resume later,
// enabling lazy evaluation and infinite sequences.

console.log("=== Generators and Iterators ===\n");

// 1. Basic generator — yield pauses execution
console.log("--- Basic Generator ---");
function* countUp() {
  console.log("  Before first yield");
  yield 1;
  console.log("  Between yields");
  yield 2;
  console.log("  After last yield");
  return 3; // return value is NOT included in for...of
}

const gen = countUp();
console.log("Generator created, nothing logged yet!");
console.log("First .next():", gen.next());   // { value: 1, done: false }
console.log("Second .next():", gen.next());  // { value: 2, done: false }
console.log("Third .next():", gen.next());   // { value: 3, done: true }
console.log("Fourth .next():", gen.next());  // { value: undefined, done: true }

// 2. The return value is invisible to for...of
console.log("\n--- Return Value Is Invisible ---");
console.log("for...of only sees yielded values:");
for (const val of countUp()) {
  process.stdout.write(`  ${val} `);
}
console.log(""); // The 3 (return value) is never seen!

// 3. Infinite sequences — lazy evaluation
console.log("\n--- Infinite Fibonacci Sequence ---");
function* fibonacci() {
  let [a, b] = [0, 1];
  while (true) {
    yield a;
    [a, b] = [b, a + b];
  }
}

// Take first 10 Fibonacci numbers without computing an infinite list
const fib = fibonacci();
const first10 = [];
for (let i = 0; i < 10; i++) {
  first10.push(fib.next().value);
}
console.log("First 10 Fibonacci:", first10);

// 4. Two-way communication — yield can RECEIVE values
console.log("\n--- Two-Way Communication ---");
function* conversation() {
  const name = yield "What is your name?";
  const age = yield `Hello ${name}! How old are you?`;
  return `${name} is ${age} years old.`;
}

const chat = conversation();
console.log("Q:", chat.next().value);               // "What is your name?"
console.log("Q:", chat.next("Alice").value);         // "Hello Alice! How old are you?"
console.log("A:", chat.next(30).value);              // "Alice is 30 years old."
// The argument to .next() becomes the value that `yield` evaluates to!
// The FIRST .next() call's argument is always discarded.

// 5. Generator as state machine
console.log("\n--- Generator State Machine ---");
function* trafficLight() {
  while (true) {
    yield "GREEN";
    yield "YELLOW";
    yield "RED";
  }
}
const light = trafficLight();
const sequence = Array.from({ length: 7 }, () => light.next().value);
console.log("Traffic light:", sequence);

// 6. yield* — delegating to another generator
console.log("\n--- yield* Delegation ---");
function* innerGen() {
  yield "inner-1";
  yield "inner-2";
  return "inner-return"; // captured by yield*
}

function* outerGen() {
  yield "outer-1";
  const innerResult = yield* innerGen(); // delegates to innerGen
  console.log("  yield* captured return:", innerResult);
  yield "outer-3";
}

console.log("Delegated sequence:", [...outerGen()]);
// ["outer-1", "inner-1", "inner-2", "outer-3"]
// Note: "inner-return" is NOT yielded, but IS available as the result of yield*

// 7. Generators are iterable — spread, destructuring, etc.
console.log("\n--- Generators Are Iterable ---");
function* range(start, end) {
  for (let i = start; i < end; i++) {
    yield i;
  }
}

const [a, b, ...rest] = range(1, 8);
console.log("Destructured: a =", a, "b =", b, "rest =", rest);

// 8. Early termination with .return()
console.log("\n--- Early Termination ---");
function* resourceGen() {
  try {
    yield 1;
    yield 2;
    yield 3;
  } finally {
    console.log("  Cleanup! (finally block runs even on .return())");
  }
}

const rg = resourceGen();
console.log("next:", rg.next());           // { value: 1, done: false }
console.log("return:", rg.return("done")); // triggers finally, { value: "done", done: true }
console.log("next after return:", rg.next()); // { value: undefined, done: true }

// 9. Throwing into generators
console.log("\n--- Throwing Into Generators ---");
function* errorHandler() {
  try {
    const val = yield "waiting...";
    console.log("  Got:", val);
  } catch (e) {
    console.log("  Caught inside generator:", e.message);
    yield "recovered";
  }
}

const eh = errorHandler();
console.log("First:", eh.next().value);                        // "waiting..."
const throwResult = eh.throw(new Error("boom!"));
console.log("After throw:", throwResult.value);                // "recovered"
// The error is thrown AT the yield point inside the generator.
// The catch block handles it and yields "recovered".
