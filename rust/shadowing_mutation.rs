// shadowing_mutation.rs — let shadowing vs mut, type-changing shadow, immutability by default
//
// Rust distinguishes between "rebinding a name" (shadowing) and "changing a value" (mutation).
// Shadowing creates a NEW variable that happens to have the same name.
// This has surprising implications for type changes and scoping.

fn main() {
    println!("=== Experiment 6: Shadowing & Mutation ===\n");

    // --- Surprise 1: Shadowing can change the TYPE of a variable ---
    let x = "42";              // x is &str
    let x = x.parse::<i32>().unwrap();  // x is now i32 — same name, different type!
    let x = x + 8;             // x is still i32
    println!("6a) x started as &str \"42\", now it's i32: {}", x);
    println!("    Shadowing lets you reuse names across type transformations\n");

    // --- Surprise 2: mut vs shadowing — they're fundamentally different ---
    let mut a = 5;
    a = 10;           // Mutation: same memory location, same type, value changed
    // a = "hello";   // COMPILE ERROR: can't change type with mut

    let b = 5;
    let b = "hello";  // Shadowing: new variable, new type, old b is gone
    println!("6b) mut changes value (a={}), shadow changes anything (b=\"{}\")\n", a, b);

    // --- Surprise 3: Shadowing in inner scopes ---
    let x = 5;
    {
        let x = x * 2;  // Inner shadow
        println!("6c) Inner scope x = {} (shadowed)", x);
    }
    println!("6d) Outer scope x = {} (original, unaffected by inner shadow)\n", x);

    // --- Surprise 4: Shadowing can make a mutable variable immutable ---
    let mut value = String::from("mutable");
    value.push_str(" string");
    let value = value;  // Shadow: now immutable!
    // value.push_str("!");  // COMPILE ERROR: value is no longer mutable
    println!("6e) \"{}\" was mutable, shadowed to immutable — freezing pattern", value);

    // And the reverse — making an immutable value mutable:
    let data = vec![1, 2, 3];
    let mut data = data;  // Shadow with mut
    data.push(4);
    println!("6f) Immutable vec shadowed to mutable: {:?}\n", data);

    // --- Surprise 5: Immutability is the DEFAULT — this is unusual among languages ---
    let x = 42;
    // x = 43;  // COMPILE ERROR without `mut`
    println!("6g) Variables are immutable by default: x = {}", x);
    println!("    Most languages default to mutable — Rust makes you opt-in\n");

    // --- Surprise 6: Interior mutability — Cell and RefCell ---
    use std::cell::Cell;
    use std::cell::RefCell;

    // Cell: for Copy types — get/set without &mut
    let cell = Cell::new(42);
    println!("6h) Cell value: {}", cell.get());
    cell.set(99);  // No &mut needed!
    println!("6i) Cell after set: {} — mutated through shared reference!", cell.get());

    // RefCell: for non-Copy types — runtime borrow checking
    let ref_cell = RefCell::new(vec![1, 2, 3]);
    ref_cell.borrow_mut().push(4);  // Runtime borrow check instead of compile-time
    println!("6j) RefCell: {:?} — runtime-checked mutation\n", ref_cell.borrow());

    // --- Surprise 7: Constants vs statics vs let ---
    const MAX_SIZE: usize = 100;        // Inlined at every use site, no memory address
    static COUNTER: i32 = 0;            // Has a fixed memory address, lives for 'static
    let local = 42;                     // Stack allocated, scope-limited

    println!("6k) const MAX_SIZE = {} (inlined, no address)", MAX_SIZE);
    println!("6l) static COUNTER = {} (fixed address: {:p})", COUNTER, &COUNTER);
    println!("6m) let local = {} (stack, temporary)\n", local);

    // --- Surprise 8: Shadowing in function parameters ---
    let result = shadow_param(42);
    println!("6n) shadow_param(42) = \"{}\" — parameter was shadowed inside fn", result);

    // --- Surprise 9: Pattern shadowing in match ---
    let value = Some(42);
    match value {
        Some(value) => println!("6o) Inside match: value = {} (shadows outer 'value')", value),
        None => println!("6o) None"),
    }
    println!("6p) Outside match: value = {:?} (original, a Some variant)", value);
}

fn shadow_param(x: i32) -> String {
    let x = x * 2;            // Shadow the parameter
    let x = format!("doubled: {}", x);  // Shadow again, changing type
    x
}
