// string_types.rs — String vs &str, UTF-8 indexing, char vs byte iteration
//
// Rust strings are always valid UTF-8. This means indexing by position is O(n),
// you can't index with s[0], and a "character" might be 1-4 bytes.
// String and &str are fundamentally different types with different use cases.

fn main() {
    println!("=== Experiment 7: String Types & UTF-8 ===\n");

    // --- Surprise 1: String vs &str — owned vs borrowed ---
    let owned: String = String::from("hello");    // Heap-allocated, growable, owned
    let borrowed: &str = "hello";                  // Points to static memory (or a String)
    let also_borrowed: &str = &owned;              // Borrow from a String

    println!("7a) String size on stack: {} bytes (ptr + len + capacity)", std::mem::size_of::<String>());
    println!("    &str size on stack:   {} bytes (ptr + len, no capacity)", std::mem::size_of::<&str>());
    println!("    owned = \"{}\", borrowed = \"{}\", also = \"{}\"", owned, borrowed, also_borrowed);

    // --- Surprise 2: You CANNOT index a String with s[0] ---
    let hello = String::from("hello");
    // let h = hello[0];  // COMPILE ERROR: String cannot be indexed by integer
    // Reason: indexing should be O(1), but UTF-8 chars are variable-width
    let h = hello.as_bytes()[0];  // This works but gives a BYTE, not a char
    println!("\n7b) hello.as_bytes()[0] = {} (ASCII code for '{}')", h, h as char);
    let h = hello.chars().nth(0).unwrap();
    println!("    hello.chars().nth(0) = '{}' (correct but O(n))", h);

    // --- Surprise 3: Multi-byte characters break naive assumptions ---
    let emoji = "Hello 🦀 Rust!";
    println!("\n7c) \"{}\"", emoji);
    println!("    .len()          = {} bytes (NOT characters!)", emoji.len());
    println!("    .chars().count() = {} characters", emoji.chars().count());
    // The crab emoji 🦀 is 4 bytes in UTF-8!

    // --- Surprise 4: Slicing by byte index can PANIC ---
    let russian = "Здравствуйте";  // Cyrillic: each char is 2 bytes
    println!("\n7d) \"{}\"", russian);
    println!("    .len()          = {} bytes", russian.len());
    println!("    .chars().count() = {} chars", russian.chars().count());

    let first_two_chars = &russian[0..4];  // 2 chars × 2 bytes each = 4 bytes
    println!("    &russian[0..4]   = \"{}\" (must slice at char boundaries!)", first_two_chars);
    // &russian[0..3] would PANIC at runtime: "byte index 3 is not a char boundary"

    // --- Surprise 5: Three ways to view a string ---
    let hindi = "नमस्ते";
    println!("\n7e) \"{}\" has three representations:", hindi);

    print!("    Bytes:  ");
    for b in hindi.bytes() {
        print!("{} ", b);
    }
    println!(" ({} bytes)", hindi.len());

    print!("    Chars:  ");
    for c in hindi.chars() {
        print!("'{}' ", c);
    }
    println!(" ({} chars)", hindi.chars().count());

    // Note: Some "characters" are actually combining characters
    // The visual letter "स्ते" is multiple Unicode scalar values combined

    // --- Surprise 6: String concatenation quirks ---
    let s1 = String::from("Hello");
    let s2 = String::from(" World");

    // Method 1: + operator (takes ownership of s1, borrows s2)
    let s3 = s1 + &s2;  // s1 is MOVED, s2 is borrowed
    // println!("{}", s1);  // COMPILE ERROR: s1 was moved
    println!("\n7f) s1 + &s2 = \"{}\" — s1 was consumed!", s3);

    // Method 2: format! (borrows everything)
    let s4 = String::from("foo");
    let s5 = String::from("bar");
    let s6 = format!("{}{}", s4, s5);  // Neither s4 nor s5 is moved
    println!("7g) format! = \"{}\", s4 still alive: \"{}\"", s6, s4);

    // --- Surprise 7: String is a Vec<u8> under the hood ---
    let mut s = String::from("hello");
    println!("\n7h) String internals:");
    println!("    capacity: {}, len: {}, ptr: {:p}", s.capacity(), s.len(), s.as_ptr());
    s.push_str(" world");
    println!("    After push: capacity: {}, len: {}", s.capacity(), s.len());

    // You can even build a String from raw bytes (if they're valid UTF-8)
    let sparkle = String::from_utf8(vec![240, 159, 146, 150]).unwrap();
    println!("    From raw bytes: {}", sparkle);

    // --- Surprise 8: Cow<str> — avoid allocation when possible ---
    use std::borrow::Cow;

    fn maybe_modify(input: &str, should_modify: bool) -> Cow<str> {
        if should_modify {
            Cow::Owned(input.to_uppercase())   // Allocates only when needed
        } else {
            Cow::Borrowed(input)               // Zero allocation!
        }
    }

    let result1 = maybe_modify("hello", false);
    let result2 = maybe_modify("hello", true);
    println!("\n7i) Cow borrowed: \"{}\" (no allocation)", result1);
    println!("    Cow owned:    \"{}\" (allocated only because we modified)", result2);

    // --- Surprise 9: char is 4 bytes, not 1 ---
    println!("\n7j) Size of char: {} bytes (always 4, holds any Unicode scalar value)", std::mem::size_of::<char>());
    println!("    Size of u8:   {} byte", std::mem::size_of::<u8>());
    println!("    'A' as u32 = {}, '🦀' as u32 = {}", 'A' as u32, '🦀' as u32);
}
