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
#include <string>
#include "fsfiles.h"
#include "fd.h"
#include "png.h"

struct PNGState
{
    PNGState(const FSFiles* host, const std::string& name)
        : _host(host), _name(name) {}

    ~PNGState()
    {
        delete _data;
    }

    void alloc()
    {
        int psize = _channels;
        _stride = psize * _w;
        _size = _stride * _h;
        _data = new uchar[_size];
    }
    
    const FSFiles*      _host;
    std::string         _name;
    size_t              _w;
    size_t              _h;
    int                 _channels;
    int                 _color_type;
    int                 _passCount;
    int                 _interlace_type;
    bool                _thumb = false;

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

static void png_read_data(png_structp png_ptr,
                          png_bytep data,
                          png_size_t length)
{
    FD* fd = (FD*)png_get_io_ptr(png_ptr);
    assert(fd);

    if (!fd->read(data, length))
    {
        png_error(png_ptr, "Read Error");
    }
}

static void setState(PNGState* ps, png_structp png_ptr, png_infop info_ptr)
{
    assert(ps);
    
     //init png reading
    png_set_expand(png_ptr);
    png_set_scale_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);

    ps->_color_type = png_get_color_type(png_ptr, info_ptr);
    
    if (ps->_color_type == PNG_COLOR_TYPE_RGB ||
        ps->_color_type == PNG_COLOR_TYPE_GRAY)
    {
        LOG4(TAG_FSI, "png adding alpha filler");
        png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    }

    ps->_interlace_type = png_get_interlace_type(png_ptr, info_ptr);

    png_set_bgr(png_ptr);

    if (!ps->_thumb)
    {
        // if interlaces and not thumb, let libpng combine sub-images
        ps->_passCount = png_set_interlace_handling(png_ptr);
    }
    
    png_read_update_info(png_ptr, info_ptr);

    //init read frame struct
    ps->_w = png_get_image_width(png_ptr, info_ptr);
    ps->_h = png_get_image_height(png_ptr, info_ptr);
    ps->_channels = png_get_channels(png_ptr, info_ptr);
    ps->_color_type = png_get_color_type(png_ptr, info_ptr);

    LOG4(TAG_FSI "png_info_fn; ", ps->_name << " " << ps->_w << "x" << ps->_h << " chans:" << ps->_channels);

    assert(ps->_channels == 3 || ps->_channels == 4);
}

static void png_info_fn(png_structp png_ptr, png_infop info_ptr)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    setState(ps, png_ptr, info_ptr);
    ps->alloc();
}

static void png_row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    assert(ps);
    //LOG3(TAG_FSI "png_row_fn; row ", row_num);

    if (new_row)
    {
        assert(row_num < ps->_h);
        
        uchar* rp = ps->_data + row_num*ps->_stride;
        png_progressive_combine_row(png_ptr,
                                    rp,
                                    new_row);
    }
}

static void png_end_fn(png_structp png_ptr, png_infop info_ptr)
{
    PNGState* ps = (PNGState*)png_get_progressive_ptr(png_ptr);
    assert(ps);
    //LOG2(TAG_FSI, "png_end_fn");

    ps->_complete = true;
    
}

QImage FSFiles::loadPNGProgressive(const string& path, const Name& id) const
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

            ok = _png != 0;
            
            if (ok)
            {
                _info = png_create_info_struct(_png);
                ok = _info != 0;
            }

            if (!ok)
            {
                LOG1(TAG_FSI "png, init failed ", path);
            }
        }

        if (ok)
        {
            //set png jump position
            if (!setjmp(png_jmpbuf(_png)))
            {
                // dont compute and test CRC
                png_set_crc_action(_png, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
                
                /* Turn off ADLER32 checking while reading */
                png_set_option(_png, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
            
                // read here
                PNGState ps(this, path);

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

QImage FSFiles::loadPNG(const string& path,
                        const Name& id,
                        bool thumb) const
{
    QImage img;

    png_structp _png = 0;
    png_infop _info = 0;

    FD fd = _openFile(path.c_str());

    bool ok = fd;

    if (ok)
    {
        _png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                      NULL,
                                      user_error_fn,
                                      user_warning_fn);
        ok = _png != 0;

        if (ok)
        {
            _info = png_create_info_struct(_png);
            ok = _info != 0;
        }
        
        if (!ok)
        {
            LOG1(TAG_FSI "png, init failed ", path);
        }
    }

    if (ok)
    {
        //set png jump position
        if (!setjmp(png_jmpbuf(_png)))
        {
            // dont compute and test CRC
            png_set_crc_action(_png, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);

            /* Turn off ADLER32 checking while reading */
            png_set_option(_png, PNG_IGNORE_ADLER32, PNG_OPTION_ON);
            
            png_set_read_fn(_png, (png_voidp)&fd, png_read_data);

            png_read_info(_png, _info);

            PNGState ps(this, path);
            ps._thumb = thumb;
            setState(&ps, _png, _info);

            switch (ps._interlace_type)
            {
            case PNG_INTERLACE_NONE:
                {
                    if (thumb)
                    {
                        // no interlace but want thumb, can't do that!
                        ok = false;
                        break;
                    }
                    
                    ps.alloc();
                    for (size_t i = 0; i < ps._h; ++i)
                    {
                        uchar* p = ps._data + i*ps._stride;
                        png_read_row(_png, p, 0);
                    }
                }
                break;
            case PNG_INTERLACE_ADAM7:
                {
                    if (!thumb)
                    {
                        ps.alloc();
                        for (int j = 0; j < PNG_INTERLACE_ADAM7_PASSES; ++j)
                        {
                            uchar* p = ps._data;
                            for (size_t i = 0; i < ps._h; ++i)
                            {
                                png_read_row(_png, p, 0);
                                p += ps._stride;
                            }
                        }
                    }
                    else
                    {
                        for (int j = 0; j < PNG_INTERLACE_ADAM7_PASSES; ++j)
                        {
                            png_uint_32 pw = PNG_PASS_COLS(ps._w, j);
                            png_uint_32 ph = PNG_PASS_ROWS(ps._h, j);

                            //LOG3("pass ", j << " h=" << ph << ", w=" << pw); // << " x=" << x << " y =" << y);
                            
                            if (pw && ph)
                            {
                                // change size to first interlace

                                // seems we need more space to read row
                                int rz = png_get_rowbytes(_png, _info);
                                uchar* row = new uchar[rz];

                                ps._w = pw;
                                ps._h = ph;
                                ps.alloc();
                                uchar* p = ps._data;
                                for (size_t i = 0; i < ph; ++i)
                                {
                                    png_read_row(_png, row, 0);
                                    memcpy(p, row, ps._stride);
                                    p += ps._stride;
                                }

                                delete [] row;

                                break;
                            }
                        }
                    }
                }
                break;
            default:
               png_error(_png, "invalid interlace type");
               break;
            }

            if (ok)
            {
                png_read_end(_png, 0);

                RawPixels rp;
                rp._data = ps._data;
                rp._w = ps._w;
                rp._h = ps._h;
                rp._pixelSize = ps._channels;
                rp.init();

                img = makeImage(&rp, thumb ? 0 : &id);
                ps._data = 0; // drop
            }
        }
    }

    if (_png)
    {
        png_destroy_read_struct(&_png, &_info, NULL);
    }
    
    return img;
}


