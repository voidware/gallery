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

#ifdef _MSC_VER

#include <iostream>
#include <windows.h>

struct WinError
{
    WinError()
    {
        _error = GetLastError();
        _handle();
    }

    WinError(int e)
    {
        _error = e; 
        _handle();
    }

private:

    void _handle()
    {
        _format();
#ifdef _DEBUG
        OutputDebugString(buf);
#endif
        std::cerr << "Windows Error: " << buf << std::endl;
        
    }

    void _format()
    {
        DWORD buflen = sizeof(buf)/sizeof(wchar_t);

        *buf = 0;

        DWORD n = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                                0,
                                _error,
                                0,
                                buf, buflen,
                                0);

        if (!n)
        {
            /* see if we have some likely modules */
            HMODULE m1 = GetModuleHandle("WININET.DLL");
            if (m1)
            {
                n = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE,
                                  m1,
                                  _error,
                                  0,
                                  buf, buflen,
                                  0);
            }
        }
    }

    char                        buf[256];
    unsigned int                _error;
};
#else

// dummy
struct WinError {};

#endif 


