#ifndef ALIB_LINALG_H
#define ALIB_LINALG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./utils.h"

/* 
 * aidan bird 2021 
 *
 * This file does:
 * - Generic matrix structs
 * - Matrix operations
 * - LU Factorization
 * - Solving linear systems
 *
 * Usage:
 * DEF_MATRIX(type, name prefix, printf format, zero type, one type) \
 * DEF_MATRIX_REAL is for float and double types.
 */

#define xstr(s) str(s)
#define str(s) #s

/* returns non-zero if the dimensions of A are the same as the dimensions of B */
#define linalg_dims_eq(A_PTR, B_PTR) \
    ((A_PTR) == (B_PTR) || ((A_PTR)->n == (B_PTR)->n && (A_PTR)->m == (B_PTR)->m))

/* returns the address (void*) of an element */
#define linalg_addr_of_matrix_element(MATRIX_PTR, ROW, COL) \
    (void *)((MATRIX_PTR)->start + (ROW) * (MATRIX_PTR)->m + (COL))

/* return the element at row and col  */
#define linalg_get_matrix_element(MATRIX_PTR, ROW, COL) \
    (MATRIX_PTR)->start[(ROW) * (MATRIX_PTR)->m + (COL)]

/* 
 * get the total number of elements in a matrix. 
 * this returns n*m
 */
#define linalg_get_matrix_element_count(MATRIX_PTR) \
    ((MATRIX_PTR)->n * (MATRIX_PTR)->m) \

/* 
 * get the sizeof all the matrix elements.
 * T is the matrix's element type
 */
#define linalg_sizeof_matrix(MATRIX_PTR, T) \
    (size_t)(sizeof(T) * linalg_get_matrix_element_count(MATRIX_PTR)) \

/* 
 * get the sizeof all the matrix elements + the sizeof the matrix struct itself.
 * T is the matrix's element type
 */
#define linalg_sizeof_matrix_total(MATRIX_PTR, T) \
    (sizeof(*MATRIX_PTR) + linalg_sizeof_matrix(MATRIX_PTR , T)) \

/* returns non-zero if the matrix is square */
#define linalg_is_matrix_square(MATRIX_PTR) \
    ((MATRIX_PTR)->n == (MATRIX_PTR)->m)

/* returns non-zero if A can contain B */
#define linalg_can_contain(A_PTR, B_PTR) \
    ((A_PTR)->n >= (B_PTR)->n && (A_PTR)->m >= (B_PTR)->m)

/* used for defining a new matrix type */
#define DEF_MATRIX(T, PREFIX, PRINTF_STR, ZERO, ONE) \
    DEF_MATRIX_BASE(T, PREFIX, PRINTF_STR, ZERO, ONE) \

/* used for defining a floating point typed matrix */
#define DEF_MATRIX_REAL(T, PREFIX, PRINTF_STR, ZERO, ONE, ABS_FUNC) \
    DEF_MATRIX_BASE(T, PREFIX, PRINTF_STR, ZERO, ONE) \
    DEF_MATRIX_BASE(uint8_t, PREFIX##Permutation, "%d ", 0, 1) \
    DEF_MATRIX_EXT(T, PREFIX, PRINTF_STR, ZERO, ONE, ABS_FUNC)

#define DEF_MATRIX_BASE(T, PREFIX, PRINTF_STR, ZERO, ONE) \
typedef struct PREFIX##Matrix PREFIX##Matrix; \
PREFIX##Matrix * \
new##PREFIX##Matrix(size_t n, size_t m); \
void PREFIX##MatrixTranspose(PREFIX##Matrix *mat); \
PREFIX##Matrix *PREFIX##MatrixDup(const PREFIX##Matrix *mat); \
void PREFIX##MatrixOnes(PREFIX##Matrix *mat); \
void PREFIX##MatrixZeros(PREFIX##Matrix *mat); \
int PREFIX##MatrixEye(PREFIX##Matrix *mat); \
void PREFIX##MatrixSetDiag(PREFIX##Matrix *mat, T x); \
void PREFIX##MatrixPrint(const PREFIX##Matrix *mat); \
void PREFIX##MatrixZeroLowerTriangle(PREFIX##Matrix *a); \
void PREFIX##MatrixZeroUpperTriangle(PREFIX##Matrix *a); \
int PREFIX##MatrixCopyUpperTriangle(PREFIX##Matrix *dest, \
    const PREFIX##Matrix *restrict src); \
int PREFIX##MatrixSubRow(PREFIX##Matrix *dest, const PREFIX##Matrix *src, \
    size_t destrow, size_t srcrow, float rowscale); \
