// trait_objects.rs — Static vs dynamic dispatch, dyn Trait, object safety
//
// Rust supports two kinds of polymorphism: static dispatch (generics, monomorphized
// at compile time) and dynamic dispatch (trait objects, resolved at runtime via vtable).
// Not all traits can be used as trait objects — they must be "object safe."

fn main() {
    println!("=== Experiment 5: Trait Objects & Dispatch ===\n");

    // --- Surprise 1: Static dispatch — the compiler generates specialized code ---
    let circle = Circle { radius: 5.0 };
    let rect = Rectangle { width: 3.0, height: 4.0 };

    // These calls are monomorphized: the compiler creates separate functions for each type
    println!("5a) Static dispatch (generics):");
    print_area_static(&circle);
    print_area_static(&rect);

    // --- Surprise 2: Dynamic dispatch — one function, vtable lookup at runtime ---
    println!("\n5b) Dynamic dispatch (dyn Trait):");
    let shapes: Vec<Box<dyn Shape>> = vec![
        Box::new(Circle { radius: 5.0 }),
        Box::new(Rectangle { width: 3.0, height: 4.0 }),
    ];
    for shape in &shapes {
        print_area_dynamic(shape.as_ref());
    }

    // --- Surprise 3: Size difference — generics are zero-cost, trait objects have overhead ---
    println!("\n5c) Size comparison:");
    println!("     Circle size:           {} bytes", std::mem::size_of::<Circle>());
    println!("     &Circle size:          {} bytes (just a pointer)", std::mem::size_of::<&Circle>());
    println!("     &dyn Shape size:       {} bytes (pointer + vtable pointer)", std::mem::size_of::<&dyn Shape>());
    println!("     Box<dyn Shape> size:   {} bytes (same: ptr + vtable)", std::mem::size_of::<Box<dyn Shape>>());
    // A trait object reference is a "fat pointer" — twice the size of a regular reference!

    // --- Surprise 4: Object safety — not all traits can become trait objects ---
    // A trait is NOT object safe if it:
    //   - Has methods that return Self (the concrete type is unknown)
    //   - Has generic methods (can't put in vtable)
    //   - Has static methods without a receiver (no self parameter)

    // This trait is object safe:
    trait Drawable {
        fn draw(&self) -> String;
    }

    // This trait is NOT object safe (returns Self):
    // trait Cloneable {
    //     fn clone(&self) -> Self;  // Can't be used as dyn Cloneable
    // }
    println!("\n5d) Clone trait is NOT object safe — returns Self, unknown at runtime");

    // --- Surprise 5: Workaround for non-object-safe traits ---
    // Use a helper trait that returns Box<dyn ...> instead of Self
    trait CloneBox {
        fn clone_box(&self) -> Box<dyn CloneBox>;
        fn name(&self) -> &str;
    }

    #[derive(Clone)]
    struct Dog { name: String }

    impl CloneBox for Dog {
        fn clone_box(&self) -> Box<dyn CloneBox> {
            Box::new(self.clone())
        }
        fn name(&self) -> &str { &self.name }
    }

    let dog: Box<dyn CloneBox> = Box::new(Dog { name: "Rex".into() });
    let dog2 = dog.clone_box();
    println!("5e) Cloned trait object: original={}, clone={}", dog.name(), dog2.name());

    // --- Surprise 6: Multiple trait bounds work with generics but not trait objects ---
    // fn do_stuff(item: &(dyn Shape + std::fmt::Debug)) { }  // OK: multiple traits with +
    // But you CAN'T have dyn Trait1 + Trait2 if both have methods (only auto traits like Send)
    println!("\n5f) dyn Shape + Send is OK (Send is auto trait)");
    println!("    dyn Shape + Debug requires both to be 'compatible'\n");

    // --- Surprise 7: Trait upcasting is stable since Rust 1.76 ---
    trait Animal: std::fmt::Display {
        fn speak(&self) -> &str;
    }

    struct Cat;
    impl std::fmt::Display for Cat {
        fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
            write!(f, "Cat")
        }
    }
    impl Animal for Cat {
        fn speak(&self) -> &str { "meow" }
    }

    let animal: Box<dyn Animal> = Box::new(Cat);
    println!("5g) Animal says: {}", animal.speak());
    // Trait upcasting: convert dyn Animal to dyn Display
    let displayable: &dyn std::fmt::Display = &*animal;
    println!("5h) Upcasted to Display: {}", displayable);

    // --- Surprise 8: Enum dispatch as an alternative (no heap allocation) ---
    enum ShapeEnum {
        Circle(Circle),
        Rect(Rectangle),
    }

    impl ShapeEnum {
        fn area(&self) -> f64 {
            match self {
                ShapeEnum::Circle(c) => c.area(),
                ShapeEnum::Rect(r) => r.area(),
            }
        }
    }

    let shapes = vec![
        ShapeEnum::Circle(Circle { radius: 1.0 }),
        ShapeEnum::Rect(Rectangle { width: 2.0, height: 3.0 }),
    ];
    println!("\n5i) Enum dispatch (no heap, no vtable):");
    for s in &shapes {
        println!("     area = {:.2}", s.area());
    }
}

trait Shape {
    fn area(&self) -> f64;
    fn name(&self) -> &str;
}

struct Circle { radius: f64 }
struct Rectangle { width: f64, height: f64 }

impl Shape for Circle {
    fn area(&self) -> f64 { std::f64::consts::PI * self.radius * self.radius }
    fn name(&self) -> &str { "Circle" }
}

impl Shape for Rectangle {
    fn area(&self) -> f64 { self.width * self.height }
    fn name(&self) -> &str { "Rectangle" }
}

// Static dispatch: compiler generates a separate version for Circle and Rectangle
fn print_area_static<T: Shape>(shape: &T) {
    println!("     {} area = {:.2}", shape.name(), shape.area());
}

// Dynamic dispatch: single function, vtable lookup at runtime
fn print_area_dynamic(shape: &dyn Shape) {
    println!("     {} area = {:.2}", shape.name(), shape.area());
}
