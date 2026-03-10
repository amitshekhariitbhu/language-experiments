package main

import "fmt"

func main() {
	fmt.Println("=== Experiment 2: Slice Header vs Underlying Array ===")
	fmt.Println()

	// --- Part A: Slices share underlying arrays ---
	fmt.Println("Part A: Slice aliasing — two slices, one array")
	a := []int{1, 2, 3, 4, 5}
	b := a[1:3] // b is [2, 3], but shares a's backing array
	fmt.Printf("  a = %v\n", a)
	fmt.Printf("  b = %v (a[1:3])\n", b)
	b[0] = 99
	fmt.Printf("  After b[0] = 99:\n")
	fmt.Printf("  a = %v  <- a[1] changed too!\n", a)
	fmt.Printf("  b = %v\n", b)

	fmt.Println()

	// --- Part B: Append capacity surprise ---
	fmt.Println("Part B: Append can silently overwrite elements in the original slice")
	original := []int{1, 2, 3, 4, 5}
	sub := original[0:2] // sub is [1, 2], but cap is 5
	fmt.Printf("  original = %v\n", original)
	fmt.Printf("  sub = %v, len=%d, cap=%d\n", sub, len(sub), cap(sub))

	// Appending to sub does NOT allocate a new array — it overwrites original[2]!
	sub = append(sub, 999)
	fmt.Printf("  After append(sub, 999):\n")
	fmt.Printf("  sub      = %v\n", sub)
	fmt.Printf("  original = %v  <- original[2] was overwritten!\n", original)

	fmt.Println()

	// --- Part C: Full-slice expression to prevent this ---
	fmt.Println("Part C: Three-index slice to limit capacity")
	original2 := []int{1, 2, 3, 4, 5}
	safe := original2[0:2:2] // cap is limited to 2
	fmt.Printf("  safe = %v, len=%d, cap=%d\n", safe, len(safe), cap(safe))
	safe = append(safe, 999) // forces new allocation
	fmt.Printf("  After append(safe, 999):\n")
	fmt.Printf("  safe      = %v\n", safe)
	fmt.Printf("  original2 = %v  <- untouched!\n", original2)

	fmt.Println()

	// --- Part D: Nil slice vs empty slice ---
	fmt.Println("Part D: Nil slice vs empty slice")
	var nilSlice []int
	emptySlice := []int{}
	fmt.Printf("  nilSlice == nil: %v, len=%d, cap=%d\n", nilSlice == nil, len(nilSlice), cap(nilSlice))
	fmt.Printf("  emptySlice == nil: %v, len=%d, cap=%d\n", emptySlice == nil, len(emptySlice), cap(emptySlice))
	fmt.Println("  Both work with len(), cap(), range, and append — but they differ in nil checks and JSON marshaling")
}
