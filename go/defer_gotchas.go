package main

import "fmt"

func main() {
	fmt.Println("=== Experiment 4: Defer Gotchas ===")
	fmt.Println()

	// --- Part A: Defer arguments are evaluated immediately ---
	fmt.Println("Part A: Defer evaluates arguments at defer time, not execution time")
	x := 0
	defer fmt.Printf("  [deferred] x = %d (captured at defer time)\n", x)
	x = 42
	fmt.Printf("  x is now %d, but the deferred call already captured 0\n", x)

	fmt.Println()

	// --- Part B: Defer with closures captures the variable, not the value ---
	fmt.Println("Part B: Defer with closure captures the variable itself")
	y := 0
	defer func() {
		fmt.Printf("  [deferred closure] y = %d (sees final value)\n", y)
	}()
	y = 100
	fmt.Printf("  y is now %d, and the deferred closure WILL see this value\n", y)

	fmt.Println()

	// --- Part C: Defer in a loop — they all execute at function exit ---
	fmt.Println("Part C: Defer in a loop — all defers stack up and run at function exit")
	fmt.Println("  Calling deferInLoop()...")
	deferInLoop()

	fmt.Println()

	// --- Part D: Named return values + defer ---
	fmt.Println("Part D: Defer can modify named return values")
	result := namedReturn()
	fmt.Printf("  namedReturn() = %d (defer changed it from 1 to 2!)\n", result)

	fmt.Println()

	// --- Part E: Defer execution order (LIFO) ---
	fmt.Println("Part E: Defers execute in LIFO (stack) order")
	deferOrder()
}

func deferInLoop() {
	// This creates 5 deferred calls — all run when function exits, in reverse order
	for i := 0; i < 5; i++ {
		defer fmt.Printf("  deferred: %d\n", i)
	}
	fmt.Println("  (deferred prints will appear after this, in reverse order)")
}

func namedReturn() (result int) {
	// The defer can modify the named return value AFTER the return statement
	defer func() {
		result++ // modifies the actual return value!
	}()
	return 1 // sets result = 1, then defer runs and increments it to 2
}

func deferOrder() {
	defer fmt.Println("  first deferred (runs last)")
	defer fmt.Println("  second deferred (runs second)")
	defer fmt.Println("  third deferred (runs first)")
	fmt.Println("  (regular execution)")
}
