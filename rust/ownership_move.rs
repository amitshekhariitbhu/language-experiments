// ownership_move.rs — Move semantics, Copy vs Clone, and surprising ownership transfers
//
// Rust's ownership model is its most distinctive feature. Values have exactly one owner,
// and when ownership transfers (a "move"), the original binding becomes invalid.
// But some types implement Copy and are duplicated instead of moved.

fn main() {
    println!("=== Experiment 1: Ownership & Move Semantics ===\n");

    // --- Surprise 1: Integers are Copy, Strings are not ---
    let x = 42;
    let y = x;      // x is COPIED (i32 implements Copy)
    println!("1a) x = {}, y = {} — both valid because i32 is Copy", x, y);

    let s1 = String::from("hello");
    let s2 = s1;    // s1 is MOVED into s2
    // println!("{}", s1);  // COMPILE ERROR: value used after move
    println!("1b) s2 = \"{}\" — s1 is gone after move", s2);

    // --- Surprise 2: Move happens in function calls too ---
    let name = String::from("Rust");
    take_ownership(name);
    // println!("{}", name);  // COMPILE ERROR: name was moved into the function
    println!("1c) 'name' no longer accessible after passing to function\n");

    // --- Surprise 3: Returning from a function transfers ownership back ---
    let returned = give_ownership();
    println!("1d) Got ownership back: \"{}\"", returned);

    // --- Surprise 4: Clone creates a deep copy, both remain valid ---
    let original = String::from("deep copy me");
    let cloned = original.clone();
    println!("1e) original = \"{}\", cloned = \"{}\" — both valid after clone\n", original, cloned);

    // --- Surprise 5: Tuples are Copy ONLY if all elements are Copy ---
    let t1 = (1, 2, 3);       // all i32 (Copy), so tuple is Copy
    let t2 = t1;
    println!("1f) t1 = {:?}, t2 = {:?} — tuple of Copy types is Copy", t1, t2);

    let t3 = (1, String::from("not copy"));  // String is NOT Copy
    let t4 = t3;               // entire tuple is moved
    // println!("{:?}", t3);   // COMPILE ERROR
    println!("1g) t4 = {:?} — tuple with String is moved, not copied\n", t4);

    // --- Surprise 6: Partial moves in structs ---
    #[derive(Debug)]
    struct Person {
        name: String,
        age: u32,
    }

    let person = Person { name: String::from("Alice"), age: 30 };
    let moved_name = person.name;  // partial move: only `name` is moved
    // println!("{:?}", person);    // COMPILE ERROR: can't use person as a whole
    println!("1h) Moved name: \"{}\", but person.age = {} still works!", moved_name, person.age);
    println!("     Partial moves let you use un-moved fields individually.\n");

    // --- Surprise 7: Closures capture by move or borrow ---
    let data = vec![1, 2, 3];
    let closure_borrow = || println!("1i) Borrowed: {:?}", data);
    closure_borrow();
    println!("1j) Still accessible after borrow closure: {:?}", data);

    let data2 = vec![4, 5, 6];
    let closure_move = move || println!("1k) Moved into closure: {:?}", data2);
    closure_move();
    // println!("{:?}", data2);  // COMPILE ERROR: moved into closure
    println!("1l) data2 is gone — 'move' keyword forces ownership transfer to closure");
}

fn take_ownership(s: String) {
    println!("1c) Took ownership of: \"{}\"", s);
    // s is dropped here when it goes out of scope
}

fn give_ownership() -> String {
    let s = String::from("I am returned");
    s  // ownership transfers to the caller
}
