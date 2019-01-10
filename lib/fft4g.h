#ifndef FFT4G_H
#define FFT4G_H

/* header for fft4g.c */

#include <math.h>
#include <stdio.h>
#define MAX(x,y) ((x) > (y) ? (x) : (y))

/* random number generator, 0 <= RND < 1 */
#define RND(p) ((*(p) = (*(p) * 7141 + 54773) % 259200) * (1.0 / 259200.0))

#ifndef NMAX
#define NMAX 8192
#define NMAXSQRT 64
#endif

void cdft(int n, int isgn, double *a, int *ip, double *w);
void rdft(int, int, double *, int *, double *);
void ddct(int, int, double *, int *, double *);
void ddst(int, int, double *, int *, double *);
void dfct(int, double *, double *, int *, double *);
void dfst(int, double *, double *, int *, double *);
void putdata(int nini, int nend, double *a);
double errorcheck(int nini, int nend, double scale, double *a);

#endif // FFT4G_H
