// borrow_checker.rs — Mutable vs immutable borrows, Non-Lexical Lifetimes (NLL)
//
// The borrow checker enforces: either ONE mutable reference OR any number of immutable
// references, but never both simultaneously. NLL made this analysis smarter by tracking
// where references are actually *used*, not just where they're *declared*.

fn main() {
    println!("=== Experiment 2: Borrow Checker & NLL ===\n");

    // --- Surprise 1: Multiple immutable borrows are fine ---
    let data = vec![1, 2, 3];
    let r1 = &data;
    let r2 = &data;
    println!("2a) r1 = {:?}, r2 = {:?} — multiple immutable borrows OK", r1, r2);

    // --- Surprise 2: NLL allows this — the immutable borrow's "lifetime" ends at last use ---
    let mut data = vec![1, 2, 3];
    let r1 = &data;
    println!("2b) Immutable borrow: {:?}", r1);
    // r1 is never used again, so NLL considers it "dead" here
    data.push(4);  // Mutable borrow is fine! Before NLL (Rust 2015), this was an error.
    println!("2c) After push: {:?} — NLL lets us mutate because r1 is no longer used\n", data);

    // --- Surprise 3: Reborrowing — you can pass &mut through functions without moving it ---
    let mut value = String::from("hello");
    modify(&mut value);  // This "reborrows" — doesn't consume the &mut
    modify(&mut value);  // Can do it again!
    println!("2d) After two modifications: \"{}\"", value);

    // --- Surprise 4: You can shadow a mutable binding with an immutable borrow ---
    let mut x = 5;
    x += 1;
    let x = &x;  // x is now an immutable reference to the value 6
    // x += 1;    // COMPILE ERROR: x is no longer mutable
    println!("2e) Shadowed mutable with immutable ref: {}\n", x);

    // --- Surprise 5: Borrowing individual fields of a struct is fine ---
    struct Point { x: i32, y: i32 }
    let mut p = Point { x: 1, y: 2 };
    let px = &mut p.x;
    let py = &mut p.y;  // This is OK! Different fields can be mutably borrowed simultaneously
    *px = 10;
    *py = 20;
    println!("2f) Borrowed different struct fields mutably: ({}, {})", p.x, p.y);

    // --- Surprise 6: But Vec indexing can't do the same ---
    let mut v = vec![1, 2, 3];
    // let a = &mut v[0];
    // let b = &mut v[1];  // COMPILE ERROR: can't prove non-overlapping at compile time
    // The compiler sees both as &mut v[_] — it can't verify distinct indices.
    // Use split_at_mut instead:
    let (left, right) = v.split_at_mut(1);
    left[0] = 10;
    right[0] = 20;
    println!("2g) split_at_mut trick: {:?} — needed for multiple mutable vec refs\n", v);

    // --- Surprise 7: Temporary borrows in match arms ---
    let mut opt = Some(String::from("data"));
    // This works because the borrow from matching is short-lived:
    match &opt {
        Some(s) => println!("2h) Peeked at option: \"{}\"", s),
        None => println!("2h) None"),
    }
    // opt is not consumed — we borrowed with &opt
    opt = None;  // Can still mutate
    println!("2i) Changed opt to: {:?}", opt);

    // --- Surprise 8: The "cannot borrow as mutable because also borrowed as immutable" dance ---
    let mut scores = vec![1, 2, 3, 4, 5];
    // This pattern is common and works thanks to NLL:
    let last = scores.last().copied();  // .copied() converts Option<&i32> to Option<i32>
    if let Some(val) = last {
        scores.push(val * 2);  // OK because `last` doesn't hold a reference anymore
    }
    println!("2j) Scores after conditional push: {:?}", scores);
    println!("     .copied() trick: extracts value, releases borrow");
}

fn modify(s: &mut String) {
    s.push_str("!");
}
