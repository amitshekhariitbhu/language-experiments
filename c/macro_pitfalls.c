/*
 * macro_pitfalls.c — Preprocessor Macro Pitfalls in C
 *
 * Demonstrates:
 *   1. Double evaluation of arguments
 *   2. Operator precedence issues without parentheses
 *   3. Side effects in macro arguments
 *   4. Type-unsafe macros vs inline functions
 *   5. Multiline macro pitfalls (dangling else)
 *   6. Token pasting and stringification
 */

#include <stdio.h>
#include <stdlib.h>

/* PITFALL 1: No parentheses around parameters */
#define BAD_DOUBLE(x)  x * 2
#define GOOD_DOUBLE(x) ((x) * 2)

/* PITFALL 2: Double evaluation */
#define BAD_MAX(a, b)  ((a) > (b) ? (a) : (b))

/* PITFALL 3: No braces in multi-statement macro */
#define BAD_SWAP(a, b)  int _tmp = (a); (a) = (b); (b) = _tmp;
/* Better: use do-while(0) idiom */
#define GOOD_SWAP(a, b) do { int _tmp = (a); (a) = (b); (b) = _tmp; } while(0)

/* PITFALL 4: Macro vs function — type blindness */
#define SQUARE(x) ((x) * (x))

/* Stringification and token pasting */
#define STRINGIFY(x)     #x
#define CONCAT(a, b)     a##b
#define XSTRINGIFY(x)    STRINGIFY(x)

#define VERSION 42

/* A macro that counts invocations via side effect */
static int call_count = 0;
int counted_value(int v) {
    call_count++;
    return v;
}

int main(void) {
    printf("=== Parentheses Matter ===\n\n");

    printf("BAD_DOUBLE(3 + 1)  = %d  (expands to: 3 + 1 * 2 = 5!)\n",
           BAD_DOUBLE(3 + 1));
    printf("GOOD_DOUBLE(3 + 1) = %d  (expands to: ((3 + 1) * 2) = 8)\n",
           GOOD_DOUBLE(3 + 1));

    printf("\n=== Double Evaluation ===\n\n");

    /* BAD_MAX evaluates the "winning" argument TWICE */
    int a = 5, b = 3;
    call_count = 0;
    int result = BAD_MAX(counted_value(a), counted_value(b));
    printf("BAD_MAX(counted_value(5), counted_value(3)) = %d\n", result);
    printf("counted_value was called %d times (expected 2, got 3!)\n", call_count);
    printf("The winning argument (5) was evaluated TWICE.\n");

    /* With increment operators, it's even worse */
    int x = 5, y = 3;
    printf("\nBefore: x=%d, y=%d\n", x, y);
    int max_val = BAD_MAX(x++, y++);
    printf("BAD_MAX(x++, y++) = %d\n", max_val);
    printf("After:  x=%d, y=%d\n", x, y);
    printf("x was incremented TWICE because it was the larger value!\n");

    printf("\n=== Multi-Statement Macro: Dangling Else ===\n\n");

    /* BAD_SWAP without do-while(0) breaks in if-else:
     *
     *   if (cond)
     *       BAD_SWAP(a, b);   // expands to multiple statements!
     *   else
     *       ...               // compiler error: else without if
     *
     * The do-while(0) trick makes the macro a single statement.
     */
    int p = 10, q = 20;
    printf("Before GOOD_SWAP: p=%d, q=%d\n", p, q);
    if (1)
        GOOD_SWAP(p, q);  /* works correctly in if-else */
    else
        (void)0;
    printf("After GOOD_SWAP:  p=%d, q=%d\n", p, q);
    printf("do-while(0) idiom makes multi-statement macros safe in if-else.\n");

    printf("\n=== SQUARE Macro Double Evaluation ===\n\n");

    int val = 4;
    printf("SQUARE(val)  = %d  (correct: 4*4 = 16)\n", SQUARE(val));
    printf("SQUARE(val++) = ?\n");
    val = 4;
    int sq = SQUARE(val++);
    printf("SQUARE(val++) = %d  (expands to (val++) * (val++) = 4*5 = 20?!)\n", sq);
    printf("val is now %d (incremented twice!)\n", val);
    printf("Actual result is implementation-defined due to unsequenced modifications.\n");

    printf("\n=== Stringification and Token Pasting ===\n\n");

    /* # turns a macro argument into a string literal */
    printf("STRINGIFY(hello world) = \"%s\"\n", STRINGIFY(hello world));

    /* Stringification does NOT expand macros in its argument */
    printf("STRINGIFY(VERSION) = \"%s\"  (not expanded!)\n", STRINGIFY(VERSION));
    /* Double indirection trick forces expansion first */
    printf("XSTRINGIFY(VERSION) = \"%s\"  (expanded via indirection)\n",
           XSTRINGIFY(VERSION));

    /* ## concatenates tokens */
    int CONCAT(my, _var) = 999;  /* creates variable: my_var */
    printf("CONCAT(my, _var) created variable my_var = %d\n", my_var);

    printf("\n=== Macro vs Inline Function ===\n\n");

    /* Macros have no type checking. This "works" but may surprise: */
    printf("SQUARE applied to different types:\n");
    printf("  SQUARE(3)    = %d   (int * int)\n", SQUARE(3));
    printf("  SQUARE(3.5)  = %f   (double * double)\n", SQUARE(3.5));
    printf("  SQUARE('A')  = %d   (char promoted to int: 65*65)\n", SQUARE('A'));
    printf("An inline function would enforce a single type.\n");

    printf("\n=== Comma Operator in Macro Arguments ===\n\n");

    /* The preprocessor splits arguments on commas, even inside expressions.
     * BAD_MAX(1, (2, 3)) — the (2,3) uses the comma OPERATOR.
     * Without parentheses, BAD_MAX(a, b, c) would be 3 args. */
    printf("BAD_MAX((2, 3), 1) = %d\n", BAD_MAX((2, 3), 1));
    printf("The comma operator (2,3) evaluates to 3.\n");
    printf("Without parens, the macro would see 3 arguments and fail.\n");

    printf("\nDone.\n");
    return 0;
}
