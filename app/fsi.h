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

#pragma once

#include <QImage>
#include <string>
#include <vector>
#include "logged.h"
#include "qdefs.h"
#include "cutils.h"
#include "strutils.h"
#include "levelfilter.h"

#define TAG_FSI  "FSI, "

struct FSITraits
{
    enum ImgType
    {
        img_void,
        img_jpg,
        img_png,
        img_webp,
    };

    struct SuffType
    {
        const char* _suff;
        ImgType     _type;
    };
    
    static int isImageSuffix(const char* s)
    {
        static const SuffType sufTab[] =
            {
                { ".jpg", img_jpg },
                { ".jpeg", img_jpg },
                { ".png", img_png },
                { ".apng", img_png },
                { ".webp", img_webp },
            };

        for (int i = 0; i < ASIZE(sufTab); ++i)
            if (equalsIgnoreCase(s, sufTab[i]._suff))
                return sufTab[i]._type;
        
        return 0;
    }

    static bool isImageFile(const std::string& s)
    {
        std::string suf = suffixOf(s);
        return isImageSuffix(suf.c_str()) != 0;
    }

    static bool isJPG(const std::string& s)
    {
        return isImageSuffix(suffixOf(s).c_str()) == img_jpg;
    }

    static bool isWEBP(const std::string& s)
    {
        return isImageSuffix(suffixOf(s).c_str()) == img_webp;
    }

    static bool isPNG(const std::string& s)
    {
        return isImageSuffix(suffixOf(s).c_str()) == img_png;
    }

    static int isHexFile(const std::string& s, int i = 0)
    {
        // is the filename a hex number
        // eg a409482348208f.jpg
        // return position of end of hex or -1 if not hex
        // note: short names are not considered hex
        // eg bee.jpg

        int r = -1; // not hex
        int l = s.length();
        const char* p = s.c_str();
        
        while (i < l && u_ishex(p[i])) ++i;
        
        // check to see if this is the end of filename, otherwise fail
        // allow end of string or (eg) ".jpg" suffix
        if (i < l)
        {
            if (p[i] == '.' && isImageSuffix(p + i)) r = i;
        }
        else
        {
            // end of string, ok
            r = i;
        }

        // disregard short files as hex
        // eg min of 6 chars
        if (r < 6)
        {
            r = -1;
        }

        //if (r >= 0) LOG3(TAG_FSI, "### hex file " << s);

        return r;
    }

};


struct FSI
{
    // file system interface

    typedef std::string string;

    struct Name
    {
        string          _name;
        int             _orient = 0;
        bool            _autolevel;
        

        Name(const string& name) : _name(name) {}
    };
    
    typedef std::vector<Name> Names;

    enum SortOrder
    {
     sort_name = 0,
     sort_date = 1,
    };

    Names                 _names;
    FSI::SortOrder        _ordering = FSI::sort_name;

    Name& namefor(const string& ix) 
    {
        int i = std::stoi(ix);
        assert(i >= 0 && i < (int)_names.size());
        return _names[i];
    }

    virtual bool getNames()  = 0;
    virtual QImage load(const string& ix) = 0;
    virtual QImage loadThumb(const string& ix, int w, int h) = 0;

    string              _baseDir;

    static QImage makeImage(RawPixels* rp, const Name* id)
    {
        if (id && id->_autolevel) levelFilter(rp);

        QImage::Format f = (QImage::Format)rp->_format;

        if (!f)
        {
            // guess based on pixel size
            f = rp->_pixelSize == 3 ? QImage::Format_RGB888 :
                QImage::Format_RGB32;
        }
        
        return QImage(rp->_data,
                      rp->_w,
                      rp->_h,
                      f,
                      &imageCleanup, rp->_data);
    }

protected:
    

    static void imageCleanup(void* info)
    {
        //LOG1(TAG_RESAMPLE, "deleteing image data");

        uint8_t* data = (uint8_t*)info;
        delete data;
    }

};
