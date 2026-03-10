// pattern_matching.rs — Exhaustive matching, destructuring, guards, @ bindings
//
// Rust's pattern matching is far more powerful than switch/case in most languages.
// It's exhaustive (must cover all cases), supports deep destructuring, and
// integrates with the type system.

fn main() {
    println!("=== Experiment 4: Pattern Matching ===\n");

    // --- Surprise 1: Exhaustive matching catches missing cases at COMPILE TIME ---
    #[derive(Debug)]
    enum TrafficLight {
        Red,
        Yellow,
        Green,
    }

    let light = TrafficLight::Yellow;
    // If you comment out any arm, the compiler errors — no silent bugs
    let action = match light {
        TrafficLight::Red => "stop",
        TrafficLight::Yellow => "caution",
        TrafficLight::Green => "go",
    };
    println!("4a) {:?} means: {}", light, action);

    // --- Surprise 2: Destructuring nested enums and tuples ---
    #[derive(Debug)]
    enum Shape {
        Circle(f64),
        Rectangle(f64, f64),
        Triangle { base: f64, height: f64 },
    }

    let shapes = vec![
        Shape::Circle(5.0),
        Shape::Rectangle(3.0, 4.0),
        Shape::Triangle { base: 6.0, height: 3.0 },
    ];

    for shape in &shapes {
        let area = match shape {
            Shape::Circle(r) => std::f64::consts::PI * r * r,
            Shape::Rectangle(w, h) => w * h,
            Shape::Triangle { base, height } => 0.5 * base * height,
        };
        println!("4b) {:?} area = {:.2}", shape, area);
    }
    println!();

    // --- Surprise 3: Match guards add extra conditions ---
    let number = 42;
    let description = match number {
        n if n < 0 => "negative",
        0 => "zero",
        n if n % 2 == 0 => "positive even",
        _ => "positive odd",
    };
    println!("4c) {} is {}", number, description);

    // --- Surprise 4: @ bindings — bind a value while also testing a pattern ---
    let age = 25;
    let category = match age {
        n @ 0..=12 => format!("child (age {})", n),
        n @ 13..=19 => format!("teenager (age {})", n),
        n @ 20..=64 => format!("adult (age {})", n),
        n @ 65.. => format!("senior (age {})", n),
        // This is exhaustive for non-negative values since we start from 0
        // and all patterns above already cover the range we care about.
        _ => unreachable!(),
    };
    println!("4d) Age {} → {} — @ binds the value AND matches the range\n", age, category);

    // --- Surprise 5: Or-patterns and multiple matches ---
    let code = 404;
    let meaning = match code {
        200 | 201 | 202 => "success",
        301 | 302 => "redirect",
        400 | 404 => "client error",
        500..=599 => "server error",
        _ => "unknown",
    };
    println!("4e) HTTP {} = {}", code, meaning);

    // --- Surprise 6: Destructuring references requires careful & matching ---
    let values = vec![1, 2, 3, 4, 5];
    let count = values.iter().filter(|&&x| x > 2).count();
    // Why &&x? .iter() gives &i32, .filter() gives &&i32, so we destructure twice
    println!("4f) Values > 2: {} — note the &&x destructure in filter\n", count);

    // --- Surprise 7: if let and while let — lightweight pattern matching ---
    let config: Option<(String, u16)> = Some((String::from("localhost"), 8080));
    if let Some((host, port)) = &config {
        println!("4g) if let destructured: {}:{}", host, port);
    }
    // config is still available because we borrowed with &config
    println!("4h) config still available: {:?}", config);

    // while let — iterate until pattern fails
    let mut stack = vec![1, 2, 3];
    print!("4i) Popping stack: ");
    while let Some(top) = stack.pop() {
        print!("{} ", top);
    }
    println!("— while let stops when pop() returns None\n");

    // --- Surprise 8: Matching on references vs values ---
    let reference = &42;
    // Two equivalent ways to match:
    match reference {
        &val => println!("4j) Destructured &: val = {}", val),
    }
    match *reference {
        val => println!("4k) Dereferenced first: val = {}", val),
    }

    // --- Surprise 9: Irrefutable vs refutable patterns ---
    // `let` requires irrefutable patterns (must always match)
    let (a, b, c) = (1, 2, 3);  // Always matches — irrefutable
    println!("4l) Irrefutable let destructure: a={}, b={}, c={}", a, b, c);
    // let Some(x) = some_option;  // COMPILE ERROR: refutable pattern in let
    // Use `if let` or `let ... else` for refutable patterns:
    let some_val: Option<i32> = Some(99);
    let Some(x) = some_val else { panic!("was None") };
    println!("4m) let-else pattern: x = {} — Rust 1.65+ feature", x);
}
