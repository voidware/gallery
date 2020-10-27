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
#include "fsfiles.h"
#include "fd.h"
#include "png.h"

struct PNGState
{
    PNGState(const FSFiles* host) : _host(host) {}

    ~PNGState()
    {
        delete _data;
    }
    
    const FSFiles*      _host;
    size_t              _w;
    size_t              _h;
    int                 _channels;
    int                 _color_type;

    size_t              _stride;
    size_t              _size;
    uchar*              _data = 0;
    bool                _complete = false;
};

static void user_error_fn(png_structp png_ptr,
                          png_const_charp error_msg)
{
    LOG1(TAG_FSI "PNG Error: ", error_msg);
}

static void user_warning_fn(png_structp png_ptr,
                            png_const_charp warning_msg)
{
    LOG2(TAG_FSI "PNG warning: ", warning_msg);
}

static void png_info_fn(png_structp png_ptr, png_infop info_ptr)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    assert(ps);
    
     //init png reading
    png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);

    ps->_color_type = png_get_color_type(png_ptr, info_ptr);
    
    if (ps->_color_type == PNG_COLOR_TYPE_RGB ||
        ps->_color_type == PNG_COLOR_TYPE_GRAY)
    {
        LOG3(TAG_FSI, "png adding alpha filler");
        png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    }

    //png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    png_set_bgr(png_ptr);
    png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);

    //init read frame struct
    ps->_w = png_get_image_width(png_ptr, info_ptr);
    ps->_h = png_get_image_height(png_ptr, info_ptr);
    ps->_channels = png_get_channels(png_ptr, info_ptr);
    ps->_color_type = png_get_color_type(png_ptr, info_ptr);

    LOG4(TAG_FSI "png_info_fn; ", ps->_w << "x" << ps->_h << " chans:" << ps->_channels);

    assert(ps->_channels == 4);

    ps->_stride = ps->_channels * ps->_w;
    ps->_size = ps->_stride * ps->_h;
    ps->_data = new uchar[ps->_size];
}

static void png_row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    assert(ps);
    assert(!pass);
    //LOG3(TAG_FSI "png_row_fn; row ", row_num);

    assert(row_num < ps->_h);
    
    uchar* rp = ps->_data + row_num*ps->_stride;
    png_progressive_combine_row(png_ptr,
                                rp,
                                new_row);        
}

static void png_end_fn(png_structp png_ptr, png_infop info_ptr)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    assert(ps);
    //LOG2(TAG_FSI, "png_end_fn");

    ps->_complete = true;
    
}

QImage FSFiles::loadPNG(const string& path, const Name& id) const
{
    QImage img;
    FD::Pos fsize;
    uchar* data = _loadFile(path, fsize);

    png_structp _png = 0;
    png_infop _info = 0;

    if (data)
    {
        bool ok = !png_sig_cmp((png_const_bytep)data, 0, 8);

        if (!ok)
        {
            LOG1(TAG_FSI "png, wrong signature in ", path);
        }
        
        if (ok)
        {
            //init png structs
            _png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                          NULL,
                                          user_error_fn,
                                          user_warning_fn);

            if(!_png)
            {
                LOG1(TAG_FSI "png, init failed ", path);
                ok = false;
            }
        }

        if (ok)
        {
            _info = png_create_info_struct(_png);
            if(!_info)
            {
                LOG1(TAG_FSI "png, init failed ", path);
                ok = false;
            }
        }

        if (ok)
        {
            //set png jump position
            if (!setjmp(png_jmpbuf(_png)))
            {
                // read here

                PNGState ps(this);

                png_set_progressive_read_fn(_png,
                                            (png_voidp)&ps,
                                            png_info_fn,
                                            png_row_fn,
                                            png_end_fn);

                
                png_process_data(_png, _info, (png_bytep)data, fsize);

                if (ps._complete)
                {
                    LOG4(TAG_FSI "PNG complete ", path);

                    RawPixels rp;
                    rp._data = ps._data;
                    rp._w = ps._w;
                    rp._h = ps._h;
                    rp._pixelSize = ps._channels;
                    rp.init();
                    img = makeImage(&rp, &id);
                    ps._data = 0; // drop
                }
            }
        }

        if (_png)
        {
            png_destroy_read_struct(&_png, &_info, NULL);
        }
        
        delete data;
    }
    return img;
}
