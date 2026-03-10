package main

import (
	"fmt"
	"unsafe"
)

func main() {
	fmt.Println("=== Experiment 8: Struct Comparison and Empty Struct ===")
	fmt.Println()

	// --- Part A: Structs with only comparable fields can be compared with == ---
	fmt.Println("Part A: Comparable structs — all fields must be comparable")
	type Point struct {
		X, Y int
	}
	p1 := Point{1, 2}
	p2 := Point{1, 2}
	p3 := Point{3, 4}
	fmt.Printf("  p1 == p2: %v (same values)\n", p1 == p2)
	fmt.Printf("  p1 == p3: %v (different values)\n", p1 == p3)

	fmt.Println()

	// --- Part B: Structs with slices/maps/functions are NOT comparable ---
	fmt.Println("Part B: Non-comparable structs (contain slice, map, or func)")
	type Config struct {
		Name string
		Tags []string // slices are NOT comparable
	}
	c1 := Config{Name: "test", Tags: []string{"a"}}
	c2 := Config{Name: "test", Tags: []string{"a"}}
	_ = c1
	_ = c2
	// c1 == c2 would be a COMPILE ERROR: invalid operation: cannot compare c1 == c2
	fmt.Println("  Config{} == Config{} would be a COMPILE ERROR because Tags is a slice.")
	fmt.Println("  Slices, maps, and functions cannot be compared with ==.")

	fmt.Println()

	// --- Part C: Structs can be used as map keys only if comparable ---
	fmt.Println("Part C: Comparable structs as map keys")
	type Coord struct {
		X, Y int
	}
	grid := map[Coord]string{
		{0, 0}: "origin",
		{1, 0}: "east",
		{0, 1}: "north",
	}
	fmt.Printf("  grid[{0,0}] = %q\n", grid[Coord{0, 0}])
	fmt.Printf("  grid[{1,0}] = %q\n", grid[Coord{1, 0}])
	fmt.Println("  Structs with slices CANNOT be map keys — compile error.")

	fmt.Println()

	// --- Part D: Empty struct has zero size ---
	fmt.Println("Part D: Empty struct{} takes zero bytes")
	type Empty struct{}
	fmt.Printf("  unsafe.Sizeof(struct{}{}): %d bytes\n", unsafe.Sizeof(Empty{}))
	fmt.Printf("  unsafe.Sizeof(int(0)):    %d bytes\n", unsafe.Sizeof(int(0)))

	// All empty struct values have the same address!
	e1 := struct{}{}
	e2 := struct{}{}
	fmt.Printf("  &e1 == &e2: %v (compiler may give them the same address)\n", &e1 == &e2)

	fmt.Println()

	// --- Part E: Empty struct for sets and signaling ---
	fmt.Println("Part E: Common uses of empty struct")

	// As a set (map[T]struct{} instead of map[T]bool to save memory)
	set := map[string]struct{}{
		"apple":  {},
		"banana": {},
	}
	if _, ok := set["apple"]; ok {
		fmt.Println("  Set contains 'apple' (using map[string]struct{})")
	}
	fmt.Printf("  Memory: map[string]struct{} values cost 0 bytes each\n")
	fmt.Printf("  vs map[string]bool where each value costs 1 byte\n")

	// As a channel signal (chan struct{} for pure signaling)
	done := make(chan struct{})
	go func() {
		fmt.Println("  Worker: doing work...")
		close(done) // signal completion with zero allocation
	}()
	<-done
	fmt.Println("  chan struct{} is idiomatic for signaling — zero memory per message")

	fmt.Println()

	// --- Part F: Struct field order affects size due to padding ---
	fmt.Println("Part F: Struct padding — field order changes size!")
	type Wasteful struct {
		A bool  // 1 byte + 7 bytes padding
		B int64 // 8 bytes
		C bool  // 1 byte + 7 bytes padding
	}
	type Compact struct {
		B int64 // 8 bytes
		A bool  // 1 byte
		C bool  // 1 byte + 6 bytes padding
	}
	fmt.Printf("  Wasteful (bool, int64, bool): %d bytes\n", unsafe.Sizeof(Wasteful{}))
	fmt.Printf("  Compact  (int64, bool, bool): %d bytes\n", unsafe.Sizeof(Compact{}))
	fmt.Println("  Same fields, different order, different size due to alignment padding!")
}
