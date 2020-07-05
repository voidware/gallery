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

#include <string.h>
#include "sstream.h"

struct StdFStream: public StdStream
{
    typedef std::string string;

    // Constructors
    StdFStream()
    {
        _fp = 0;
        _also = 0;
    }
    
    ~StdFStream() { close(); }

    const char* filename() const override { return _filename.c_str(); }
    bool ok() const override { return _fp != 0; }
    
    bool open(const char* filename, const char* mode = "w")
    {
        close();
        _filename = filename;
        _fp = fopen(filename, mode);
        return _fp != 0;
    }

    void close()
    {
        if (_fp)
        {
            fclose(_fp);
            _fp = 0;
        }
    }

    void setAlso(std::ostream* os)
    {
        _also = os;
    }


    bool _emit(StdStreamBuf* buf) override
    {
        bool r = false;
        if (_fp)
        {
            size_t nw = buf->size();
            r = (fwrite(*buf, 1, buf->size(), _fp) == nw) && !fflush(_fp);
            
            if (!r)
            {
                // failed, signal error
                close();
            }
        }
        
        if (_also)
        {
            _also->write(*buf, buf->size());
            _also->flush();
        }
        
        return r;
    }

protected:


    string                                      _filename;
    FILE*                                       _fp;

    // optional addition output (eg cout)
    std::ostream*                               _also;

};

