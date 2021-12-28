#include <math.h>

#include "../src/linalg.h"

DEF_MATRIX_REAL(float, F, "%-12e ", 0.0f, 1.0f, fabsf)


// /*
//  * REQUIRES
//  * a is a square matrix.
//  * a is a lower triangle matrix.
//  * b is a column matrix.
//  * only one solution exists.
//  *
//  * MODIFIES
//  * none
//  *
//  * EFFECTS
//  * solves ax = b using forward substitution
//  * returns the result of x.
//  * returns NULL on error.
//  */
// FMatrix *
// fwdSub(const FMatrix *restrict a, const FMatrix *restrict b)
// {
//     FMatrix *ret;
//     float sum;
// 
//     /* a must be square */
//     ret = FMatrixDup(b);
//     if (!ret)
//         goto error1;
//     for (size_t i = 0; i < a->n; i++) {
//         sum = 0;
//         for (size_t j = 0; j < i; j++)
//             sum += linalg_get_matrix_element(a, i, j) 
//                 * linalg_get_matrix_element(ret, j, 0);
//         linalg_get_matrix_element(ret, i, 0) = 
//             (linalg_get_matrix_element(b, i, 0) - sum) 
//             / linalg_get_matrix_element(a, i, i);
//     }
//     return ret;
// error1:;
//     return NULL;
// }
// 
// /*
//  * REQUIRES
//  * a is a square matrix.
//  * a is a upper triangle matrix.
//  * b is a column matrix.
//  * only one solution exists.
//  *
//  * MODIFIES
//  * none
//  *
//  * EFFECTS
//  * solves ax = b using back substitution
//  * returns the result of x.
//  * returns NULL on error.
//  */
// FMatrix *
// backSub(const FMatrix *restrict a, const FMatrix *restrict b)
// {
//     FMatrix *ret;
//     float sum;
//     size_t k;
// 
//     /* a must be square */
//     ret = FMatrixDup(b);
//     if (!ret)
//         goto error1;
//     for (size_t i = 0; i < a->n; i++) {
//         k = a->n - i - 1;
//         sum = 0;
//         for (size_t j = k + 1; j < a->n; j++)
//             sum += linalg_get_matrix_element(a, k, j) * 
//                 linalg_get_matrix_element(ret, j, 0);
//         linalg_get_matrix_element(ret, k, 0) = 
//             (linalg_get_matrix_element(b, k, 0) - sum) 
//             / linalg_get_matrix_element(a, k, k);
//     }
//     return ret;
// error1:;
//     return NULL;
// }
// 
// /*
//  * REQUIRES
//  * l, u, and perm corospond to the LU factorization of a.
//  * b is a column matrix.
//  * only one solution exists
//  *
//  * MODIFIES
//  *
//  * EFFECTS
//  * solves ax = b given b, the LU factorization of a, and the permutation matrix
//  * returns the result of x.
//  * returns NULL on error.
//  */
// FMatrix *
// FMatrixLUSolve(const FMatrix *restrict b,
//     const FMatrix *restrict l, const FMatrix *restrict u,
//     const FPermutationMatrix *restrict perm)
// {
//     FMatrix *fperm;
//     FMatrix *tmp;
//     FMatrix *d;
//     FMatrix *ret;
// 
//     /* 
//      * computation outline using octave notation
//      * d = l \ (perm * b)
//      * x = u \ d
//      */
//     fperm = newFMatrix(perm->n, perm->m);
//     if (!fperm)
//         goto error1;
//     for (size_t i = 0; i < linalg_get_matrix_element_count(perm); i++)
//         fperm->start[i] = perm->start[i];
//     tmp = FMatrixMultSimple(fperm, b);
//     if (!tmp)
//         goto error2;
//     d = fwdSub(l, tmp);
//     if (!d)
//         goto error3;
//     ret = backSub(u, d);
//     if (!ret)
//         goto error4;
//     free(fperm);
//     free(tmp);
//     free(d);
//     return ret;
// error4:;
//     free(d);
// error3:;
//     free(tmp);
// error2:;
//     free(fperm);
// error1:;
//     return NULL;
// }

int
main()
{
    FMatrix *mat;
    FMatrix *l;
    FMatrix *u;
    FMatrix *b;
    FMatrix *x;
    FPermutationMatrix *p;
    const float data[] = {
        0, 1, 6,
        3, 5, 7,
        4, 9, 2,
    };
    const float data2[] = {
        1, 2, 3
    };

    mat = newFMatrix(3, 3);
    l = newFMatrix(3, 3);
    u = newFMatrix(3, 3);
    p = newFPermutationMatrix(3, 3);
    b = newFMatrix(3, 1);
    x = newFMatrix(3, 1);
    memcpy(mat->start, data, sizeof(data));
    memcpy(b->start, data2, sizeof(data2));
    FMatrixLUFactorize(mat, l, u, p);
    FMatrixLUSolve(b, l, u, p);
    free(mat);
    free(l);
    free(u);
    free(p);
    free(x);
    free(b);
    return 0;
}
