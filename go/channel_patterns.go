package main

import (
	"fmt"
	"time"
)

func main() {
	fmt.Println("=== Experiment 7: Channel Patterns and Pitfalls ===")
	fmt.Println()

	// --- Part A: Unbuffered channels require sender and receiver to be ready ---
	fmt.Println("Part A: Unbuffered channel — sender blocks until receiver is ready")
	ch := make(chan string)
	go func() {
		fmt.Println("  Goroutine: about to send...")
		ch <- "hello" // blocks until main goroutine reads
		fmt.Println("  Goroutine: sent!")
	}()
	time.Sleep(50 * time.Millisecond) // let the goroutine block
	msg := <-ch
	fmt.Printf("  Received: %s\n", msg)
	time.Sleep(10 * time.Millisecond) // let "sent!" print

	fmt.Println()

	// --- Part B: Buffered channel doesn't block until buffer is full ---
	fmt.Println("Part B: Buffered channel — send doesn't block until buffer is full")
	bch := make(chan int, 3)
	bch <- 1
	bch <- 2
	bch <- 3
	fmt.Printf("  Sent 3 values to buffer of size 3 without blocking.\n")
	fmt.Printf("  len=%d, cap=%d\n", len(bch), cap(bch))
	// bch <- 4 would deadlock here since no one is reading!
	fmt.Println("  A 4th send would deadlock (no reader).")

	fmt.Println()

	// --- Part C: Reading from a closed channel ---
	fmt.Println("Part C: Reading from a closed channel returns zero values")
	dataCh := make(chan int, 3)
	dataCh <- 10
	dataCh <- 20
	close(dataCh)
	// We can still read the buffered values, then we get zero values
	fmt.Printf("  Read 1: %d\n", <-dataCh)
	fmt.Printf("  Read 2: %d\n", <-dataCh)
	val, ok := <-dataCh
	fmt.Printf("  Read 3: val=%d, ok=%v  <- channel is closed and empty\n", val, ok)
	fmt.Println("  After close: reads return zero-value with ok=false. No panic.")

	fmt.Println()

	// --- Part D: Select with default for non-blocking operations ---
	fmt.Println("Part D: Select with default — non-blocking channel operations")
	emptyCh := make(chan int)
	select {
	case v := <-emptyCh:
		fmt.Printf("  Got %d (this won't happen)\n", v)
	default:
		fmt.Println("  Default case: channel not ready, didn't block!")
	}

	fmt.Println()

	// --- Part E: Nil channels block forever (useful in select!) ---
	fmt.Println("Part E: Nil channels block forever — useful for disabling select cases")
	var nilCh chan int // nil channel
	doneCh := make(chan bool, 1)
	doneCh <- true

	select {
	case <-nilCh:
		fmt.Println("  This never executes — nil channel blocks forever")
	case <-doneCh:
		fmt.Println("  Done channel wins because nilCh is permanently blocked")
	}
	fmt.Println("  Trick: set a channel to nil in a select loop to disable that case dynamically")

	fmt.Println()

	// --- Part F: Directional channels enforce send/receive only at compile time ---
	fmt.Println("Part F: Directional channels provide compile-time safety")
	bidir := make(chan int, 1)
	sendOnly(bidir)
	recvOnly(bidir)
	fmt.Println("  Directional channel types (chan<-, <-chan) prevent misuse at compile time.")
}

func sendOnly(ch chan<- int) {
	ch <- 42
	fmt.Println("  sendOnly: sent 42")
	// <-ch would be a compile error: cannot receive from send-only channel
}

func recvOnly(ch <-chan int) {
	v := <-ch
	fmt.Printf("  recvOnly: received %d\n", v)
	// ch <- 1 would be a compile error: cannot send to receive-only channel
}
