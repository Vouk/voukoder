#pragma once

#include <stdint.h>
#include <math.h>

void rat_approx(double f, int64_t md, int64_t* num, int64_t* denom)
{
    int64_t a, h[3] = { 0, 1, 0 }, k[3] = { 1, 0, 0 };
    int64_t x, d, n = 1;
    int i, neg = 0;

    if (md <= 1) { *denom = 1; *num = (int64_t)f; return; }

    if (f < 0) { neg = 1; f = -f; }

    while (f != floor(f)) { n <<= 1; f *= 2; }
    d = (int64_t)f;

    for (i = 0; i < 64; i++) {
        a = n ? d / n : 0;
        if (i && !a) break;

        x = d; d = n; n = x % n;

        x = a;
        if (k[1] * a + k[0] >= md) {
            x = (md - k[0]) / k[1];
            if (x * 2 >= a || k[1] >= md)
                i = 65;
            else
                break;
        }

        h[2] = x * h[1] + h[0]; h[0] = h[1]; h[1] = h[2];
        k[2] = x * k[1] + k[0]; k[0] = k[1]; k[1] = k[2];
    }
    *denom = k[1];
    *num = neg ? -h[1] : h[1];
}