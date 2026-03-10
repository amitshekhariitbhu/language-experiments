package main

import (
	"errors"
	"fmt"
)

// Sentinel errors — package-level error values
var (
	ErrNotFound   = errors.New("not found")
	ErrPermission = errors.New("permission denied")
)

// Custom error type
type ValidationError struct {
	Field   string
	Message string
}

func (e *ValidationError) Error() string {
	return fmt.Sprintf("validation error: field %q: %s", e.Field, e.Message)
}

func main() {
	fmt.Println("=== Experiment 6: Error Handling Subtleties ===")
	fmt.Println()

	// --- Part A: errors.Is vs == ---
	fmt.Println("Part A: Wrapped errors — == fails, errors.Is succeeds")
	original := ErrNotFound
	wrapped := fmt.Errorf("database query failed: %w", original)

	fmt.Printf("  wrapped == ErrNotFound: %v  <- false! == doesn't unwrap.\n", wrapped == ErrNotFound)
	fmt.Printf("  errors.Is(wrapped, ErrNotFound): %v  <- true! It unwraps the chain.\n", errors.Is(wrapped, ErrNotFound))

	fmt.Println()

	// --- Part B: Double wrapping ---
	fmt.Println("Part B: errors.Is traverses the entire wrap chain")
	doubleWrapped := fmt.Errorf("service layer: %w", wrapped)
	fmt.Printf("  errors.Is(doubleWrapped, ErrNotFound): %v\n", errors.Is(doubleWrapped, ErrNotFound))
	fmt.Printf("  Error text: %s\n", doubleWrapped)

	fmt.Println()

	// --- Part C: errors.As for type assertions ---
	fmt.Println("Part C: errors.As unwraps to find a specific error type")
	valErr := &ValidationError{Field: "email", Message: "invalid format"}
	wrappedVal := fmt.Errorf("user registration failed: %w", valErr)
	doubleWrappedVal := fmt.Errorf("API error: %w", wrappedVal)

	var target *ValidationError
	if errors.As(doubleWrappedVal, &target) {
		fmt.Printf("  Found ValidationError: field=%q message=%q\n", target.Field, target.Message)
	}
	fmt.Println("  errors.As walks the chain and finds the *ValidationError inside")

	fmt.Println()

	// --- Part D: %w vs %v — only %w allows unwrapping ---
	fmt.Println("Part D: fmt.Errorf with %%v vs %%w")
	wrappedWithW := fmt.Errorf("with %%w: %w", ErrPermission)
	wrappedWithV := fmt.Errorf("with %%v: %v", ErrPermission)
	fmt.Printf("  errors.Is(wrappedWithW, ErrPermission): %v  <- true, %%w preserves the chain\n",
		errors.Is(wrappedWithW, ErrPermission))
	fmt.Printf("  errors.Is(wrappedWithV, ErrPermission): %v  <- false! %%v converts to string, breaks the chain\n",
		errors.Is(wrappedWithV, ErrPermission))

	fmt.Println()

	// --- Part E: Creating sentinel errors that are the same text but different identity ---
	fmt.Println("Part E: Two errors.New with same text are NOT equal")
	e1 := errors.New("something went wrong")
	e2 := errors.New("something went wrong")
	fmt.Printf("  e1 == e2: %v  <- false! Different pointers, same text.\n", e1 == e2)
	fmt.Printf("  e1.Error() == e2.Error(): %v  <- true, the strings match.\n", e1.Error() == e2.Error())
	fmt.Println("  Sentinel errors have identity, not just textual equality.")
}
