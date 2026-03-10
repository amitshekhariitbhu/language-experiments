package main

import (
	"fmt"
	"sync"
)

func main() {
	fmt.Println("=== Experiment 1: Goroutine Closure Variable Capture ===")
	fmt.Println()

	// --- Part A: The classic loop variable trap (pre-Go 1.22 behavior) ---
	// In Go 1.22+, loop variables are per-iteration, so the old trap is fixed.
	// But we can still demonstrate the closure capture issue with a shared variable.

	fmt.Println("Part A: Closure captures variable by REFERENCE, not value")
	fmt.Println("Using a shared variable modified before goroutines run:")

	var wg sync.WaitGroup
	x := 0
	var funcs []func()
	for i := 0; i < 5; i++ {
		x = i // x is a single variable, all closures share it
		funcs = append(funcs, func() {
			defer wg.Done()
			fmt.Printf("  closure sees x = %d\n", x)
		})
	}
	// x is now 4 after the loop
	wg.Add(len(funcs))
	for _, f := range funcs {
		go f()
	}
	wg.Wait()
	fmt.Println("  -> All closures print 4 because they capture the variable x, not its value at creation time")

	fmt.Println()

	// --- Part B: The fix — pass the value as a parameter ---
	fmt.Println("Part B: Fix by passing value as parameter")
	x = 0
	var funcs2 []func()
	for i := 0; i < 5; i++ {
		x = i
		val := x // create a new variable each iteration
		funcs2 = append(funcs2, func() {
			defer wg.Done()
			fmt.Printf("  closure sees val = %d\n", val)
		})
	}
	wg.Add(len(funcs2))
	for _, f := range funcs2 {
		go f()
	}
	wg.Wait()
	fmt.Println("  -> Each closure captures its own copy of val")

	fmt.Println()

	// --- Part C: Go 1.22+ loop variable semantics ---
	fmt.Println("Part C: Go 1.22+ loop variables are per-iteration")
	wg.Add(5)
	for i := 0; i < 5; i++ {
		go func() {
			defer wg.Done()
			fmt.Printf("  i = %d\n", i)
		}()
	}
	wg.Wait()
	fmt.Println("  -> In Go 1.22+, each iteration gets its own 'i', so values are distinct (though order is random)")
}
