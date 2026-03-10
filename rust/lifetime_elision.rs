// lifetime_elision.rs — When lifetimes are needed, elision rules, and 'static
//
// Lifetimes are Rust's way of ensuring references are always valid. The compiler
// can often infer them (elision), but sometimes you must annotate explicitly.

fn main() {
    println!("=== Experiment 3: Lifetime Elision & 'static ===\n");

    // --- Surprise 1: Lifetime elision makes most functions clean ---
    // The compiler applies 3 rules automatically:
    //   1. Each reference parameter gets its own lifetime
    //   2. If there's exactly one input lifetime, it's assigned to all outputs
    //   3. If one of the inputs is &self or &mut self, its lifetime is used for output
    let s = String::from("hello world");
    let word = first_word(&s);  // No lifetime annotations needed!
    println!("3a) First word: \"{}\" — elision rule #2 handled this", word);

    // --- Surprise 2: When elision fails — multiple input lifetimes ---
    let string1 = String::from("long string");
    let result;
    {
        let string2 = String::from("xyz");
        result = longest(&string1, &string2);
        // We must use `result` while string2 is still alive
        println!("3b) Longest: \"{}\" — needs explicit lifetime annotation", result);
    }
    // println!("{}", result);  // Would fail: string2 might have been the longest

    // --- Surprise 3: 'static lifetime — lives for the entire program ---
    let s: &'static str = "I live forever";
    println!("3c) Static string literal: \"{}\"", s);

    // String literals are ALWAYS &'static str because they're baked into the binary
    let also_static = get_static_str();
    println!("3d) Returned static str: \"{}\"", also_static);

    // --- Surprise 4: 'static doesn't mean "immutable" or "constant" ---
    // It means the value CAN live for the entire program, not that it MUST.
    let owned = String::from("I'm owned");
    let boxed: Box<dyn std::fmt::Display + 'static> = Box::new(owned);
    // owned String satisfies 'static because it OWNS its data (no borrows that could expire)
    println!("3e) Box<dyn Display + 'static>: {} — owned types satisfy 'static", boxed);

    // --- Surprise 5: Lifetime annotations don't change how long values live ---
    // They only describe relationships between reference lifetimes
    let x = 5;
    let r = &x;
    println!("3f) Lifetimes are descriptive, not prescriptive: {}", r);

    // --- Surprise 6: Struct lifetimes — a struct holding a reference ---
    let novel = String::from("Call me Ishmael. Some years ago...");
    let excerpt = ImportantExcerpt { part: first_word(&novel) };
    println!("3g) Struct with lifetime: part = \"{}\"", excerpt.part);
    // excerpt can't outlive novel — the lifetime annotation guarantees this

    // --- Surprise 7: Lifetime bounds on generic types ---
    let s1 = String::from("held");
    let wrapper = Wrapper { value: &s1 };
    println!("3h) Wrapper: {:?}", wrapper);

    // --- Surprise 8: The 'anonymous lifetime '_ ---
    let data = vec![1, 2, 3, 4, 5];
    let iter = make_iter(&data);
    let collected: Vec<&i32> = iter.collect();
    println!("3i) Collected from anonymous lifetime iter: {:?}", collected);
}

// Elision rule #2: one input lifetime → output gets the same
fn first_word(s: &str) -> &str {
    let bytes = s.as_bytes();
    for (i, &byte) in bytes.iter().enumerate() {
        if byte == b' ' {
            return &s[..i];
        }
    }
    s
}

// Elision FAILS here: two input lifetimes, compiler can't guess which output uses
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {
    if x.len() >= y.len() { x } else { y }
}

fn get_static_str() -> &'static str {
    "compiled into the binary"
}

struct ImportantExcerpt<'a> {
    part: &'a str,
}

#[derive(Debug)]
struct Wrapper<'a> {
    value: &'a String,
}

// Using impl Trait with anonymous lifetime
fn make_iter(data: &[i32]) -> impl Iterator<Item = &i32> {
    data.iter()
}
