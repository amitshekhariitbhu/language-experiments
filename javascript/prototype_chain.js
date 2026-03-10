// Prototype Chain Surprises
// JavaScript's prototype-based inheritance has subtle behaviors that
// trip up even experienced developers.

console.log("=== Prototype Chain Surprises ===\n");

// 1. Object.create(null) — an object with NO prototype
console.log("--- Object.create(null): The Bare Object ---");
const bare = Object.create(null);
bare.key = "value";
console.log("bare.key:", bare.key);
console.log("bare.toString:", bare.toString);         // undefined!
console.log("bare instanceof Object:", bare instanceof Object); // false!
// This object has absolutely no prototype chain. No toString, no hasOwnProperty, nothing.
// Useful for dictionaries where you don't want prototype pollution.

try {
  console.log("bare + '':", bare + "");
} catch (e) {
  console.log("bare + '' throws:", e.message);
  // Cannot convert object to primitive — no Symbol.toPrimitive, valueOf, or toString!
}

// 2. Modifying prototypes affects all instances retroactively
console.log("\n--- Retroactive Prototype Modification ---");
function Animal(name) {
  this.name = name;
}
const dog = new Animal("Rex");
console.log("dog.speak before:", dog.speak); // undefined

Animal.prototype.speak = function () {
  return `${this.name} says woof!`;
};
console.log("dog.speak after:", dog.speak()); // "Rex says woof!" — added AFTER creation!

// 3. Property shadowing
console.log("\n--- Property Shadowing ---");
const parent = { color: "red", data: [1, 2, 3] };
const child = Object.create(parent);

console.log("child.color:", child.color);               // "red" (inherited)
console.log("child.hasOwnProperty('color'):", child.hasOwnProperty("color")); // false

child.color = "blue";
console.log("child.color:", child.color);               // "blue" (shadowed)
console.log("parent.color:", parent.color);             // "red" (unchanged)
console.log("child.hasOwnProperty('color'):", child.hasOwnProperty("color")); // true

// BUT: mutating an inherited reference type affects the parent!
child.data.push(4);
console.log("child.data:", child.data);                 // [1,2,3,4]
console.log("parent.data:", parent.data);               // [1,2,3,4] — ALSO modified!
// Reading child.data traverses the prototype chain and returns the SAME array.
// push() mutates it in place. Assignment (child.data = ...) would shadow instead.

// 4. Constructor property can lie
console.log("\n--- Constructor Property Lies ---");
function Foo() {}
function Bar() {}
Bar.prototype = Object.create(Foo.prototype);
// Forgot to fix constructor!

const bar = new Bar();
console.log("bar instanceof Bar:", bar instanceof Bar);             // true
console.log("bar instanceof Foo:", bar instanceof Foo);             // true
console.log("bar.constructor === Bar:", bar.constructor === Bar);   // false!
console.log("bar.constructor === Foo:", bar.constructor === Foo);   // true! (inherited from Foo.prototype)
// The constructor property is just a regular property — it's not magically maintained.

// 5. __proto__ vs prototype
console.log("\n--- __proto__ vs .prototype ---");
function MyClass() {}
const inst = new MyClass();

console.log("inst.__proto__ === MyClass.prototype:", inst.__proto__ === MyClass.prototype); // true
console.log("MyClass.__proto__ === Function.prototype:", MyClass.__proto__ === Function.prototype); // true
console.log("MyClass.prototype.__proto__ === Object.prototype:",
  MyClass.prototype.__proto__ === Object.prototype); // true
// .prototype is what instances get as their __proto__.
// .__proto__ is the actual link in the chain.
// Functions themselves inherit from Function.prototype.

// 6. The prototype chain ends at null
console.log("\n--- The End of the Chain ---");
console.log("Object.prototype.__proto__:", Object.prototype.__proto__); // null
// Object.prototype is the final link. Its __proto__ is null, ending the chain.

// 7. Surprising: arrays are objects with numeric keys
console.log("\n--- Arrays Are Objects ---");
const arr = ["a", "b", "c"];
console.log("typeof arr:", typeof arr);                    // "object"
console.log("arr instanceof Array:", arr instanceof Array); // true
console.log("arr instanceof Object:", arr instanceof Object); // true
console.log("Array.prototype.__proto__ === Object.prototype:",
  Array.prototype.__proto__ === Object.prototype); // true

// You can add arbitrary properties to arrays:
arr.secret = "hidden";
console.log("arr.secret:", arr.secret);         // "hidden"
console.log("arr.length:", arr.length);         // 3 — .length unaffected by non-numeric keys
console.log("Object.keys(arr):", Object.keys(arr)); // ["0", "1", "2", "secret"]
