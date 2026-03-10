/*
 * undefined_behavior.c — Demonstrating (Safely!) Undefined Behavior in C
 *
 * NOTE: We demonstrate these concepts by printing observations, but we
 * carefully avoid actually invoking UB in ways that would crash or corrupt.
 * Where real UB would occur, we show the principle and explain what the
 * standard says.
 *
 * Demonstrates:
 *   1. Signed integer overflow is UB (unsigned wraps, signed does not)
 *   2. Uninitialized local variables hold indeterminate values
 *   3. Strict aliasing violations
 *   4. Sequence point violations
 *   5. Shifting by more than the width of a type
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

int main(void) {
    printf("=== Unsigned Wrap vs Signed Overflow ===\n\n");

    /* Unsigned arithmetic is well-defined: it wraps modulo 2^N */
    unsigned int u = UINT_MAX;
    printf("UINT_MAX     = %u\n", u);
    printf("UINT_MAX + 1 = %u  (wraps to 0 — well defined)\n", u + 1u);

    /* Signed overflow is UNDEFINED BEHAVIOR.
     * The compiler is allowed to assume it NEVER happens,
     * which lets it optimize in surprising ways. */
    int s = INT_MAX;
    printf("\nINT_MAX = %d\n", s);
    printf("INT_MAX + 1 would be UNDEFINED BEHAVIOR.\n");
    printf("The compiler can assume signed overflow never happens.\n");
    printf("This means: if (x + 1 > x) can be optimized to always-true!\n");

    /* Demonstration of how the compiler exploits this: */
    /* A compiler might optimize this loop to be infinite if it assumes
     * signed overflow can't happen: for(int i = 0; i >= 0; i++) */
    printf("\nA loop 'for(int i=0; i>=0; i++)' may NEVER terminate\n");
    printf("because the compiler assumes i never overflows past INT_MAX.\n");

    printf("\n=== Uninitialized Variables ===\n\n");

    /* Local variables in C are NOT zero-initialized.
     * Reading an uninitialized variable is UB in some cases. */
    /* We intentionally show the concept without actually reading garbage: */
    volatile int uninit;  /* volatile prevents optimization */
    printf("An uninitialized local int could contain ANY value.\n");
    printf("The standard says reading it is undefined behavior.\n");
    printf("Static/global variables ARE zero-initialized by the standard.\n");

    static int zero_init;
    printf("static int zero_init = %d  (guaranteed to be 0)\n", zero_init);

    printf("\n=== Strict Aliasing: Type Punning the Right Way ===\n\n");

    /* Accessing an object through a pointer of incompatible type
     * violates strict aliasing and is UB. The legal way is memcpy. */
    float f = 3.14f;
    uint32_t bits;

    /* WRONG (UB via strict aliasing violation):
     *   uint32_t bits = *(uint32_t*)&f;
     * The compiler may reorder or optimize away the read. */

    /* RIGHT (well-defined via memcpy): */
    memcpy(&bits, &f, sizeof(bits));
    printf("float %.2f has bit pattern: 0x%08X\n", f, bits);
    printf("Using memcpy for type punning is well-defined.\n");
    printf("Using *(uint32_t*)&f is a strict aliasing violation (UB).\n");

    printf("\n=== Sequence Points ===\n\n");

    /* Modifying a variable more than once between sequence points is UB.
     * Examples of UB:
     *   i = i++ + i++;
     *   a[i] = i++;
     *   printf(\"%d %d\", i++, i++);
     * The order of evaluation of function arguments is unspecified. */
    printf("i = i++ + i++  is UNDEFINED BEHAVIOR.\n");
    printf("The variable 'i' is modified twice without an intervening\n");
    printf("sequence point. Different compilers give different results.\n");

    int x = 5;
    /* Safe version: separate statements with sequence points */
    x++;   /* sequence point at ; */
    x++;   /* sequence point at ; */
    printf("After two separate x++ statements: x = %d  (well-defined)\n", x);

    printf("\n=== Shift by Width of Type ===\n\n");

    /* Shifting by >= the bit width of the type is UB for signed and unsigned. */
    printf("sizeof(int) = %zu, so int has %zu bits\n",
           sizeof(int), sizeof(int) * CHAR_BIT);
    printf("1 << 31 on a 32-bit int: result depends on signedness.\n");
    printf("1 << 32 on a 32-bit int: UNDEFINED BEHAVIOR.\n");
    printf("1u << 31 = %u  (well-defined for unsigned)\n", 1u << 31);
    printf("1u << 32 would be UB (shift >= width of type).\n");

    /* Negative shift amounts are also UB */
    printf("Shifting by a negative amount is also UB.\n");

    printf("\n=== Division by Zero ===\n\n");

    printf("Integer division by zero is undefined behavior.\n");
    printf("INT_MIN / -1 is also UB (result overflows signed int).\n");
    printf("INT_MIN = %d, -INT_MIN would overflow.\n", INT_MIN);

    printf("\nDone.\n");
    return 0;
}
