#ifndef ALIB_LINALG_H
#define ALIB_LINALG_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./utils.h"

#define xstr(s) str(s)
#define str(s) #s

#define linalg_dims_eq(A_PTR, B_PTR) \
    ((A_PTR) == (B_PTR) || ((A_PTR)->n == (B_PTR)->n && (A_PTR)->m == (B_PTR)->m))

#define linalg_addr_of_matrix_element(MATRIX_PTR, ROW, COL) \
    (void *)((MATRIX_PTR)->start + (ROW) * (MATRIX_PTR)->m + (COL))

#define linalg_get_matrix_element(MATRIX_PTR, ROW, COL) \
    (MATRIX_PTR)->start[(ROW) * (MATRIX_PTR)->m + (COL)]

#define linalg_get_matrix_element_count(MATRIX_PTR) \
    (MATRIX_PTR)->n * (MATRIX_PTR)->m \

#define linalg_sizeof_matrix(MATRIX_PTR, T) \
    sizeof(T) * linalg_get_matrix_element_count(MATRIX_PTR) \

#define linalg_sizeof_matrix_total(MATRIX_PTR) \
    (sizeof(MATRIX) + linalg_sizeof_matrix(MATRIX_PTR)) \

#define linalg_is_matrix_square(MATRIX_PTR) \
    ((MATRIX_PTR)->n == (MATRIX_PTR)->m)

/* can a contain b? */
#define linalg_can_contain(A_PTR, B_PTR) \
    ((A_PTR)->n >= (B_PTR)->n && (A_PTR)->m >= (B_PTR)->m)

#define DEF_MATRIX(T, PREFIX, PRINTF_STR, ZERO, ONE) \
    DEF_MATRIX_BASE(T, PREFIX, PRINTF_STR, ZERO, ONE) \
    DEF_MATRIX_BASE(uint8_t, PREFIX##Permutation, "%d ", 0, 1) \
    DEF_MATRIX_EXT(T, PREFIX, PRINTF_STR, ZERO, ONE)

#define DEF_MATRIX_BASE(T, PREFIX, PRINTF_STR, ZERO, ONE) \
typedef struct PREFIX##Matrix PREFIX##Matrix; \
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
    for (size_t i = 0; i < mat->n; i++) \
        linalg_get_matrix_element(mat, i, i) = (ONE); \
    return 0; \
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
 * using matlab notation: computes dest(destrow, :) - src(srcrow, :) and stores the result at
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
}

/* call DEF_MATRIX_BASE first */
#define DEF_MATRIX_EXT(T, PREFIX, PRINTF_STR, ZERO, ONE) \
\
/*
 * XXX DOES NOT WORK YET
 *
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
 \
    if (!linalg_dims_eq(a, l) || !linalg_dims_eq(a, u)) \
        return -1; \
    PREFIX##MatrixEye(l); \
    PREFIX##MatrixCpy(u, a); \
    for (size_t i = 0; i < a->n - 1; i++) { \
        for (size_t j = i + 1; j < a->n; j++) { \
            rowScale = linalg_get_matrix_element(u, j, i) \
                / linalg_get_matrix_element(u, i, i); \
            linalg_get_matrix_element(l, j, i) = rowScale; \
            PREFIX##MatrixSubRow(u, u, j, i, rowScale); \
        } \
    } \
    PREFIX##MatrixZeroLowerTriangle(u); \
    return 0; \
}

#endif
