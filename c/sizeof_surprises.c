/*
 * sizeof_surprises.c — sizeof and Struct Padding/Alignment Surprises
 *
 * Demonstrates:
 *   1. sizeof(char) is always 1 by definition
 *   2. sizeof(array) vs sizeof(pointer) — the decay trap
 *   3. sizeof doesn't evaluate its operand (no side effects!)
 *   4. Struct padding and alignment
 *   5. The empty struct question
 *   6. sizeof applied to expressions vs types
 */

#include <stdio.h>
#include <stddef.h>

/* Struct with poor layout — lots of padding */
struct Wasteful {
    char   a;    /* 1 byte + 3 padding */
    int    b;    /* 4 bytes */
    char   c;    /* 1 byte + 7 padding */
    double d;    /* 8 bytes */
};
/* Total might be 24 bytes instead of 14 */

/* Same fields, better layout — minimal padding */
struct Efficient {
    double d;    /* 8 bytes */
    int    b;    /* 4 bytes */
    char   a;    /* 1 byte */
    char   c;    /* 1 byte + 2 padding */
};
/* Total might be 16 bytes */

/* Struct with bit fields */
struct Flags {
    unsigned int active  : 1;
    unsigned int ready   : 1;
    unsigned int mode    : 3;
    unsigned int padding : 27;
};

int main(void) {
    printf("=== sizeof(char) Is Always 1 ===\n\n");

    /* The C standard defines sizeof(char) == 1. Always.
     * CHAR_BIT gives the number of bits in a char (at least 8). */
    printf("sizeof(char)          = %zu  (always 1 by definition)\n", sizeof(char));
    printf("sizeof(unsigned char) = %zu\n", sizeof(unsigned char));
    printf("sizeof(signed char)   = %zu\n", sizeof(signed char));
    printf("All three char types are always size 1.\n");

    printf("\n=== Array vs Pointer sizeof ===\n\n");

    int arr[10];
    int *ptr = arr;

    printf("sizeof(arr) = %zu  (10 * %zu = full array size)\n",
           sizeof(arr), sizeof(int));
    printf("sizeof(ptr) = %zu  (just a pointer, regardless of what it points to)\n",
           sizeof(ptr));
    printf("Number of elements = sizeof(arr)/sizeof(arr[0]) = %zu\n",
           sizeof(arr)/sizeof(arr[0]));

    /* When passed to a function, arrays decay to pointers,
     * so sizeof inside the function gives pointer size, not array size. */
    printf("\nWARNING: When an array is passed to a function,\n");
    printf("sizeof(param) gives the pointer size, not the array size!\n");

    printf("\n=== sizeof Does NOT Evaluate Its Operand ===\n\n");

    /* sizeof is a compile-time operator (except for VLAs).
     * The expression inside sizeof is NEVER executed at runtime. */
    int x = 5;
    size_t s = sizeof(x++);  /* x++ is NOT executed! */
    printf("Before: x = 5\n");
    printf("sizeof(x++) = %zu\n", s);
    printf("After:  x = %d  (x++ was never executed!)\n", x);

    /* This means these are all safe: */
    int *null_ptr = NULL;
    printf("\nsizeof(*null_ptr) = %zu  (no crash! null is never dereferenced)\n",
           sizeof(*null_ptr));

    printf("\n=== Struct Padding and Alignment ===\n\n");

    printf("struct Wasteful (poor layout):\n");
    printf("  sizeof = %zu bytes (sum of fields = %zu)\n",
           sizeof(struct Wasteful),
           sizeof(char) + sizeof(int) + sizeof(char) + sizeof(double));
    printf("  Offsets: a=%zu, b=%zu, c=%zu, d=%zu\n",
           offsetof(struct Wasteful, a),
           offsetof(struct Wasteful, b),
           offsetof(struct Wasteful, c),
           offsetof(struct Wasteful, d));

    printf("\nstruct Efficient (good layout):\n");
    printf("  sizeof = %zu bytes (same fields, less padding!)\n",
           sizeof(struct Efficient));
    printf("  Offsets: d=%zu, b=%zu, a=%zu, c=%zu\n",
           offsetof(struct Efficient, d),
           offsetof(struct Efficient, b),
           offsetof(struct Efficient, a),
           offsetof(struct Efficient, c));

    printf("\nWasted space: %zu bytes just from field ordering!\n",
           sizeof(struct Wasteful) - sizeof(struct Efficient));

    printf("\n=== Bit Fields ===\n\n");

    struct Flags f = {1, 0, 5, 0};
    printf("struct Flags with bit fields:\n");
    printf("  sizeof = %zu bytes (all flags packed into one int)\n",
           sizeof(struct Flags));
    printf("  active=%u, ready=%u, mode=%u\n", f.active, f.ready, f.mode);

    printf("\n=== sizeof on Expressions vs Types ===\n\n");

    /* sizeof can take a type in parentheses or an expression */
    printf("sizeof(int)       = %zu\n", sizeof(int));
    printf("sizeof(int*)      = %zu\n", sizeof(int*));
    printf("sizeof(int[100])  = %zu\n", sizeof(int[100]));
    printf("sizeof 42         = %zu  (type of 42 is int)\n", sizeof 42);
    printf("sizeof 42L        = %zu  (type of 42L is long)\n", sizeof 42L);
    printf("sizeof 42LL       = %zu  (type of 42LL is long long)\n", sizeof 42LL);
    printf("sizeof 3.14       = %zu  (type of 3.14 is double)\n", sizeof 3.14);
    printf("sizeof 3.14f      = %zu  (type of 3.14f is float)\n", sizeof 3.14f);

    /* Character literals in C are int, not char! (Unlike C++) */
    printf("\nsizeof('A')       = %zu  (in C, char literals are int!)\n", sizeof('A'));
    printf("sizeof((char)'A') = %zu  (explicitly cast to char)\n", sizeof((char)'A'));

    printf("\nDone.\n");
    return 0;
}
