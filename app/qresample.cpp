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

#include <Qt>
#include "qresample.h"
#include "logged.h"

#define TAG_RESAMPLE "resample, "

static void imageCleanup(void* info)
{
    //LOG1(TAG_RESAMPLE, "deleteing image data");
    uint8_t* data = (uint8_t*)info;
    delete data;
}

#define ABORT if (*abort) { delete ddst.data; return QImage(); }

QImage QResample(const QImage& src, int w, int h, volatile bool* abort)
{
    ImageData dsrc;
    ImageData ddst;
    bool ok;

    ABORT;
        
    dsrc.data = (uint8_t*)src.constBits();
    dsrc.lineStride = src.bytesPerLine();
    dsrc.pixelStride = 4;
    dsrc.width = src.width();
    dsrc.height = src.height();
    
    QImage::Format f = src.format();
    ok = (f == QImage::Format_RGB32) || (f == QImage::Format_ARGB32);

    if (!ok)
    {
        if (f == QImage::Format_Grayscale8)
        {
            LOG2(TAG_RESAMPLE, "converting from grey");
            return QResample(src.convertToFormat(QImage::Format_ARGB32), w, h, abort);
        }
    }

    if (ok)
    {
        ddst.width = w;
        ddst.height = h;
        ddst.pixelStride = dsrc.pixelStride;
        ddst.lineStride = ddst.width*4;

        // memory is given to the new Qimage with a cleanup
        ddst.data = new uint8_t[ddst.lineStride * ddst.height];

        // downsize only!
        ok = ResampleMix(&dsrc, &ddst, abort);    

        ABORT;

        if (!ok)
        {
            ok = ResampleLanczos(&dsrc, &ddst, 4, abort); // 4 => choose best quality
            ABORT;
        }

        if (ok)
        {
            return QImage(ddst.data, ddst.width, ddst.height, f,
                          imageCleanup, ddst.data);
        }
        else
        {
            LOG1(TAG_RESAMPLE, "resample reject");
            delete ddst.data; ddst.data = 0;

        }
    }
    else
    {
        LOG1(TAG_RESAMPLE, "wrong format image " << (int)f);
    }

    ABORT;

    // bail out with Qt
    return src.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
