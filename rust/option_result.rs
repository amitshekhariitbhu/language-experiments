// option_result.rs — No null, Option/Result combinators, ? operator, unwrap dangers
//
// Rust has no null. Instead, it uses Option<T> for "maybe a value" and Result<T, E>
// for "success or error." The type system forces you to handle both cases.

fn main() {
    println!("=== Experiment 8: Option, Result & Error Handling ===\n");

    // --- Surprise 1: There is no null in Rust ---
    // let x: i32 = null;  // COMPILE ERROR: `null` doesn't exist
    // Instead, use Option:
    let present: Option<i32> = Some(42);
    let absent: Option<i32> = None;
    println!("8a) present = {:?}, absent = {:?}", present, absent);
    println!("    Option<i32> size: {} bytes (i32 = 4, + 1 for discriminant, + alignment)",
             std::mem::size_of::<Option<i32>>());

    // --- Surprise 2: Niche optimization — Option<&T> is the SAME size as &T ---
    println!("\n8b) Size optimization:");
    println!("    &i32 size:          {} bytes", std::mem::size_of::<&i32>());
    println!("    Option<&i32> size:  {} bytes — SAME! Null pointer used for None",
             std::mem::size_of::<Option<&i32>>());
    println!("    Box<i32> size:        {} bytes", std::mem::size_of::<Box<i32>>());
    println!("    Option<Box<i32>> size: {} bytes — also optimized!",
             std::mem::size_of::<Option<Box<i32>>>());
    // The compiler knows references/Box can never be null, so it uses 0x0 for None

    // --- Surprise 3: Pattern matching forces you to handle both cases ---
    let maybe_name: Option<&str> = Some("Alice");
    // Can't just use maybe_name as a string — must unwrap somehow:
    match maybe_name {
        Some(name) => println!("\n8c) Hello, {}!", name),
        None => println!("\n8c) No name provided"),
    }

    // --- Surprise 4: Combinators — functional chaining without unwrapping ---
    let numbers = vec!["1", "2", "abc", "4"];
    let parsed: Vec<Option<i32>> = numbers.iter()
        .map(|s| s.parse::<i32>().ok())  // .ok() converts Result to Option
        .collect();
    println!("8d) Parsed: {:?} — errors become None", parsed);

    // map: transform the inner value
    let length = Some("hello").map(|s| s.len());
    println!("8e) Some(\"hello\").map(len) = {:?}", length);

    // and_then (flatmap): chain operations that return Option
    let result = Some("42")
        .and_then(|s| s.parse::<i32>().ok())
        .and_then(|n| if n > 0 { Some(n * 2) } else { None });
    println!("8f) Chained: Some(\"42\") → parse → double = {:?}", result);

    // unwrap_or: provide a default
    let val = None::<i32>.unwrap_or(0);
    println!("8g) None.unwrap_or(0) = {}", val);

    // unwrap_or_else: lazy default (only computed if None)
    let val = None::<i32>.unwrap_or_else(|| expensive_default());
    println!("8h) None.unwrap_or_else(|| ...) = {}\n", val);

    // --- Surprise 5: The ? operator — early return on None/Err ---
    println!("8i) Using ? operator:");
    match find_and_double("42") {
        Some(v) => println!("     find_and_double(\"42\") = {}", v),
        None => println!("     find_and_double(\"42\") = None"),
    }
    match find_and_double("abc") {
        Some(v) => println!("     find_and_double(\"abc\") = {}", v),
        None => println!("     find_and_double(\"abc\") = None (? short-circuited)"),
    }

    // --- Surprise 6: Result — like Option but carries error info ---
    println!("\n8j) Result examples:");
    match divide(10.0, 3.0) {
        Ok(val) => println!("     10 / 3 = {:.4}", val),
        Err(e) => println!("     Error: {}", e),
    }
    match divide(10.0, 0.0) {
        Ok(val) => println!("     10 / 0 = {:.4}", val),
        Err(e) => println!("     Error: {}", e),
    }

    // --- Surprise 7: ? works on Result too, converting error types ---
    match parse_and_divide("10", "3") {
        Ok(val) => println!("     parse_and_divide(\"10\", \"3\") = {:.4}", val),
        Err(e) => println!("     Error: {}", e),
    }
    match parse_and_divide("10", "abc") {
        Ok(val) => println!("     parse_and_divide(\"10\", \"abc\") = {:.4}", val),
        Err(e) => println!("     Error: {}", e),
    }

    // --- Surprise 8: unwrap() panics — a deliberate crash ---
    println!("\n8k) unwrap dangers:");
    println!("     Some(42).unwrap() = {}", Some(42).unwrap());
    // None::<i32>.unwrap();  // PANIC: "called `Option::unwrap()` on a `None` value"
    println!("     None.unwrap() would PANIC — use expect() for better messages");

    // expect() is like unwrap() but with a custom panic message
    let config = Some("production");
    let env = config.expect("CONFIG must be set");
    println!("     config.expect(...) = \"{}\"", env);

    // --- Surprise 9: Collecting Results — all-or-nothing ---
    let strings = vec!["1", "2", "3"];
    let nums: Result<Vec<i32>, _> = strings.iter().map(|s| s.parse::<i32>()).collect();
    println!("\n8l) Collecting Ok results: {:?}", nums);

    let mixed = vec!["1", "abc", "3"];
    let nums: Result<Vec<i32>, _> = mixed.iter().map(|s| s.parse::<i32>()).collect();
    println!("8m) Collecting with error: {:?} — one error fails the whole thing", nums);

    // --- Surprise 10: Option methods for working with references ---
    let mut opt = Some(String::from("hello"));
    // as_ref() converts &Option<T> to Option<&T>
    if let Some(s) = opt.as_ref() {
        println!("\n8n) as_ref(): \"{}\" (borrowed, opt still owns it)", s);
    }
    // as_mut() converts &mut Option<T> to Option<&mut T>
    if let Some(s) = opt.as_mut() {
        s.push_str(" world");
    }
    println!("8o) After as_mut() modification: {:?}", opt);

    // take() extracts the value, leaving None behind
    let taken = opt.take();
    println!("8p) take(): taken = {:?}, opt = {:?}", taken, opt);
}

fn expensive_default() -> i32 {
    99  // Imagine this is a costly computation
}

fn find_and_double(s: &str) -> Option<i32> {
    let n = s.parse::<i32>().ok()?;  // ? returns None if parse fails
    Some(n * 2)
}

fn divide(a: f64, b: f64) -> Result<f64, String> {
    if b == 0.0 {
        Err(String::from("division by zero"))
    } else {
        Ok(a / b)
    }
}

fn parse_and_divide(a: &str, b: &str) -> Result<f64, String> {
    let x: f64 = a.parse().map_err(|e: std::num::ParseFloatError| e.to_string())?;
    let y: f64 = b.parse().map_err(|e: std::num::ParseFloatError| e.to_string())?;
    divide(x, y)
}
