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
#include <assert.h>
#include "cutils.h"

struct Opt
{
    static char* cleanArg(char* arg)
    {
        // fix any spaces at the start or end of arg

        while (u_isspace(*arg)) ++arg;
        char* p = arg;
        char* q = p + strlen(p);
        while (p != q && u_isspace(q[-1])) *--q = 0;
        return p;
    }
    
    static bool isOpt(char* opt, const char* val)
    {
        // test option and if so, remove it
        bool r = !strcmp(opt, val);
        if (r) *opt = 0;
        return r;
    }

    static char* isOptArg(char** opt, const char* val)
    {
        char* arg = 0;
        static char dummy[1] = { 0 };
    
        // test option and if so, remove it
        if (!strcmp(*opt, val))
        {
            arg = opt[1];
            if (arg)
            {
                opt[0] = dummy;
                opt[1] = dummy;
                arg = cleanArg(arg);
            }
        }
        return arg;
    }

    static char* nextOptArg(char**& optAddr, const char* val, bool keep = false)
    {
        char* arg = 0;
        static char dummy[1] = { 0 };

        char** opt = optAddr;

        // test option and if so, remove it unless `keep`
        if (!strcmp(*opt, val))
        {
            arg = opt[1];
            if (arg)
            {
                ++optAddr; // skip arg

                if (!keep)
                {
                    opt[0] = dummy;
                    opt[1] = dummy;
                }

                arg = cleanArg(arg);
            }
        }
        return arg;
    }

    static bool nextOpt(char** opt, const char* val, bool keep = false)
    {
        static char dummy[1] = { 0 };

        // test option and if so, remove it unless `keep`
        bool r = !strcmp(*opt, val);

        if (r)
        {
            if (!keep) opt[0] = dummy;
        }
        return r;
    }

    static void rebuildArgs(int& argc, char** argv)
    {
        int n = 0;
        char** p = argv;
        char** q = p;

        while (*q)
        {
            if (**q)
            {
                *p++ = *q;
                ++n;
            }
            ++q;
        }
        *p = 0;
        argc = n;
    }

    static char** copyArgs(int argc, char** argv, int xtra = 0)
    {
        // call `deleteCopyArgs` on result
        
        size_t sz = 0;
        int i;
        for (i = 0; i < argc; ++i) sz += strlen(argv[i]) + 1;

        char** argv1 = new char*[argc+1+xtra];
        char* p = new char[sz];

        for (i = 0; i < argc; ++i)
        {
            argv1[i] = p;
            sz = strlen(argv[i]) + 1;
            memcpy(p, argv[i], sz);
            p += sz;
        }
        argv1[i] = 0;
        return argv1;
    }

    static char** createArgs(int argc, const char* argv0)
    {
        // call `deleteCopyArgs` on result
        assert(argc >= 1);
        
        size_t sz = strlen(argv0) + 1;
        char** argv = new char*[argc+1];
        argv[0] = new char[sz];
        strcpy(argv[0], argv0);
        argv[1] = 0;
        return argv;
    }

    static void addArg(int& argc, char** argv, const char* arg)
    {
        // ASSUME `argv` is the result of `copyArgs`
        // ASSUME that `xtra` space is preallocated
        
        argv[argc++] = (char*)arg;
        argv[argc] = 0;
    }
    
    static void addArg(int& argc, char** argv, const char* opt, const char* arg)
    {
        addArg(argc, argv, opt);
        addArg(argc, argv, arg);
    }

    static void deleteCopyArgs(char** argv)
    {
        // ASSUME `argv` is the result of `copyArgs`
        delete [] argv[0];
        delete [] argv;
    }
    
};
