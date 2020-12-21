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

#include "fsfiles.h"
#include "fd.h"
#include "webp/decode.h"

QImage FSFiles::loadWebp(const string& path, const Name& id, bool thumb) const
{
    QImage img;
    FD::Pos fsize;
    uchar* data = _loadFile(path, fsize);

    if (data)
    {
        WebPDecoderConfig config;

        bool v;

        v = WebPInitDecoderConfig(&config) != 0;
        
        if (v)
        {
            v = WebPGetFeatures(data, fsize, &config.input) == VP8_STATUS_OK;
        }
        
       if (v)
        {
            config.output.colorspace = MODE_BGRA;

            int w = config.input.width;
            int h = config.input.height;
            
            if (thumb)
            {
                config.options.no_fancy_upsampling = 1;
                config.options.bypass_filtering = 1;

                if (w > 128) w = 128;
                h = (h*w + (config.input.width - 1))/config.input.width;

                config.options.use_scaling = 1;
                config.options.scaled_width = w;
                config.options.scaled_height = h;
            }
            else
            {
                config.options.use_threads = 1;
            }

            LOG3(TAG_FSI " WEBP ", path << ", " << w << "x" << h << (thumb ? " THUMB" : ""));
            
            img = QImage(w, h, QImage::Format_ARGB32);
            if (!img.isNull())
            {
                uchar* bits = img.bits();
                int sz = img.sizeInBytes();
                int stride = img.bytesPerLine();

                config.output.u.RGBA.rgba = (uint8_t*) bits;
                config.output.u.RGBA.stride = stride;
                config.output.u.RGBA.size = sz;
                config.output.is_external_memory = 1;
                
#if 1
                v = WebPDecode(data, fsize, &config) == VP8_STATUS_OK;
#else

                // incremental decode.
                // works but decodes the picture line by line
                // rather than progressively
                
                WebPIDecoder* const idec = WebPIDecode(0, 0, &config); 
                v = idec != 0;
                if (v)
                {
                    size_t bytes_remaining = fsize;
                    uchar* dp = data;
                    while (bytes_remaining > 0) 
                    {
                        size_t r = 1024*1024;
                        if (r > bytes_remaining) r = bytes_remaining;

                        VP8StatusCode status = WebPIAppend(idec, dp, r);
                        if (status == VP8_STATUS_OK || status == VP8_STATUS_SUSPENDED) 
                        {
                            bytes_remaining -= r;
                            dp += r;
                            LOG3(TAG_FSI " decoding WEBP ", path << " " << bytes_remaining);
                        }
                        else
                        {
                            v = false;
                            break;
                        }
                    }
                    WebPIDelete(idec);
                }
#endif

                if (v)
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

#if 0
QImage FSFiles::loadWebpOld(const string& path) const
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
#endif
