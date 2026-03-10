package main

import (
	"fmt"
	"sync"
)

func main() {
	fmt.Println("=== Experiment 5: Map Iteration Surprises ===")
	fmt.Println()

	// --- Part A: Map iteration order is randomized ---
	fmt.Println("Part A: Map iteration order is deliberately randomized")
	m := map[string]int{
		"alpha":   1,
		"bravo":   2,
		"charlie": 3,
		"delta":   4,
		"echo":    5,
	}

	fmt.Println("  Three iterations over the same map:")
	for run := 0; run < 3; run++ {
		fmt.Printf("    Run %d: ", run)
		for k := range m {
			fmt.Printf("%s ", k)
		}
		fmt.Println()
	}
	fmt.Println("  -> Go deliberately randomizes map iteration to prevent code from depending on order.")

	fmt.Println()

	// --- Part B: Deleting keys during iteration is safe ---
	fmt.Println("Part B: Deleting keys during iteration is safe (specified by the language)")
	m2 := map[int]string{1: "a", 2: "b", 3: "c", 4: "d"}
	fmt.Printf("  Before: %v\n", m2)
	for k := range m2 {
		if k%2 == 0 {
			delete(m2, k)
		}
	}
	fmt.Printf("  After deleting even keys: %v\n", m2)

	fmt.Println()

	// --- Part C: Adding keys during iteration — may or may not be seen ---
	fmt.Println("Part C: Adding keys during iteration — undefined whether new keys are visited")
	m3 := map[int]bool{1: true, 2: true}
	seen := []int{}
	for k := range m3 {
		seen = append(seen, k)
		if k == 1 {
			m3[10] = true // add a new key during iteration
			m3[20] = true
		}
	}
	fmt.Printf("  Started with keys 1,2. Added 10,20 during iteration.\n")
	fmt.Printf("  Keys actually seen: %v (new keys may or may not appear)\n", seen)

	fmt.Println()

	// --- Part D: Concurrent map access causes a panic ---
	fmt.Println("Part D: Concurrent map read/write causes a FATAL panic (not just a data race)")
	fmt.Println("  Go's runtime detects concurrent map access and crashes intentionally.")
	fmt.Println("  (We'll demonstrate with recovery to avoid crashing this program)")

	demonstrateConcurrentMapPanic()
}

func demonstrateConcurrentMapPanic() {
	// We use a goroutine to trigger the concurrent map write detection.
	// The runtime will panic with "concurrent map writes" or "concurrent map read and map write".
	// We catch it to keep the program running.

	m := make(map[int]int)
	var wg sync.WaitGroup
	done := make(chan bool)

	// Writer goroutine
	wg.Add(1)
	go func() {
		defer wg.Done()
		defer func() {
			if r := recover(); r != nil {
				// Note: runtime panics from concurrent map access are NOT recoverable
				// with recover(). They are fatal. So this won't actually catch it.
				fmt.Printf("  Recovered: %v\n", r)
			}
		}()
		for {
			select {
			case <-done:
				return
			default:
				m[1] = 1
			}
		}
	}()

	// Instead of actually crashing, we'll just describe it
	close(done)
	wg.Wait()

	fmt.Println("  In a real scenario with true concurrent writes, the runtime would print:")
	fmt.Println("  'fatal error: concurrent map writes' and crash the entire program.")
	fmt.Println("  This is NOT a recoverable panic — recover() cannot catch it.")
	fmt.Println("  Solution: use sync.Map or protect with a sync.RWMutex.")
}
