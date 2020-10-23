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

#include <turbojpeg.h>
#include <libexif/exif-loader.h>
#include <assert.h>
#include "fsfiles.h"
#include "fd.h"
#include "webp/decode.h"

//https://cboard.cprogramming.com/c-programming/179287-reading-exif-data-png-file.html

static void imageCleanup(void* info)
{
    //LOG1(TAG_RESAMPLE, "deleteing image data");

    uint8_t* data = (uint8_t*)info;
    delete data;
}

bool FSFiles::loadThumbJpg(const uchar* data, unsigned int fsize,
                           int rw, int rh,
                           QImage& img)
{
    bool r = false;

    assert(data);

    int flags = 0;
    tjhandle tj = 0;
    tjscalingfactor scalingFactor = { 1, 8 }; // 1/8

    tjscalingfactor *scalingFactors;
    int numScalingFactors;
    scalingFactors = tjGetScalingFactors(&numScalingFactors);
    if (!scalingFactors) return false;


    flags |= TJFLAG_FASTUPSAMPLE;
    flags |= TJFLAG_FASTDCT;

    tj = tjInitDecompress();
    if (tj)
    {
        int w, h;
        int inSubsamp;
        int inColorspace;
        if (!tjDecompressHeader3(tj,
                                 data,
                                 (unsigned long)fsize,
                                 &w, &h,
                                 &inSubsamp,
                                 &inColorspace))
        {

            int dw, dh;

            if (rw > 0 && rh > 0)
            {
                // have a requested thumb size
                // find the best scaling factor that we support

                int leastw = w;
                
                for (int i = 0; i < numScalingFactors; ++i)
                {
                    // never upscale a thumb!
                    if (scalingFactors[i].num > scalingFactors[i].denom)
                        continue;

                    //LOG3(TAG_FSI, "scaling factor " << scalingFactors[i].num << "/" << scalingFactors[i].denom);

                    // find the least scaled width >= requested width
                    dw = TJSCALED(w, scalingFactors[i]);
                    if (dw >= rw && dw < leastw)
                    {
                        leastw = dw;
                        scalingFactor = scalingFactors[i];
                    }
                }
            }

            //LOG3(TAG_FSI, "thumb scaling factor " << scalingFactor.num << "/" << scalingFactor.denom);

            dw = TJSCALED(w, scalingFactor);
            dh = TJSCALED(h, scalingFactor);
            
            int dpixelFormat = TJPF_RGB;
            int pixelSize = 3;

            //int dpixelFormat = TJPF_BGRX;
            //int pixelSize = 4;

            //LOG3("decoding original size ", w << "x" << h);            
            //LOG3("decoding thumb size ", dw << "x" << dh);

            // round to 4x
            int stride = (dw*pixelSize + 3) & ~3;
            int dsize = dh*stride;
            uchar* ddata = new uchar[dsize];

            if (!tjDecompress2(tj,
                               data,
                               (unsigned long)fsize,
                               ddata,
                               dw,
                               stride,
                               dh,
                               dpixelFormat,
                               flags))
            {
                img = QImage(ddata, dw, dh,
                             (pixelSize == 3 ? QImage::Format_RGB888 :
                              QImage::Format_RGB32),
                             imageCleanup, ddata);

                ddata = 0; // drop
                r = true;
            }
            else
            {
                const char* why = tjGetErrorStr2(tj);
                LOG1(TAG_FSI, "unable to decompress " << why);
            }

            delete [] ddata;
                    
        }
        else
        {
            LOG1(TAG_FSI, "bad jpeg header");
        }
    }
    else
    {
        LOG1(TAG_FSI, "unable to init jpg");
    }
    
    return r;
}

bool FSFiles::loadExifThumb(const char* fname, QImage& img, int& orient)
{
    bool r = false;

    ExifLoader *l;

    /* Create an ExifLoader object to manage the EXIF loading process */
    l = exif_loader_new();
    if (l)
    {
        ExifData *ed;
        
        /* Load the EXIF data from the image file */
        exif_loader_write_file(l, fname);

        /* Get a pointer to the EXIF data */
        ed = exif_loader_get_data(l);

        /* The loader is no longer needed--free it */
        exif_loader_unref(l);

        if (ed)
        {
            //exif_data_dump(ed);

            ExifByteOrder bo = exif_data_get_byte_order(ed);
            ExifEntry* eo = exif_data_get_entry(ed, EXIF_TAG_ORIENTATION);
            if (eo)
            {
                /*
                  0th Row      0th Column
                  1  top          left side
                  2  top          right side
                  3  bottom       right side  // 180?
                  4  bottom       left side
                  5  left side    top
                  6  right side   top
                  7  right side   bottom
                  8  left side    bottom
                */

                orient = exif_get_short(eo->data, bo);

                if (orient > 1) LOG3(TAG_FSI, fname << " orientation " << orient);
            }
            
            /* Make sure the image had a thumbnail before trying to write it */
            if (ed->data && ed->size)
            {
                img.loadFromData(ed->data, ed->size);
                if (!img.isNull()) r = true;
                else
                {
                    LOG3(TAG_FSI, "thumb nail present, but failed for " << fname);
                }
            }
            else
            {
                //LOG3(TAG_FSI, "no thumb for " << fname);
            }

            /* Free the EXIF data */
            exif_data_unref(ed);
        }
    }

    return r;
}

