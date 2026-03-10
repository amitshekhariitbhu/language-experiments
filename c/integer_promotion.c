/*
 * integer_promotion.c — Integer Promotion and Conversion Traps
 *
 * Demonstrates:
 *   1. Small types (char, short) are promoted to int in expressions
 *   2. Unsigned/signed comparison traps: -1 > 1u is TRUE
 *   3. Implicit conversion rank rules
 *   4. The sizeof subtraction trap
 *   5. Bitwise operations on signed types
 */

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

int main(void) {
    printf("=== Integer Promotion: Small Types Become int ===\n\n");

    /* When char or short appear in an expression, they are promoted to int
     * before any operation is performed. */
    char c1 = 100, c2 = 100;

    /* c1 * c2 = 10000, which overflows char (max 127 for signed char).
     * But the multiplication actually happens on int-promoted values,
     * so the result is correct as an int. */
    printf("char c1 = 100, c2 = 100\n");
    printf("c1 * c2 = %d  (promoted to int before multiplication)\n", c1 * c2);
    printf("sizeof(c1 * c2) = %zu  (result is int, not char!)\n", sizeof(c1 * c2));
    printf("sizeof(c1) = %zu, but sizeof(c1 + 0) = %zu\n",
           sizeof(c1), sizeof(c1 + 0));

    /* Even unary + promotes to int */
    printf("sizeof(+c1) = %zu  (unary + promotes char to int)\n", sizeof(+c1));

    printf("\n=== The Signed/Unsigned Comparison Trap ===\n\n");

    /* When comparing signed and unsigned of the same rank,
     * the signed value is converted to unsigned. */
    int          neg = -1;
    unsigned int pos = 1;

    printf("int neg = -1, unsigned int pos = 1\n");
    printf("neg < pos ? %s\n", neg < pos ? "true" : "FALSE — SURPRISE!");

    /* -1 converted to unsigned becomes UINT_MAX (all bits set) */
    printf("-1 as unsigned int = %u  (that's UINT_MAX!)\n", (unsigned int)-1);
    printf("So the comparison becomes: %u < 1 => false\n", (unsigned int)-1);

    printf("\n=== More Signed/Unsigned Surprises ===\n\n");

    /* This affects all comparison and arithmetic operators */
    unsigned int u = 1;
    int          s = -2;
    printf("unsigned 1 + signed (-2) = %u  (result is unsigned!)\n", u + s);
    printf("As signed, that would be: %d\n", (int)(u + s));

    /* The ternary operator also converts: */
    int result = (neg > pos) ? neg : pos;
    printf("((-1 > 1u) ? -1 : 1) = %d  (surprise: -1 > 1u is true!)\n",
           (-1 > 1u) ? -1 : 1);

    printf("\n=== The sizeof Subtraction Trap ===\n\n");

    /* sizeof returns size_t, which is unsigned.
     * Subtracting two size_t values can wrap around. */
    int arr1[5], arr2[10];

    /* sizeof(arr1) - sizeof(arr2) wraps to a huge positive number
     * because the result type is size_t (unsigned). */
    size_t diff = sizeof(arr1) - sizeof(arr2);
    printf("sizeof(arr1) = %zu, sizeof(arr2) = %zu\n",
           sizeof(arr1), sizeof(arr2));
    printf("sizeof(arr1) - sizeof(arr2) = %zu  (unsigned underflow!)\n", diff);
    printf("As signed: %zd\n", (ssize_t)diff);

    /* This is a common bug in conditions: */
    if (sizeof(arr1) - sizeof(arr2) > 0) {
        printf("sizeof(arr1) - sizeof(arr2) > 0 is TRUE (due to unsigned wrap)!\n");
    }

    printf("\n=== Promotion in Bitwise Operations ===\n\n");

    unsigned char mask = 0xFF;
    /* ~mask should give 0x00 for a byte, but promotion to int
     * means we get 0xFFFFFF00 (on 32-bit int) */
    printf("unsigned char mask = 0xFF\n");
    printf("~mask = 0x%X  (promoted to int, upper bits are set!)\n", ~mask);
    printf("Expected 0x00, got 0x%X because ~operates on int\n", ~mask);

    /* Correct way: mask back to the desired width */
    printf("(unsigned char)(~mask) = 0x%X  (correct)\n", (unsigned char)(~mask));

    printf("\n=== Implicit Conversion Rank ===\n\n");

    /* Types have a rank: long long > long > int > short > char
     * In binary operations, the lower-ranked operand is promoted. */
    int    i = 42;
    long   l = 42;
    float  f = 42.0f;
    double d = 42.0;

    printf("int + long     -> long    (sizeof = %zu)\n", sizeof(i + l));
    printf("int + float    -> float   (sizeof = %zu)\n", sizeof(i + f));
    printf("int + double   -> double  (sizeof = %zu)\n", sizeof(i + d));
    printf("float + double -> double  (sizeof = %zu)\n", sizeof(f + d));

    printf("\nDone.\n");
    return 0;
}
