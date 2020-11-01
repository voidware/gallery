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
#include "lodepng.h"


QImage FSFiles::loadPNG2(const string& path, const Name& id) const
{
    QImage img;
    FD::Pos fsize;
    uchar* data = _loadFile(path, fsize);

    if (data)
    {
        lodepng::State state;
        unsigned int w, h;
        uchar* raw;
        unsigned int e = lodepng_decode(&raw, &w, &h, &state, data, 
                                        (size_t)fsize);
        
        if (e)
        {
            LOG1(TAG_FSI "PNG error ", lodepng_error_text(e));
        }
        else
        {
            RawPixels rp;
            rp._data = raw;
            rp._w = w;
            rp._h = h;
            rp._pixelSize = 4; // ps._channels;
            //rp._format = QImage::Format_ARGB32;
            rp._format = QImage::Format_RGBA8888;
            rp.init();
            img = makeImage(&rp, &id);

            //img = img.rgbSwapped();
        }
        
        delete data;
    }
    return img;
}
