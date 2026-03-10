/*
 * pointer_arithmetic.c — Pointer Arithmetic Surprises in C
 *
 * Demonstrates:
 *   1. Pointer arithmetic scales by the size of the pointed-to type
 *   2. Arrays decay to pointers in most contexts
 *   3. Pointer subtraction returns element count, not byte count
 *   4. Array indexing is secretly pointer arithmetic (a[i] == *(a+i) == i[a])
 */

#include <stdio.h>
#include <stdint.h>

int main(void) {
    printf("=== Pointer Arithmetic Scales by Type Size ===\n\n");

    int    arr_int[5]    = {10, 20, 30, 40, 50};
    double arr_double[5] = {1.1, 2.2, 3.3, 4.4, 5.5};
    char   arr_char[5]   = {'A', 'B', 'C', 'D', 'E'};

    int    *pi = arr_int;
    double *pd = arr_double;
    char   *pc = arr_char;

    /* Adding 1 to a pointer advances by sizeof(type) bytes, not 1 byte */
    printf("int*:    pi=%p,  pi+1=%p  (diff = %ld bytes, sizeof(int)=%zu)\n",
           (void*)pi, (void*)(pi+1),
           (long)((char*)(pi+1) - (char*)pi), sizeof(int));

    printf("double*: pd=%p,  pd+1=%p  (diff = %ld bytes, sizeof(double)=%zu)\n",
           (void*)pd, (void*)(pd+1),
           (long)((char*)(pd+1) - (char*)pd), sizeof(double));

    printf("char*:   pc=%p,  pc+1=%p  (diff = %ld bytes, sizeof(char)=%zu)\n",
           (void*)pc, (void*)(pc+1),
           (long)((char*)(pc+1) - (char*)pc), sizeof(char));

    printf("\n=== Array Decay: sizeof(array) vs sizeof(pointer) ===\n\n");

    /* In most expressions, an array name decays to a pointer to its first element.
     * BUT sizeof is one of the few contexts where it does NOT decay. */
    printf("sizeof(arr_int) = %zu  (full array: 5 * %zu = %zu bytes)\n",
           sizeof(arr_int), sizeof(int), 5 * sizeof(int));

    int *decayed = arr_int;  /* array decays to pointer here */
    printf("sizeof(decayed) = %zu  (just a pointer, lost array size info)\n",
           sizeof(decayed));

    printf("\n=== Pointer Subtraction Returns Element Count ===\n\n");

    int *start = &arr_int[0];
    int *end   = &arr_int[4];

    /* Subtracting two pointers gives the number of ELEMENTS between them,
     * not the number of bytes. The result type is ptrdiff_t. */
    printf("end - start = %td elements (not %ld bytes)\n",
           end - start,
           (long)((char*)end - (char*)start));

    printf("\n=== The Commutative Indexing Trick: a[i] == i[a] ===\n\n");

    /* a[i] is defined as *(a + i). Since addition is commutative,
     * *(a + i) == *(i + a) == i[a]. This is valid C! */
    printf("arr_int[2] = %d\n", arr_int[2]);
    printf("2[arr_int] = %d\n", 2[arr_int]);
    printf("*(arr_int + 2) = %d\n", *(arr_int + 2));

    /* This even works with string literals */
    printf("\n\"HELLO\"[1] = '%c'\n", "HELLO"[1]);
    printf("1[\"HELLO\"] = '%c'\n", 1["HELLO"]);

    printf("\n=== Multidimensional Array Pointer Arithmetic ===\n\n");

    int mat[3][4] = {
        { 1,  2,  3,  4},
        { 5,  6,  7,  8},
        { 9, 10, 11, 12}
    };

    /* mat decays to int(*)[4] — a pointer to an array of 4 ints.
     * Adding 1 advances by sizeof(int[4]) = 16 bytes. */
    printf("mat      = %p\n", (void*)mat);
    printf("mat + 1  = %p  (advanced by %zu bytes = sizeof(int[4]))\n",
           (void*)(mat + 1),
           (size_t)((char*)(mat+1) - (char*)mat));

    /* Flattening via pointer cast — legal but subtle */
    int *flat = (int*)mat;
    printf("Flat access: flat[5] = %d  (same as mat[1][1])\n", flat[5]);
    printf("mat[1][1]   = %d\n", mat[1][1]);

    printf("\nDone.\n");
    return 0;
}
