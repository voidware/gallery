/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2020 Voidware Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License (LGPL) as published
 * by the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * 
 * contact@voidware.com
 */

#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "resample.h"
#include "logged.h"

#define EMIT_PNMxx

typedef float F;

#ifdef BUILD_LUT
// these functions build the tables that are included in lut.c
static F srgb_to_linear(F v_srgb)
{
	if(v_srgb<=0.04045f) return v_srgb/12.92f;
    return powf( (v_srgb+0.055f)/(1.055f), 2.4f);
}

static unsigned int linear_to_srgb(F v)
{
    unsigned int vi;

    assert(v < 525000);
  
    // round to nearest 0-255
	if (v <= 0.0031308f) vi = 12.92f*v + 0.5f;
    else
    {
        vi = (int)(1.055f*powf(v,1/2.4f) - 0.055f + 0.5f);
    }
    
    assert(vi <= 255);
    return vi;
}
#endif

// load prebuilt tables
#include "lut.c"

#ifdef EMIT_PNM
void writePNM(ImageData* img)
{
    FILE* fp = fopen("out.pnm", "wb");
    fprintf(fp, "P3\n%d %d\n255\n", img->width, img->height);
    for (int y = 0; y < img->height; ++y)
    {
        for (int x = 0; x < img->width; ++x)
        {
            uint8_t* dp = img->data + y*img->lineStride + x*img->pixelStride;
            if (x) fprintf(fp, " ");
            fprintf(fp, "%d %d %d", dp[2], dp[1], dp[0]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    printf("wrote PNM %d %d\n", img->width, img->height);
}
#endif

#define ABORT  if (*abort) break

bool ResampleMix(const ImageData* src, ImageData* dst, volatile bool* abort)
{
    int dw = src->width - dst->width;
    int dh = src->height - dst->height;

    if (!dw && !dh)
    {
        // handle case where scale is 1
        memcpy(dst->data, src->data, dst->lineStride*dst->height);
        return true;
    }

    // downsample only!
    if (dw < 0 || dh < 0)
    {
        LOG4("ResampleMix rejecting upsize ", dw);
        return false;
    }

    // same shape needed
    assert(src->pixelStride == dst->pixelStride);
    
    // make two accumulator rows
    int rowsz = dst->width*dst->pixelStride; 
    F* rowmem = new F[rowsz*2];

    F* row1 = rowmem;
    F* row2 = rowmem + rowsz;

    memset(row1, 0, rowsz*sizeof(F));

    F sx = ((F)dst->width)/src->width;
    F sy = ((F)dst->height)/src->height;

    int y2 = 0;
    int dy = 0;
    int incy;
    F dyp2 = 0;

    uint8_t* dsrc = src->data;
    uint8_t* ddst = dst->data;
    for (int y = 0; y < src->height; ++y)
    {
        F y1p, y2p;

        ABORT;

        y1p = sy;
        y2p = 0;

        dy += dst->height;
        incy = (dy >= src->height);
        if (incy)
        {
            dy -= src->height;
            
            incy = (++y2 < dst->height);

            if (incy)
            {
                dyp2 += 1;

                // accumulating also into row2
                memset(row2, 0, rowsz*sizeof(F));
                
                //y1p = F(y2*src->height - y*dst->height)/src->height;
                y1p = dyp2;
                y2p = sy - y1p;

                assert(y1p > 0);
            }
        }

        dyp2 -= sy;

        F* rp1 = row1;
        F* rp2 = row2;

        int x2 = 0;
        uint8_t* ds = dsrc;
        int dx = 0;
        F dxp2 = 0;

        for (int x = 0; x < src->width; ++x)
        {
            int drp = 0;
            dx += dst->width;
            if (dx >= src->width)
            {
                dx -= src->width;
                
                if (++x2 < dst->width)
                {
                    drp = dst->pixelStride;
                    dxp2 += 1;
                }
            }

            F x1p;
            F x2p;

            if (drp)
            {
                // this is more accurate but slower
                //F x1p = F(x2*src->width - x*dst->width)/src->width;

                x1p = dxp2;
                x2p = sx - x1p;

                //assert(x1p >= 0);
                //assert(x2p >= 0);
            }
            else
            {
                x1p = sx;
                x2p = 0;
            }
            
            dxp2 -= sx;
            
            // XX ignore 4th component!
#define ACC(_r)         \
            (_r)[0] += linear_lut[ds[0]]*f; \
            (_r)[1] += linear_lut[ds[1]]*f; \
            (_r)[2] += linear_lut[ds[2]]*f
            
            F f;
            f = x1p*y1p;
            ACC(rp1);

            if (drp)
            {
                rp1 += drp;
                assert(x2 < dst->width);
                f = x2p*y1p;
                ACC(rp1);
            }

            if (incy)
            {
                // accumulating into row2 as well.
                assert(y2 < dst->height);
                f = x1p*y2p;
                ACC(rp2);
                rp2 += drp;

                if (drp)
                {
                    f = x2p*y2p;
                    ACC(rp2);
                }
            }

            ds += src->pixelStride;
        }


#define ACC1(_i) dp[_i] = srgb_lut[(int)rp[_i]]
       
        // finished a row and accumulated into row1 and possibly row2
        if (incy)
        {
            // row1 is done
            F* rp = row1;
            uint8_t* dp = ddst;

            int x = dst->width;
            while (x)
            {
                --x;
                ACC1(0);
                ACC1(1);
                ACC1(2);
                dp[3] = 0xff; // alpha

                rp += dst->pixelStride;
                dp += dst->pixelStride;
            }

            // swap rows
            F* tp = row1;
            row1 = row2;
            row2 = tp;

            ddst += dst->lineStride;
        }

        dsrc += src->lineStride;
    }

    delete [] rowmem;

#ifdef EMIT_PNM
    writePNM(dst);
#endif
    
    return true;
}

#ifdef BUILD_LUT

#include <stdio.h>

static void make_srgb_to_linear_table()
{
    printf("static const F linear_lut[256] = \n{\n");
    for (int i = 0; i < 256; ++i)
    {
        printf("    %f,\n", srgb_to_linear(i));
    }
    printf("};\n");

    printf("\nstatic const unsigned char srgb_lut[] = \n{\n");
    for (int i = 0; i < 525000; ++i)
    {
        printf("    %d,\n", linear_to_srgb(i));
    }
    printf("};\n");
}

int main()
{
    make_srgb_to_linear_table();
    return 0;
}

#endif

