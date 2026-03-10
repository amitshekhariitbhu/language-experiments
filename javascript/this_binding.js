// The Many Faces of `this`
// `this` in JavaScript is not determined by where a function is defined,
// but by HOW it is called — with one major exception: arrow functions.

console.log("=== The Many Faces of `this` ===\n");

// 1. Method call — `this` is the object
const obj = {
  name: "ObjectA",
  greet() {
    return `Hello from ${this.name}`;
  },
};
console.log("--- Method Call ---");
console.log("obj.greet():", obj.greet()); // "Hello from ObjectA"

// 2. Detached method — `this` becomes undefined (strict) or global
const detached = obj.greet;
try {
  console.log("\n--- Detached Method ---");
  console.log("detached():", detached());
  // In non-strict mode: "Hello from undefined" (global has no .name by default)
} catch (e) {
  console.log("detached() threw:", e.message);
}

// 3. Arrow functions capture `this` from the enclosing scope
console.log("\n--- Arrow vs Regular in Object ---");
const obj2 = {
  name: "ObjectB",
  regular() {
    return this.name;
  },
  arrow: () => {
    // `this` here is the module/global scope, NOT obj2
    return typeof this === "object" ? `this is: ${JSON.stringify(this)}` : this;
  },
};
console.log("obj2.regular():", obj2.regular()); // "ObjectB"
console.log("obj2.arrow():", obj2.arrow());     // {} or global — NOT "ObjectB"!

// 4. Arrow functions CANNOT be rebound
console.log("\n--- Arrow Functions Ignore bind/call/apply ---");
const arrowFn = () => this;
console.log("arrowFn.call({x: 1}):", JSON.stringify(arrowFn.call({ x: 1 })));
// Still the outer `this`, not {x: 1}. Arrow functions completely ignore bind/call/apply for `this`.

// 5. `this` in callbacks
console.log("\n--- Callbacks Lose `this` ---");
class Timer {
  constructor() {
    this.seconds = 0;
  }

  startRegular() {
    // Regular function loses `this`
    const self = this;
    const callback = function () {
      return typeof this.seconds; // `this` is not the Timer instance
    };
    const arrowCallback = () => {
      return typeof self.seconds; // captured via closure
    };
    return {
      regularResult: callback(),
      arrowResult: arrowCallback(),
    };
  }
}
const t = new Timer();
const results = t.startRegular();
console.log("Regular callback `this.seconds`:", results.regularResult); // undefined
console.log("Arrow callback `this.seconds`:", results.arrowResult);     // number

// 6. Constructor `this`
console.log("\n--- Constructor vs Regular Call ---");
function Person(name) {
  this.name = name;
  this.whoAmI = () => `I am ${this.name}`;
  // Arrow function here permanently binds `this` to the new instance
}
const alice = new Person("Alice");
const whoFn = alice.whoAmI;
console.log("alice.whoAmI():", alice.whoAmI());  // "I am Alice"
console.log("whoFn():", whoFn());                // "I am Alice" — arrow function remembers!

// Compare with regular function:
function Person2(name) {
  this.name = name;
  this.whoAmI = function () {
    return `I am ${this.name}`;
  };
}
const bob = new Person2("Bob");
const whoFn2 = bob.whoAmI;
console.log("bob.whoAmI():", bob.whoAmI());       // "I am Bob"
console.log("whoFn2():", whoFn2());               // "I am undefined" — lost context!

// 7. bind() creates a permanently bound function
console.log("\n--- Double Bind ---");
function showThis() {
  return this.val;
}
const bound1 = showThis.bind({ val: "first" });
const bound2 = bound1.bind({ val: "second" });
console.log("bound1():", bound1()); // "first"
console.log("bound2():", bound2()); // "first" — bind() only works ONCE! Second bind is ignored.
