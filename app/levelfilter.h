#pragma once

#include "filter.h"
#include "logged.h"

#define GETR(_p) (_p)[0]
#define GETG(_p) (_p)[1]
#define GETB(_p) (_p)[2]

#define GETI(_p) (GETR(_p) + GETG(_p) + GETB(_p))


static inline void quantiles_u8i(RawPixels* pix,
                                 size_t nb_min, size_t nb_max,
                                 int* ptr_min, int* ptr_max)
{
    size_t h_size = 256;
    size_t histo[256];
    size_t i;

    size_t size = pix->_w * pix->_h;
    uchar* dp = pix->_data;
    
    /* make a cumulative histogram */
    memset(histo, 0, h_size * sizeof(size_t));
    for (i = 0; i < size; i++)
    {
        int iv = GETI(dp)/3;
        histo[iv] += 1;
        dp += pix->_pixelSize;
    }
    
    for (i = 1; i < h_size; i++) histo[i] += histo[i - 1];

    /* get the new min/max */

    /* simple forward traversal of the cumulative histogram */
    /* search the first value > nb_min */
    i = 0;
    while (i < h_size && histo[i] <= nb_min) i++;
    
    /* the corresponding histogram value is the current cell position */
    *ptr_min = i;

    /* simple backward traversal of the cumulative histogram */
    /* search the first value <= size - nb_max */
    i = h_size - 1;
    /* i is unsigned, we check i<h_size instead of i>=0 */
    while (i < h_size && histo[i] > (size - nb_max)) i--;
    
    /*
     * if we are not at the end of the histogram,
     * get to the next cell,
     * the last (backward) value > size - nb_max
     */
    if (i < h_size - 1) i++;
    *ptr_max = i;
}

static inline void colorbalance(RawPixels* pix,
                                size_t nb_min, size_t nb_max)
{
    int i;
    int min, max;

    int size = pix->_w * pix->_h;
    uchar* dp;
    
    /* compute min and max */
    if (nb_min || nb_max)
    {
        quantiles_u8i(pix, nb_min, nb_max, &min, &max);

        // consistent with overall minmax
        min *= 3;
        max *= 3;
    }
    else
    {
        // find overall min and max
        min = 255*3;
        max = 0;
        dp = pix->_data;
        for (i = 0; i < size; i++)
        {
            int iv = GETI(dp);
            if (iv < min) min = iv;
            if (iv > max) max = iv;
            dp += pix->_pixelSize;
        }
    }

    int dm = max - min;

    dp = pix->_data;

    if (dm > 0)
    {
        LOG3("Scaling levels by ", 3*255.0/dm);
        for (i = 0; i < size; i++, dp += pix->_pixelSize)
        {
            int s = GETI(dp);
            if (!s) continue;
        
            double ss = 3*255.0*(1.0 - ((double)min)/s)/dm;

            /* if m * s > 1, a projection is needed by adjusting s */
            //if (m*ss > 1) ss = 1.0/m;

            int r = GETR(dp);
            int g = GETG(dp);
            int b = GETB(dp);

            // find max component
            int m = r;
            if (g > m) m = g;
            if (b > m) m = b;

            double mf = 255.0/m;
            if (ss > mf) ss = mf; // also prevents 255 overflow

            // only needed when using non-zero nb_min and max
            if (ss < 0) ss = 0;

            GETR(dp) *= ss;
            GETG(dp) *= ss;
            GETB(dp) *= ss;
        }
    }
}

static inline bool levelFilter(RawPixels* rp)
{
    LOG1("Performing Level filter size ", rp->_w << "x" << rp->_h);

#if 0
    // this version takes 1% of pixels to min/max
    int size = rp->_w * rp->_h;
    int n = size/100; // 1%
    colorbalance(rp, n, n);
#else
    colorbalance(rp, 0, 0);
#endif
    return true;
}
