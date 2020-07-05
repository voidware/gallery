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
#include <mutex>
#include "sfstream.h"

template<class T> struct LogBase
{
    static int         _logLevel;
};

template<class T> int LogBase<T>::_logLevel = 1;

inline void padspace(std::basic_ostream<char,std::char_traits<char> >& os,
                     int val)
{
    while (val)
    {
        --val;
        os << ' ';
    }
}

template<class T> struct LogFiler: public LogBase<T>
{
    static StdFStream          _stream;

#ifdef LOG_MT
    static std::mutex          _mutex;
#endif

    LogFiler()
    {
#ifndef NDEBUG
        // default we copy to console, except for release build which
        // is not attached to console. instead use "-log" option to write 
        // to file in release mode
        setAlso(&std::cout);
#endif
    }
    
    static void setAlso(std::ostream* os)
    {
        // allow console output to be changed or suppressed (if 0)
        _stream.setAlso(os);
    }

    bool openFile(const char* name)
    {
        // NB: non static method for that initialisation is performed
        // append to any existing log
        return _stream.open(name, "a");
    }
    
};

template<class T> StdFStream LogFiler<T>::_stream;

#ifdef LOG_MT
template<class T> std::mutex LogFiler<T>::_mutex;
#endif

typedef LogFiler<int> Logged;

#define LOGBOOL(_b) (_b ? "true" : "false")

#ifdef LOG_MT
#define LOG(_msg, _x) \
{                                                               \
std::lock_guard<std::mutex> lock(Logged::_mutex);               \
Logged::_stream << _msg << _x << std::endl << std::flush;       \
}

#else
#define LOG(_msg, _x) \
    Logged::_stream << _msg << _x << std::endl << std::flush
#endif // LOG_MT

#define LOGN(_n, _msg, _x) if (Logged::_logLevel >= _n) LOG(_msg, _x)
#define LOG1(_msg, _x) LOGN(1, _msg, _x)

#if defined(LOGGING)

#define LOG2(_msg, _x) LOGN(2, _msg, _x)
#define LOG3(_msg, _x) LOGN(3, _msg, _x)
#define LOG4(_msg, _x) LOGN(4, _msg, _x)
#define LOG5(_msg, _x) LOGN(5, _msg, _x)

#else

#define LOG2(_msg, _x)
#define LOG3(_msg, _x)
#define LOG4(_msg, _x)
#define LOG5(_msg, _x)

#endif

