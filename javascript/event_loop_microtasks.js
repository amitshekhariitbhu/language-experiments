// Event Loop: Microtasks vs Macrotasks
// The ordering of asynchronous operations in JavaScript is governed by
// a priority system that surprises many developers.

console.log("=== Event Loop: Microtasks vs Macrotasks ===\n");

// Experiment 1: Basic ordering
console.log("--- Experiment 1: Basic Ordering ---");
console.log("1. Synchronous: start");

setTimeout(() => console.log("5. setTimeout (macrotask)"), 0);

Promise.resolve().then(() => console.log("3. Promise.then (microtask)"));

queueMicrotask(() => console.log("4. queueMicrotask (microtask)"));

console.log("2. Synchronous: end");
// Microtasks (Promise, queueMicrotask) always run BEFORE macrotasks (setTimeout),
// even if setTimeout has a 0ms delay.

// Experiment 2: Nested microtasks starve macrotasks
setTimeout(() => {
  console.log("\n--- Experiment 2: Microtask Starvation ---");
  console.log("This setTimeout had to wait for ALL microtasks to complete.");

  // Now demonstrate nested microtask priority
  setTimeout(() => console.log("E. Outer setTimeout callback"), 0);

  Promise.resolve()
    .then(() => {
      console.log("A. First .then()");
      return Promise.resolve();
    })
    .then(() => {
      console.log("B. Second .then() — runs before setTimeout above!");
      return Promise.resolve();
    })
    .then(() => {
      console.log("C. Third .then() — still before setTimeout!");
    });

  queueMicrotask(() => {
    console.log("D. queueMicrotask — also before setTimeout!");
  });
}, 0);

// Experiment 3: Promise constructor is synchronous!
setTimeout(() => {
  console.log("\n--- Experiment 3: Promise Constructor is Synchronous ---");

  console.log("Before new Promise");

  const p = new Promise((resolve) => {
    console.log("Inside Promise constructor — this is SYNCHRONOUS!");
    resolve("done");
  });

  console.log("After new Promise, before .then()");

  p.then((val) => {
    console.log(`Inside .then() — this is async. Value: ${val}`);
  });

  console.log("After .then() registration — .then() hasn't fired yet!");
}, 50);

// Experiment 4: process.nextTick vs Promise (Node.js specific)
setTimeout(() => {
  console.log("\n--- Experiment 4: process.nextTick vs Promise ---");

  Promise.resolve().then(() => console.log("2. Promise microtask"));
  process.nextTick(() => console.log("1. process.nextTick — runs FIRST!"));
  // process.nextTick has its own queue that drains before the Promise microtask queue.
  // This is a Node.js-specific behavior.

  Promise.resolve().then(() => console.log("3. Another Promise"));
  process.nextTick(() => console.log("1b. Another nextTick — still before Promises!"));
}, 100);
