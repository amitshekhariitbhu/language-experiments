package main

import "fmt"

// A simple interface
type Animal interface {
	Speak() string
}

// A concrete type
type Dog struct {
	Name string
}

func (d *Dog) Speak() string {
	return "Woof"
}

// This function returns an interface, but the underlying value is a nil *Dog
func findDog(found bool) Animal {
	var d *Dog // nil pointer
	if found {
		d = &Dog{Name: "Rex"}
	}
	// BUG: returning a nil *Dog wrapped in an interface
	// The interface is NOT nil — it has type info (*Dog) but a nil value
	return d
}

// The correct way
func findDogCorrectly(found bool) Animal {
	if found {
		return &Dog{Name: "Rex"}
	}
	return nil // return bare nil, not a typed nil
}

func main() {
	fmt.Println("=== Experiment 3: The Famous nil Interface Gotcha ===")
	fmt.Println()

	// --- Part A: Interface nil vs typed nil ---
	fmt.Println("Part A: An interface holding a nil pointer is NOT nil")
	result := findDog(false)
	fmt.Printf("  result = %v\n", result)
	fmt.Printf("  result == nil: %v  <- SURPRISE! This is false.\n", result == nil)
	fmt.Printf("  Type: %T\n", result)
	fmt.Println("  The interface has type (*main.Dog) with a nil value inside.")
	fmt.Println("  An interface is only nil when BOTH type and value are nil.")

	fmt.Println()

	// --- Part B: The correct way ---
	fmt.Println("Part B: Returning bare nil makes the interface truly nil")
	result2 := findDogCorrectly(false)
	fmt.Printf("  result2 = %v\n", result2)
	fmt.Printf("  result2 == nil: %v  <- Correct!\n", result2 == nil)

	fmt.Println()

	// --- Part C: Two nils that aren't equal ---
	fmt.Println("Part C: Two 'nil' values of different types aren't equal")
	var d *Dog   // nil *Dog
	var a Animal // nil Animal (truly nil interface)
	fmt.Printf("  d == nil: %v\n", d == nil)
	fmt.Printf("  a == nil: %v\n", a == nil)
	a = d // assign nil *Dog to the interface
	fmt.Printf("  After a = d (nil *Dog assigned to Animal interface):\n")
	fmt.Printf("  a == nil: %v  <- Now false! The interface holds a type.\n", a == nil)
	fmt.Printf("  d == nil: %v  <- Still true, d is a plain nil pointer.\n", d == nil)

	fmt.Println()

	// --- Part D: error interface is the most common victim ---
	fmt.Println("Part D: The error interface trap (most common real-world bug)")
	fmt.Println("  This is why you should never do: var err *MyError; return err")
	fmt.Println("  Instead do: return nil")
}
