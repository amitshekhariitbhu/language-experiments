/*
 * string_gotchas.c — String Handling Pitfalls in C
 *
 * Demonstrates:
 *   1. C has no string type — just null-terminated char arrays
 *   2. == compares pointers, not string contents
 *   3. Missing null terminator leads to reading garbage
 *   4. Buffer overflow with strcpy
 *   5. String literal storage and modification attempts
 *   6. Implicit string concatenation
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    printf("=== No String Type: Just char Arrays ===\n\n");

    /* A "string" in C is a char array ending with '\0' (null terminator).
     * The null terminator takes one extra byte. */
    char greeting[] = "Hello";
    printf("greeting = \"%s\"\n", greeting);
    printf("sizeof(greeting) = %zu  (5 chars + 1 null terminator)\n",
           sizeof(greeting));
    printf("strlen(greeting) = %zu  (does NOT count the null terminator)\n",
           strlen(greeting));

    /* Manually showing the null terminator */
    printf("Characters: ");
    for (size_t i = 0; i <= strlen(greeting); i++) {
        if (greeting[i] == '\0')
            printf("'\\0' ");
        else
            printf("'%c' ", greeting[i]);
    }
    printf("\n");

    printf("\n=== == Compares Pointers, Not Contents ===\n\n");

    char a[] = "hello";
    char b[] = "hello";

    /* a and b are separate arrays on the stack with identical content.
     * == compares their addresses, not their contents. */
    printf("a[] = \"%s\", b[] = \"%s\"\n", a, b);
    printf("a == b ? %s  (comparing ADDRESSES: %p vs %p)\n",
           a == b ? "true" : "false", (void*)a, (void*)b);
    printf("strcmp(a, b) == 0 ? %s  (comparing CONTENTS)\n",
           strcmp(a, b) == 0 ? "true" : "false");

    /* String literals MAY be deduplicated by the compiler */
    const char *p1 = "world";
    const char *p2 = "world";
    printf("\nString literal deduplication:\n");
    printf("p1 == p2 ? %s  (compiler MAY share the same literal)\n",
           p1 == p2 ? "true" : "false");
    printf("This is implementation-defined — don't rely on it!\n");

    printf("\n=== Missing Null Terminator ===\n\n");

    /* If you create a char array without a null terminator,
     * string functions will read past the end of the array. */
    char no_null[5] = {'H', 'e', 'l', 'l', 'o'};  /* NO '\0' */
    printf("sizeof(no_null) = %zu\n", sizeof(no_null));
    printf("This array has NO null terminator.\n");
    printf("strlen() would read past the array into adjacent memory.\n");
    printf("printf with %%s would print garbage after 'Hello'.\n");

    /* Contrast with proper initialization */
    char with_null[6] = {'H', 'e', 'l', 'l', 'o', '\0'};
    printf("with_null (properly terminated): \"%s\"\n", with_null);

    /* Sneaky: if the array is larger, remaining bytes are zero-initialized */
    char padded[10] = {'H', 'i'};
    printf("padded = \"%s\" (remaining 8 bytes are zero, acting as terminator)\n",
           padded);

    printf("\n=== Buffer Overflow with String Operations ===\n\n");

    char small[6];  /* Can hold 5 chars + null */
    const char *source = "Hi";

    /* Safe: fits in buffer */
    strcpy(small, source);
    printf("After strcpy(small, \"Hi\"): \"%s\" — fits fine.\n", small);

    /* Dangerous: strcpy doesn't check bounds */
    printf("strcpy(small, \"This is way too long\") would overflow!\n");
    printf("C provides NO bounds checking on string operations.\n");
    printf("Use strncpy or snprintf for safer alternatives.\n");

    /* snprintf is the safe way */
    char safe[6];
    int written = snprintf(safe, sizeof(safe), "This is way too long");
    printf("snprintf into 6-byte buffer: \"%s\" (truncated, wanted %d chars)\n",
           safe, written);

    printf("\n=== String Literals Are Read-Only ===\n\n");

    /* String literals are stored in read-only memory.
     * char* pointing to a literal is a common bug source. */
    const char *literal = "immutable";
    printf("literal = \"%s\"\n", literal);
    printf("Modifying *literal would be undefined behavior (likely crash).\n");
    printf("Use 'const char*' for string literals to catch this at compile time.\n");

    /* char[] copies the literal onto the stack — this IS modifiable */
    char mutable_copy[] = "mutable";
    mutable_copy[0] = 'M';
    printf("mutable_copy (char[]): \"%s\" — modification is fine.\n", mutable_copy);

    printf("\n=== Implicit String Literal Concatenation ===\n\n");

    /* Adjacent string literals are concatenated at compile time.
     * This is a feature, but can cause bugs if you forget a comma. */
    const char *combined = "Hello, " "World" "!";
    printf("\"Hello, \" \"World\" \"!\" = \"%s\"\n", combined);

    /* Common bug: forgotten comma in an array of strings */
    const char *words[] = {
        "apple",
        "banana"    /* <-- forgot comma here */
        "cherry",   /* becomes "bananacherry" */
        "date"
    };
    printf("\nForgotten comma bug:\n");
    printf("Expected 4 words, got %zu:\n", sizeof(words)/sizeof(words[0]));
    for (size_t i = 0; i < sizeof(words)/sizeof(words[0]); i++) {
        printf("  words[%zu] = \"%s\"\n", i, words[i]);
    }
    printf("\"banana\" and \"cherry\" got concatenated!\n");

    printf("\nDone.\n");
    return 0;
}
