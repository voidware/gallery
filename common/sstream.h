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

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <functional>
#include "sbuf.h"

struct StdStreamBuf: public std::streambuf, public SBuf
{
    typedef std::function<bool(StdStreamBuf*)> Emitter;
    
    // NB: MUST set emitter before used
    
    void emitter(const Emitter& e) { _emitter = e; }

    void _emit()
    {
        (_emitter)(this);
        clear();
    }
    
protected:

    int_type overflow(int_type c)
    {
        if (c != traits_type::eof())
        {
            add((char)c);
            if (c == '\n')
                _emit();
        }
        return c; // traits_type::eof() if failed
    }

    int sync()
    {	
        if (size() > 0) _emit();
        return 0; // -1 if failed
    }

    Emitter     _emitter;
};

struct StdStream: public std::ostream
{
    // Constructors
    StdStream() : std::ostream(&_buf)
    {
        using std::placeholders::_1;
        _buf.emitter(std::bind(&StdStream::_emit, this, _1));
    }

    virtual ~StdStream() {}
    
    operator const      char*() const { return _buf; }
    size_t              size() const { return _buf.size(); }
    
    virtual const char* filename() const { return 0; }
    virtual bool ok() const { return true; }
    virtual bool _emit(StdStreamBuf* buf) = 0;

    int printf(const char* fmt, ...)
    {
        char buf[4096];
        
        va_list args;
        va_start(args, fmt);
        int n = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        char* tbuf = buf;
        
        if (n >= (int)sizeof(buf))
        {
            // failed. allocate temp buffer
            tbuf = new char[n+1];
            va_start(args, fmt);
            n = vsnprintf(tbuf, sizeof(buf), fmt, args);
            va_end(args);
        }
        
        write(tbuf, n);
        if (tbuf != buf) delete [] tbuf;
        
        return n;
    }

protected:

    StdStreamBuf                                _buf;

};

struct StdOStream: public StdStream
{
    bool _emit(StdStreamBuf* buf) override
    {
        // stdout emitter
        const char* p = *buf;
        while (*p) { putchar(*p); ++p; }
        return true;
    }

};

