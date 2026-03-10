/*
 * memory_layout.c — Memory Layout, Unions, and Bit Fields in C
 *
 * Demonstrates:
 *   1. Stack vs heap allocation and their addresses
 *   2. Struct member layout and offsetof
 *   3. Union type punning (members share the same memory)
 *   4. Bit fields for compact storage
 *   5. Endianness detection via union
 *   6. Flexible array members (C99)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

/* --- Struct with explicit padding analysis --- */
struct Example {
    char   a;       /* offset 0 */
    /* 3 bytes padding (to align int to 4) */
    int    b;       /* offset 4 */
    char   c;       /* offset 8 */
    /* 3 bytes padding (to align struct size to 4) */
};

/* --- Union: all members share the same memory --- */
union TypePunner {
    float    f;
    uint32_t u;
    uint8_t  bytes[4];
};

/* --- Endianness detection --- */
union EndianCheck {
    uint32_t value;
    uint8_t  bytes[4];
};

/* --- Bit fields --- */
struct PackedDate {
    unsigned int year  : 12;   /* 0-4095 */
    unsigned int month : 4;    /* 0-15 */
    unsigned int day   : 5;    /* 0-31 */
    unsigned int dow   : 3;    /* 0-7 (day of week) */
    /* Total: 24 bits, fits in one 32-bit int */
};

/* --- Flexible array member (C99) --- */
struct Message {
    uint32_t length;
    char     data[];  /* flexible array member — MUST be last */
};

/* Global and static variables */
int global_var = 42;
static int static_var = 99;

int main(void) {
    printf("=== Memory Regions: Stack, Heap, Global, Code ===\n\n");

    int stack_var = 10;
    int stack_var2 = 20;
    int *heap_var = malloc(sizeof(int));
    int *heap_var2 = malloc(sizeof(int));
    *heap_var = 30;
    *heap_var2 = 40;

    printf("Code (function):  %p  (main)\n", (void*)(size_t)main);
    printf("Global variable:  %p  (global_var)\n", (void*)&global_var);
    printf("Static variable:  %p  (static_var)\n", (void*)&static_var);
    printf("Stack variable 1: %p  (stack_var)\n",  (void*)&stack_var);
    printf("Stack variable 2: %p  (stack_var2)\n", (void*)&stack_var2);
    printf("Heap variable 1:  %p  (heap_var)\n",   (void*)heap_var);
    printf("Heap variable 2:  %p  (heap_var2)\n",  (void*)heap_var2);

    /* Stack typically grows downward */
    printf("\nStack direction: stack_var %s stack_var2\n",
           (size_t)&stack_var > (size_t)&stack_var2
           ? "is at higher address than" : "is at lower address than");
    printf("(Stack usually grows downward on most architectures)\n");

    printf("\n=== Struct Layout and Padding ===\n\n");

    struct Example ex = {'A', 42, 'B'};
    printf("struct Example { char a; int b; char c; }\n");
    printf("  sizeof = %zu  (not %zu — padding!)\n",
           sizeof(struct Example),
           sizeof(char) + sizeof(int) + sizeof(char));
    printf("  offsetof(a) = %zu\n", offsetof(struct Example, a));
    printf("  offsetof(b) = %zu  (padded to align int)\n", offsetof(struct Example, b));
    printf("  offsetof(c) = %zu\n", offsetof(struct Example, c));

    /* Visualize the raw bytes */
    printf("\n  Raw bytes of struct: ");
    unsigned char *raw = (unsigned char*)&ex;
    for (size_t i = 0; i < sizeof(ex); i++) {
        printf("%02X ", raw[i]);
    }
    printf("\n  Padding bytes are visible (typically 00 or garbage).\n");

    printf("\n=== Union: Shared Memory ===\n\n");

    union TypePunner tp;
    tp.f = 3.14f;

    printf("union TypePunner { float f; uint32_t u; uint8_t bytes[4]; }\n");
    printf("  sizeof(union) = %zu  (size of LARGEST member)\n", sizeof(tp));
    printf("  tp.f = %.2f\n", tp.f);
    printf("  tp.u = 0x%08X  (same bits, interpreted as uint32_t)\n", tp.u);
    printf("  tp.bytes = [%02X, %02X, %02X, %02X]  (individual bytes)\n",
           tp.bytes[0], tp.bytes[1], tp.bytes[2], tp.bytes[3]);
    printf("  All three members occupy the SAME memory location!\n");

    printf("\n=== Endianness Detection via Union ===\n\n");

    union EndianCheck ec;
    ec.value = 0x01020304;

    printf("  value = 0x01020304\n");
    printf("  bytes[0] = 0x%02X\n", ec.bytes[0]);

    if (ec.bytes[0] == 0x04) {
        printf("  This machine is LITTLE-ENDIAN (least significant byte first)\n");
    } else if (ec.bytes[0] == 0x01) {
        printf("  This machine is BIG-ENDIAN (most significant byte first)\n");
    } else {
        printf("  This machine has MIXED endianness\n");
    }

    printf("\n=== Bit Fields: Compact Storage ===\n\n");

    struct PackedDate today = { .year = 2026, .month = 3, .day = 10, .dow = 2 };
    printf("struct PackedDate { year:12, month:4, day:5, dow:3 }\n");
    printf("  sizeof = %zu bytes  (24 bits of data packed into %zu bytes)\n",
           sizeof(today), sizeof(today));
    printf("  Date: %u-%02u-%02u (day of week: %u)\n",
           today.year, today.month, today.day, today.dow);

    /* You cannot take the address of a bit field! */
    printf("  Note: &today.year is ILLEGAL — can't take address of bit field.\n");

    /* Show raw bits */
    uint32_t *raw_date = (uint32_t*)&today;
    printf("  Raw bits: 0x%08X\n", *raw_date);

    printf("\n=== Flexible Array Member (C99) ===\n\n");

    /* The flexible array member lets you allocate a struct with
     * a variable-length array at the end in a single allocation. */
    const char *text = "Hello, C memory!";
    size_t len = strlen(text) + 1;

    struct Message *msg = malloc(sizeof(struct Message) + len);
    msg->length = (uint32_t)len;
    memcpy(msg->data, text, len);

    printf("struct Message { uint32_t length; char data[]; }\n");
    printf("  sizeof(struct Message) = %zu  (flexible array has size 0)\n",
           sizeof(struct Message));
    printf("  Allocated: %zu + %zu = %zu bytes\n",
           sizeof(struct Message), len, sizeof(struct Message) + len);
    printf("  msg->length = %u\n", msg->length);
    printf("  msg->data   = \"%s\"\n", msg->data);
    printf("  The struct and its data are in ONE contiguous allocation.\n");

    free(msg);

    printf("\n=== Compound Literal (C99) ===\n\n");

    /* You can create temporary structs with compound literals */
    struct Example *tmp = &(struct Example){ 'X', 100, 'Y' };
    printf("Compound literal: (struct Example){ 'X', 100, 'Y' }\n");
    printf("  tmp->a = '%c', tmp->b = %d, tmp->c = '%c'\n",
           tmp->a, tmp->b, tmp->c);
    printf("  This creates a temporary object with automatic storage duration.\n");

    free(heap_var);
    free(heap_var2);

    printf("\nDone.\n");
    return 0;
}
