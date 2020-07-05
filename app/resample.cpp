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

#include <math.h>
#include <algorithm>    
#include "resample.h"

//code adapted by David Olsen from Lanczos filtering article on wikipedia.org

// round to int
#define RINT(_x)        ((int)((_x) + 0.5f))

#if 0
static inline float Lanczos(float x, int Radius)
{
  if (x == 0.0f) return 1.0f;
  if (x <= -Radius || x >= Radius) return 0.0;
  float tmp = x * M_PI;
  return Radius * std::sin(tmp) * std::sin(tmp / Radius) / (tmp * tmp);
}

#else
static inline float  Lanczos(float x, int r)
{
    if (x == 0) return 1;
    if (x >= r) return 0;
    float tmp = x * 3.141592654f;
    float v;

    // small values of x need attention for sin(x)/x in single floats
    if (x >= -0.75f && x <= 0.75f)
    {
        tmp *= tmp;
        
        // approx always within 0.0005 better as r increases
        switch (r)
        {
        case 1:
            v = 1 - tmp/3 + 2*tmp*tmp/45;
            break;
        case 2:
            v = 1 - 5*tmp/24 + 91*tmp*tmp/5760;
            break;
        case 3:
            v = 1 - 5*tmp/27 + 14*tmp*tmp/1215;
            break;
        default:
            {
                // 1 + (-0.16666666666666666 - 1/(6.*r**2))*x**2 + (0.008333333333333333 + 1/(120.*r**4) + 1/(36.*r**2))*x**4

                float r2 = r*r;
                v = 1 - (1 + 1/r2)/6*tmp + (1 + 1/r2 + 10/r2/r2/3.0f)/120*tmp*tmp;
            }
        }
        return v;
    }
    
    switch (r)
    {
    case 1:
        v = sinf(tmp)/tmp;
        v *= v;
        break;
    case 2:
        {
            float cx = cosf(tmp/2);
            v = 4*cx*(1-cx*cx)/(tmp*tmp);
        }
        break;
    case 3:
        {
            // have 2tmp/3 < 2PI
            float cx = cosf(2*tmp/3);
            v = 1.5f*(cx - 2*cx*cx+1)/(tmp*tmp);
        }
        break;
    default:
        v = r * sinf(tmp) * sinf(tmp / r) / (tmp * tmp);
    }
    return v;
}
#endif

#define ABORT if (*abort) break

bool ResampleLanczos(const ImageData* src, ImageData* dst,
                     int filter, volatile bool* abort)
{
    float factor  = dst->width / (float)src->width;
    float scale   = std::min(factor, 1.0f);
    int FilterRadius = filter;

    if (filter < 1 ) FilterRadius = 1;
    else if (filter > 3) //automatically determine fastest filter setting
    {
        FilterRadius = 3;
        if (scale < 0.67f) FilterRadius = 2;
        if (scale <= 0.5f) FilterRadius = 1;        
    }
    float support = FilterRadius / scale; 

    /* 5 = room for rounding up in calculations of start, stop and support */    
    int sz = std::min((size_t)src->width, 5+(size_t)(2*support));
    float* contribution_x = new float[sz];

    uint32_t* temp = new uint32_t[src->height * dst->width];

    // assume that both have the same pixel size
    unsigned int pc = src->pixelStride;

    if (support <= 0.5f) { support = 0.5f; scale = 1.0f; }

    float point[4] = {0,0,0,0};
        
    for (int x = 0; x < dst->width; ++x)
    {
        ABORT;

        float center = (x + 0.5f) / factor;
        int start = RINT(center - support);
        if (start < 0) start = 0;

        int stop  = RINT(center + support);
        if (stop > src->width) stop = src->width;

        float density = 0;
        size_t nmax = stop - start;
        float s = start - center + 0.5f;
        
        uint8_t* p0 = (uint8_t*)src->data + start*pc;
        uint8_t* tp = (uint8_t*)temp;
        for (int y = 0; y < src->height; y++)
        {                        
            uint8_t* p = p0;
            for (size_t n = 0; n < nmax; ++n)
            {
                if (y == 0)
                { //only come up with the contribution list once per column.
                    contribution_x[n] = Lanczos (s * scale, FilterRadius);
                    density += contribution_x[n];
                    s++;
                }

                float cw = contribution_x[n];
                for (unsigned int c = 0; c < pc; c++)
                    point[c] += (*p++) * cw;
            }

            /* Normalize. Truncate to uint8 values. Place in temp array*/
            // NB: temp array is RGBA
            uint8_t* q = tp + 4*x;
            for (size_t c = 0; c < pc; c++)
            {
                int v = RINT(point[c]/density);
                point[c] = 0; //reset value for next loop
                
                // clamp
                if (v < 0) v = 0;
                else if (v > 255) v = 255;

                *q++ = (uint8_t)v;
            }

            p0 += src->lineStride;
            tp += dst->width*4;
        }
    }

    delete [] contribution_x;

    factor  = dst->height / (float)src->height;
    scale   = std::min(factor, 1.0f);

    if (filter < 1 ) FilterRadius = 1;
    else if (filter > 3) //automatically determine fastest filter setting
    {
        FilterRadius = 3;
        if (scale < 0.67f) FilterRadius = 2;
        if (scale <= 0.5f) FilterRadius = 1;        
    }

    support = FilterRadius / scale;

    sz = std::min((size_t)src->height, 5+(size_t)(2*support));
    float* contribution_y = new float[sz];

    if (support <= 0.5f) { support = 0.5f; scale = 1; }

    uint8_t* p0 = (uint8_t *)dst->data;
    for (int y = 0; y<dst->height; ++y)
    {
        ABORT;

        float center = (y + 0.5f) / factor;
        int start = RINT(center - support);
        if (start < 0) start = 0;

        int stop  = RINT(center + support);
        if (stop > src->height) stop = src->height;

        float density = 0;
        size_t nmax = stop-start;
        float s = start - center + 0.5f;

        uint8_t* t0 = (uint8_t *)&temp[start*dst->width];
        uint8_t* q = p0;
        for (int x=0; x<dst->width; x++)
        {    
            uint8_t* p = t0 + x*4;
            for (size_t n=0; n<nmax; ++n)
            {
                if (x == 0)
                {
                    contribution_y[n] = Lanczos(s * scale, FilterRadius);
                    density += contribution_y[n];
                    s++;
                }

                float cw = contribution_y[n];
                for (unsigned int c = 0; c < pc; c++)
                    point[c] += p[c] * cw;

                p += dst->width*4;
            }

            for (size_t c = 0; c < pc; c++)
            {
                int v = RINT(point[c]/density);
                point[c] = 0;

                if (v < 0) v = 0;
                else if (v > 255) v = 255;

                *q++ = (uint8_t)v;
            }
        }
        p0 += dst->lineStride;
    }

    delete [] contribution_y;
    delete [] temp;
    return true;
}