int PREFIX##MatrixCpy(PREFIX##Matrix *restrict dest, \
    const PREFIX##Matrix *restrict src); \
PREFIX##Matrix * PREFIX##MatrixMultSimple(const PREFIX##Matrix *a,\
    const PREFIX##Matrix *b); \
struct PREFIX##Matrix \
{ \
    size_t n; \
    size_t m; \
    T *start; \
}; \
\
/*
 * REQUIRES
 * none
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * makes a new n by m matrix of T.
 * returns NULL on error.
 */ \
PREFIX##Matrix *\
new##PREFIX##Matrix(size_t n, size_t m) \
{ \
    size_t vectorSize; \
    PREFIX##Matrix *ret; \
 \
    vectorSize = sizeof(PREFIX##Matrix) + n * m * sizeof(T); \
    ret = malloc(vectorSize); \
    if (!ret) \
        return NULL; \
    ret->n = n; \
    ret->m = m; \
    ret->start = (T *)((uint8_t *)ret + sizeof(PREFIX##Matrix)); \
    return ret; \
} \
\
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * mat
 *
 * EFFECTS
 * replaces mat with its transpose.
 */ \
void \
PREFIX##MatrixTranspose(PREFIX##Matrix *mat) \
{ \
    T tmp[mat->n * mat->m]; \
\
    for (size_t i = 0; i < mat->n ; i++) \
        for (size_t j = 0; j < mat->m ; j++) \
            tmp[j * mat->m + i] = linalg_get_matrix_element(mat, i, j); \
    memcpy(mat->start, tmp, sizeof(tmp)); \
} \
\
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * duplicates mat.
 */ \
PREFIX##Matrix * \
PREFIX##MatrixDup(const PREFIX##Matrix *mat) \
{ \
    PREFIX##Matrix *ret; \
 \
    ret = new##PREFIX##Matrix(mat->n, mat->m); \
    if (!ret) \
        return NULL; \
    memcpy(ret->start, mat->start, linalg_sizeof_matrix(mat, T)); \
    return ret; \
} \
 \
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * mat
 *
 * EFFECTS
 * sets all elements in mat to ONE
 */ \
void \
PREFIX##MatrixOnes(PREFIX##Matrix *mat) \
{ \
    for (size_t i = 0; i < (mat->n * mat->m); i++) \
        mat->start[i] = (ONE); \
} \
 \
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * mat
 *
 * EFFECTS
 * sets all elements in mat to ZERO
 */ \
void \
PREFIX##MatrixZeros(PREFIX##Matrix *mat) \
{ \
    for (size_t i = 0; i < linalg_get_matrix_element_count(mat); i++) \
        mat->start[i] = (ZERO); \
} \
 \
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * mat
 *
 * EFFECTS
 * sets mat to be the identity matrix
 * mat must be a square matrix
 * returns non-zero on error (i.e., mat is not square)
 */ \
int \
PREFIX##MatrixEye(PREFIX##Matrix *mat) \
{ \
    if (!linalg_is_matrix_square(mat)) \
        return -1; \
    PREFIX##MatrixZeros(mat); \
    PREFIX##MatrixSetDiag(mat, ONE); \
    return 0; \
} \
\
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * mat
 *
 * EFFECTS
 * sets all elements along the diagonal of mat to x.
 */ \
void \
PREFIX##MatrixSetDiag(PREFIX##Matrix *mat, T x) \
{ \
    for (size_t i = 0; i < MIN(mat->n, mat->m); i++) \
        linalg_get_matrix_element(mat, i, i) = x; \
} \
 \
/*
 * REQUIRES
 * mat is valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * prints mat
 */ \
void \
PREFIX##MatrixPrint(const PREFIX##Matrix *mat) \
{ \
    for (size_t i = 0; i < mat->n; i++) { \
        for (size_t j = 0; j < mat->m; j++) \
            printf(PRINTF_STR, linalg_get_matrix_element(mat, i, j)); \
        putchar('\n'); \
    } \
} \
 \
/*
 * REQUIRES
 * a is valid
 *
 * MODIFIES
 * a
 *
 * EFFECTS
 * sets the elements in the lower triangle of a to zero.
 */ \
void \
PREFIX##MatrixZeroLowerTriangle(PREFIX##Matrix *a) \
{ \
    for (size_t i = 0; i < MIN(a->n, a->m); i++) \
        for (size_t j = 0; j < i; j++) \
            linalg_get_matrix_element(a, i, j) = 0; \
} \
\
/*
 * REQUIRES
 * dest and src are valid
 *
 * MODIFIES
 * dest
 *
 * EFFECTS
 * copies the upper triangle of src to dest.
 * only the elements in dest that corospond to the elements in the upper 
 * triangle of src are modified.
 * returns non-zero on error i.e., src is larger than dest.
 */ \
int \
PREFIX##MatrixCopyUpperTriangle(PREFIX##Matrix *dest, \
    const PREFIX##Matrix *restrict src) \
{ \
    if (dest->m < src->m || dest->n < src->n) \
        return -1; \
    for (size_t i = 0; i < MIN(src->n, src->m); i++) { \
        for (size_t j = i; j < src->m; j++) { \
            linalg_get_matrix_element(dest, i, j) =  \
                linalg_get_matrix_element(src, i, j); \
        } \
    } \
    return 0; \
} \
/*
 * REQUIRES
 * dest and src are valid.
 * destrow and srcrow are valid row indices.
 *
 * MODIFIES
 * dest
 *
 * EFFECTS
 * using octave notation: computes dest(destrow, :) - src(srcrow, :) and stores the result at
 * dest(destrow, :).
 * returns non-zero on error i.e., dest and src dimensions do not match.
 */ \
int \
PREFIX##MatrixSubRow(PREFIX##Matrix *dest, const PREFIX##Matrix *src, \
    size_t destrow, size_t srcrow, float rowscale) \
{ \
    if (!linalg_dims_eq(src, dest)) \
        return -1; \
    if (rowscale != ZERO) { \
        for (size_t i = 0; i < src->m; i++) \
            linalg_get_matrix_element(dest, destrow, i) -=  \
                linalg_get_matrix_element(src, srcrow, i) * rowscale; \
    } \
    return 0; \
} \
/*
 * REQUIRES
 * src and dest are valid
 *
 * MODIFIES
 * dest
 *
 * EFFECTS
 * copies elements from src over to dest.
 * dest must be able to store src.
 */ \
int \
PREFIX##MatrixCpy(PREFIX##Matrix *restrict dest, \
    const PREFIX##Matrix *restrict src) \
{ \
    if (!linalg_can_contain(dest, src)) \
        return -1; \
 \
    if (linalg_dims_eq(dest, src)) { \
        memcpy(dest->start, src->start, linalg_sizeof_matrix(src, T)); \
    } else { \
        for (size_t i = 0; i < src->n; i++) { \
            for (size_t j = 0; j < src->n; j++) { \
                linalg_get_matrix_element(dest, i, j) =  \
                    linalg_get_matrix_element(src, i, j); \
            } \
        } \
    } \
    return 0; \
} \
/*
 * REQUIRES
 * a and b are valid
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * computes a * b and stores the result into a new matrix
 * returns NULL on error
 */ \
PREFIX##Matrix * \
PREFIX##MatrixMultSimple(const PREFIX##Matrix *a, const PREFIX##Matrix *b) \
{ \
    PREFIX##Matrix *ret; \
    size_t ansRows; \
    size_t ansCols; \
 \
    if (a->m != b->n) \
        return NULL; \
    ansRows = a->n; \
    ansCols = b->m; \
    ret = new##PREFIX##Matrix(ansRows, ansCols); \
    if (!ret) \
        return NULL; \
    PREFIX##MatrixZeros(ret); \
    for (size_t i = 0; i < ansRows; i++) { \
        for (size_t j = 0; j < ansCols; j++) { \
            for (size_t k = 0; k < a->m; k++) { \
                linalg_get_matrix_element(ret, i, j) +=  \
                    linalg_get_matrix_element(a, i, k) \
                    * linalg_get_matrix_element(b, k, j); \
            } \
        } \
    } \
    return ret; \
} \
/*
 * REQUIRES
 * a is valid
 * dest and src corospond to rows in a 
 *
 * MODIFIES
 * a
 *
 * EFFECTS
 * swaps row dest with row src
 */ \
void \
PREFIX##MatrixSwapRows(PREFIX##Matrix *a, size_t dest, size_t src) \
{ \
    T tmp[a->m]; \
    \
    if (dest == src) \
        return; \
 \
    memcpy(tmp, linalg_addr_of_matrix_element(a, src, 0), sizeof(tmp)); \
    memcpy(linalg_addr_of_matrix_element(a, src, 0), \
        linalg_addr_of_matrix_element(a, dest, 0), sizeof(tmp)); \
    memcpy(linalg_addr_of_matrix_element(a, dest, 0), tmp, sizeof(tmp)); \
} \
void \
PREFIX##MatrixZeroUpperTriangle(PREFIX##Matrix *a) \
{ \
    for (size_t i = 0; i < a->n; i++) { \
        for (size_t j = i + 1; j < a->m; j++) { \
            linalg_get_matrix_element(a, i, j) = ZERO; \
        } \
    } \
}

/* call DEF_MATRIX_BASE first */
/* it does not make sense to use this on non-real types */
#define DEF_MATRIX_EXT(T, PREFIX, PRINTF_STR, ZERO, ONE, ABS_FUNC) \
PREFIX##Matrix *PREFIX##MatrixLUSolve(const PREFIX##Matrix *restrict b, \
    const PREFIX##Matrix *restrict l, const PREFIX##Matrix *restrict u, \
    const PREFIX##PermutationMatrix *restrict perm); \
void PREFIX##MatrixSwapRows(PREFIX##Matrix *a, size_t dest, size_t src); \
PREFIX##Matrix *PREFIX##MatrixFwdSub(const PREFIX##Matrix *restrict a, \
    const PREFIX##Matrix *restrict b); \
PREFIX##Matrix *PREFIX##MatrixBackSub(const PREFIX##Matrix *restrict a, \
    const PREFIX##Matrix *restrict b); \
int PREFIX##MatrixLUFactorize(const PREFIX##Matrix *restrict a, \
    PREFIX##Matrix *restrict l, PREFIX##Matrix *restrict u, \
    PREFIX##PermutationMatrix *restrict perm); \
\
/*
 * REQUIRES
 * a is a square matrix
 * all parameters are valid
 *
 * MODIFIES
 * l, u, perm
 *
 * EFFECTS
 * computes the LU factorization of a.
 * the results are stored in l and u.
 * the permutation matrix is stored in perm.
 * l and u must have the same dimensions as a.
 * returns non-zero on error
 */ \
int \
PREFIX##MatrixLUFactorize(const PREFIX##Matrix *restrict a, \
    PREFIX##Matrix *restrict l, PREFIX##Matrix *restrict u, \
    PREFIX##PermutationMatrix *restrict perm) \
{ \
    T rowScale; \
    T maxPivot; \
    size_t maxPivotRow; \
\
    if (!linalg_dims_eq(a, l) || !linalg_dims_eq(a, u)) \
        return -1; \
    PREFIX##MatrixCpy(u, a); \
    PREFIX##PermutationMatrixEye(perm); \
    for (size_t i = 0; i < a->n - 1; i++) { \
        /* pivot */ \
        maxPivot = ABS_FUNC(linalg_get_matrix_element(u, i, i)); \
        maxPivotRow = i; \
        for (size_t j = i + 1; j < a->n; j++) { \
            if (ABS_FUNC(linalg_get_matrix_element(u, j, i)) > maxPivot) { \
                maxPivot = ABS_FUNC(linalg_get_matrix_element(u, j, i)); \
                maxPivotRow = j; \
            } \
        } \
        if (maxPivotRow != i) { \
            PREFIX##MatrixSwapRows(u, i, maxPivotRow); \
            PREFIX##MatrixSwapRows(l, i, maxPivotRow); \
            PREFIX##PermutationMatrixSwapRows(perm, i, maxPivotRow); \
        } \
        for (size_t j = i + 1; j < a->n; j++) { \
            rowScale = linalg_get_matrix_element(u, j, i) \
                / linalg_get_matrix_element(u, i, i); \
            linalg_get_matrix_element(l, j, i) = rowScale; \
            PREFIX##MatrixSubRow(u, u, j, i, rowScale); \
        } \
    } \
    PREFIX##MatrixZeroLowerTriangle(u); \
    PREFIX##MatrixZeroUpperTriangle(l); \
    PREFIX##MatrixSetDiag(l, 1.0f); \
    return 0; \
} \
/*
 * REQUIRES
 * a is a square matrix.
 * a is a lower triangle matrix.
 * b is a column matrix.
 * only one solution exists.
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * solves ax = b using forward substitution
 * returns the result of x.
 * returns NULL on error.
 */ \
PREFIX##Matrix * \
PREFIX##MatrixFwdSub(const PREFIX##Matrix *restrict a, \
    const PREFIX##Matrix *restrict b) \
{ \
    PREFIX##Matrix *ret; \
    T sum; \
 \
    /* a must be square */ \
    ret = PREFIX##MatrixDup(b); \
    if (!ret) \
        goto error1; \
    for (size_t i = 0; i < a->n; i++) { \
        sum = 0; \
        for (size_t j = 0; j < i; j++) \
            sum += linalg_get_matrix_element(a, i, j) \
                * linalg_get_matrix_element(ret, j, 0); \
        linalg_get_matrix_element(ret, i, 0) = \
            (linalg_get_matrix_element(b, i, 0) - sum) \
            / linalg_get_matrix_element(a, i, i); \
    } \
    return ret; \
error1:; \
    return NULL; \
} \
/*
 * REQUIRES
 * a is a square matrix.
 * a is a upper triangle matrix.
 * b is a column matrix.
 * only one solution exists.
 *
 * MODIFIES
 * none
 *
 * EFFECTS
 * solves ax = b using back substitution
 * returns the result of x.
 * returns NULL on error.
 */ \
PREFIX##Matrix * \
PREFIX##MatrixBackSub(const PREFIX##Matrix *restrict a, \
    const PREFIX##Matrix *restrict b) \
{ \
    PREFIX##Matrix *ret; \
    T sum; \
    size_t k; \
 \
    /* a must be square */ \
    ret = PREFIX##MatrixDup(b); \
    if (!ret) \
        goto error1; \
    for (size_t i = 0; i < a->n; i++) { \
        k = a->n - i - 1; \
        sum = 0; \
        for (size_t j = k + 1; j < a->n; j++) \
            sum += linalg_get_matrix_element(a, k, j) * \
                linalg_get_matrix_element(ret, j, 0); \
        linalg_get_matrix_element(ret, k, 0) = \
            (linalg_get_matrix_element(b, k, 0) - sum) \
            / linalg_get_matrix_element(a, k, k); \
    } \
    return ret; \
error1:; \
    return NULL; \
} \
/*
 * REQUIRES
 * l, u, and perm corospond to the LU factorization of a.
 * b is a column matrix.
 * only one solution exists
 *
 * MODIFIES
 *
 * EFFECTS
 * solves ax = b given b, the LU factorization of a, and the permutation matrix
 * returns the result of x.
 * returns NULL on error.
 */ \
PREFIX##Matrix * \
PREFIX##MatrixLUSolve(const PREFIX##Matrix *restrict b, \
    const PREFIX##Matrix *restrict l, const PREFIX##Matrix *restrict u, \
    const PREFIX##PermutationMatrix *restrict perm) \
{ \
    PREFIX##Matrix *fperm; \
    PREFIX##Matrix *tmp; \
    PREFIX##Matrix *d; \
    PREFIX##Matrix *ret; \
 \
    /* 
     * computation outline using octave notation
     * d = l \ (perm * b)
     * x = u \ d
     */ \
    fperm = new##PREFIX##Matrix(perm->n, perm->m); \
    if (!fperm) \
        goto error1; \
    for (size_t i = 0; i < linalg_get_matrix_element_count(perm); i++) \
        fperm->start[i] = perm->start[i]; \
    tmp = PREFIX##MatrixMultSimple(fperm, b); \
    if (!tmp) \
        goto error2; \
    d = PREFIX##MatrixFwdSub(l, tmp); \
    if (!d) \
        goto error3; \
    ret = PREFIX##MatrixBackSub(u, d); \
    if (!ret) \
        goto error4; \
    free(fperm); \
    free(tmp); \
    free(d); \
    return ret; \
error4:; \
    free(d); \
error3:; \
    free(tmp); \
error2:; \
    free(fperm); \
error1:; \
    return NULL; \
}

#endif
