/*
 * function_pointers.c — Function Pointers in C
 *
 * Demonstrates:
 *   1. Function pointer declaration syntax
 *   2. Callbacks with function pointers
 *   3. qsort with custom comparators
 *   4. Dispatch tables (jump tables)
 *   5. Functions automatically decay to pointers (like arrays)
 *   6. Dereferencing function pointers is optional
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* --- Comparators for qsort --- */

int compare_ascending(const void *a, const void *b) {
    return (*(const int*)a) - (*(const int*)b);
}

int compare_descending(const void *a, const void *b) {
    return (*(const int*)b) - (*(const int*)a);
}

int compare_by_abs(const void *a, const void *b) {
    int va = abs(*(const int*)a);
    int vb = abs(*(const int*)b);
    return va - vb;
}

/* --- Callback example --- */

typedef int (*Comparator)(const void*, const void*);

void print_array(const char *label, const int *arr, size_t n) {
    printf("  %s: [", label);
    for (size_t i = 0; i < n; i++)
        printf("%s%d", i ? ", " : "", arr[i]);
    printf("]\n");
}

void sort_and_print(int *arr, size_t n, Comparator cmp, const char *desc) {
    qsort(arr, n, sizeof(int), cmp);
    print_array(desc, arr, n);
}

/* --- Arithmetic dispatch table --- */

double add(double a, double b) { return a + b; }
double sub(double a, double b) { return a - b; }
double mul(double a, double b) { return a * b; }
double divide(double a, double b) { return b != 0 ? a / b : 0; }

typedef double (*BinaryOp)(double, double);

int main(void) {
    printf("=== Function Pointer Syntax ===\n\n");

    /* Declaring a function pointer variable */
    int (*fptr)(const void*, const void*) = compare_ascending;

    printf("compare_ascending is at address: %p\n", (void*)(size_t)compare_ascending);
    printf("fptr points to:                  %p\n", (void*)(size_t)fptr);

    /* A function name decays to a pointer automatically,
     * just like array names decay to pointers. */
    printf("\nFunction decay (like array decay):\n");
    printf("  compare_ascending  == &compare_ascending ? %s\n",
           (void*)(size_t)compare_ascending == (void*)(size_t)&compare_ascending
           ? "yes" : "no");

    /* You can call through a function pointer with or without dereferencing */
    int va = 10, vb = 20;
    printf("\nCalling through function pointer:\n");
    printf("  fptr(&va, &vb)    = %d\n", fptr(&va, &vb));
    printf("  (*fptr)(&va, &vb) = %d\n", (*fptr)(&va, &vb));
    printf("  Both are valid! Dereferencing is optional.\n");

    /* You can even do absurd chains: */
    printf("  (****fptr)(&va, &vb) = %d  (also valid!)\n", (****fptr)(&va, &vb));

    printf("\n=== qsort with Different Comparators ===\n\n");

    int data1[] = {5, -3, 8, -1, 4, -7, 2};
    int data2[] = {5, -3, 8, -1, 4, -7, 2};
    int data3[] = {5, -3, 8, -1, 4, -7, 2};
    size_t n = sizeof(data1)/sizeof(data1[0]);

    print_array("Original", data1, n);
    sort_and_print(data1, n, compare_ascending,  "Ascending ");
    sort_and_print(data2, n, compare_descending, "Descending");
    sort_and_print(data3, n, compare_by_abs,     "By abs val");

    printf("\n=== Dispatch Table (Jump Table) ===\n\n");

    /* An array of function pointers acts as a dispatch table.
     * This is how many interpreters and state machines work. */
    BinaryOp operations[] = { add, sub, mul, divide };
    const char *names[]   = { "+", "-", "*", "/" };

    double x = 10.0, y = 3.0;
    printf("Dispatch table: operations on %.1f and %.1f\n", x, y);
    for (int i = 0; i < 4; i++) {
        printf("  %.1f %s %.1f = %.2f\n", x, names[i], y, operations[i](x, y));
    }

    printf("\n=== Function Pointer as Struct Member ===\n\n");

    /* Simulating object-oriented dispatch (like a vtable) */
    typedef struct {
        const char *name;
        double (*operate)(double, double);
    } Operation;

    Operation ops[] = {
        {"add",      add},
        {"subtract", sub},
        {"multiply", mul},
        {"divide",   divide},
    };

    for (int i = 0; i < 4; i++) {
        printf("  ops[%d].operate(6, 2) = %.1f  (%s)\n",
               i, ops[i].operate(6, 2), ops[i].name);
    }

    printf("\n=== Returning Function Pointers ===\n\n");

    /* The syntax for a function returning a function pointer is notoriously ugly.
     * typedef makes it readable. */

    /* Without typedef (hard to read):
     *   double (*get_op(char c))(double, double) { ... }
     * With typedef (readable):
     *   BinaryOp get_op(char c) { ... }
     */
    printf("Without typedef: double (*get_op(char c))(double, double)\n");
    printf("With typedef:    BinaryOp get_op(char c)\n");
    printf("typedefs make function pointer signatures human-readable.\n");

    printf("\n=== sizeof Function Pointer ===\n\n");

    printf("sizeof(BinaryOp)      = %zu\n", sizeof(BinaryOp));
    printf("sizeof(void*)         = %zu\n", sizeof(void*));
    printf("sizeof(compare_ascending) = %zu  (same as pointer size)\n",
           sizeof(&compare_ascending));
    printf("On most platforms, function pointers are the same size as data pointers,\n");
    printf("but the C standard does NOT guarantee this!\n");

    printf("\nDone.\n");
    return 0;
}