uchar* FSFiles::_loadFile(const string& path, FD::Pos& fsize) const
{
    // NB: caller must delete return data
    uchar* data = 0;
    
    FD fd;
    if (fd.open(path.c_str()))
    {
        data = fd.readAll(&fsize);
    
        if (!data)
        {
            LOG1(TAG_FSI "unable to read file ", path);
        }
    }
    else
    {
        LOG1(TAG_FSI "can't find ", path);
    }
    
    return data;

}

QImage FSFiles::loadJPEG(const string& path, Name& id) const
{
    QImage img;
    FD::Pos fsize;

    tjhandle tj = 0;
    if (id._orient) tj  = tjInitTransform();
    else tj = tjInitDecompress();
    if (!tj)
    {
        LOG1("JPEG lib failed ", path);
        return img;
    }
    
    uchar* data = _loadFile(path, fsize);

    unsigned char *txBuf = NULL;
    unsigned long txSize = 0;
    int flags = 0;

    if (data)
    {
        if (id._orient)
        {
            tjtransform xform;            
            memset(&xform, 0, sizeof(tjtransform));
            switch (id._orient)
            {
            case 3: // 180
                xform.op = TJXOP_ROT180;
                break;
            case 6: // 90
                xform.op = TJXOP_ROT90;
                break;
            case 8: // -90
                xform.op = TJXOP_ROT270;
                break;
            }

            xform.options |= TJXOPT_TRIM;
            if (tjTransform(tj, data, (unsigned long)fsize, 1, &txBuf, &txSize,
                            &xform, flags) < 0)
            {
                LOG1("JPEG transform failed ", path);
                delete [] data;
                return img;
            }

            delete [] data;
            data = txBuf;
            fsize = (FD::Pos)txSize;
        }
        
        int w, h;
        int inSubsamp;
        int inColorspace;
        if (!tjDecompressHeader3(tj,
                                 data,
                                 (unsigned long)fsize,
                                 &w, &h,
                                 &inSubsamp,
                                 &inColorspace))
        {
            int dpixelFormat = TJPF_BGRX;
            int pixelSize = tjPixelSize[dpixelFormat];

            LOG3("decoding jpeg size ", w << "x" << h << " pixsize " << pixelSize);
                
            uchar* ddata = new uchar[w * h * pixelSize];

            if (!tjDecompress2(tj,
                               data,
                               (unsigned long)fsize,
                               ddata,
                               w,
                               0,
                               h,
                               dpixelFormat,
                               flags))
            {
                if (_filter)
                {
                    RawPixels rp;
                    rp._data = ddata;
                    rp._size = w*h*pixelSize;
                    rp._w = w;
                    rp._h = h;
                    rp._pixelSize = pixelSize;

                    (_filter)(&rp);
                }
                
                img = QImage(ddata, w, h,
                             (pixelSize == 3 ? QImage::Format_RGB888 :
                              QImage::Format_RGB32),
                             imageCleanup, ddata);
                    
                ddata = 0; // drop
            }
            else
            {
                const char* why = tjGetErrorStr2(tj);
                LOG1(TAG_FSI, "unable to decompress " << why);
            }
                
            delete [] ddata;
        }
        else
        {
            LOG1(TAG_FSI, "bad jpeg header");
        }

        if (txBuf) tjFree(txBuf); // data  == txbuf
        else delete [] data;
    }
    return img;
}

QImage FSFiles::loadWebp(const string& path) const
{
    QImage img;
    FD::Pos fsize;
    uchar* data = _loadFile(path, fsize);

    if (data)
    {
        int w, h;
        int v = WebPGetInfo(data, fsize, &w, &h);
        if (v)
        {
            LOG3(TAG_FSI " WEBP ", path << ", " << w << "x" << h);

            img = QImage(w, h, QImage::Format_ARGB32);
            if (!img.isNull())
            {
                uchar* bits = img.bits();
                int sz = img.sizeInBytes();
                int stride = img.bytesPerLine();
                uint8_t* b =  WebPDecodeBGRAInto(data, fsize,
                                                 bits,
                                                 sz,
                                                 stride);
                if (b)
                {
                    LOG3(TAG_FSI " decoded WEBP ", path);
                }
                else
                {
                    LOG1(TAG_FSI "WEBP decode error", path);
                    img = QImage(); // drop
                }
            }
        }
        else
        {
            LOG1(TAG_FSI " Unable to decode WEBP ", path);
        }
 
        delete data;
    }
    return img;
}
