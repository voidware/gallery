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

#include <string>
#include <assert.h>
#include <iostream>
#include "cutils.h"

#pragma once

struct BigInt
{
    /* Big Integer for decimal and hex
     * Supports only non-negative numbers.
     */
    typedef std::string string;

    int         _base;
    char*       _d;
    int         _n;
    bool        _neg = false; // signal we are negative

    BigInt(const string& s, int base = 10) : _base(base)
    {_init(s.c_str()); }

    BigInt(const char* s, int base = 10) : _base(base)
    {_init(s); }
    
    BigInt(const BigInt& a)
    {
        _base = a._base;
        _n = a._n;
        _neg = a._neg;
        _d = new char[_n];
        memcpy(_d, a._d, _n*sizeof(*_d));
    }

    ~BigInt() { clear(); }

    void clear()
    {
        delete [] _d;
        _d = 0;
        _n = 0;
        _neg = false;
    }

    bool isZero() const { return _n == 0; }

    BigInt& operator-=(const BigInt& a) { _sub(a); return *this; }

    int compare(const BigInt& a) const
    {
        int v;

        BigInt t(*this);
        t -= a;


        if (t._neg) v = -1;
        else if (t.isZero()) v = 0;
        else v = 1;

        //std::cout << "temp = " << t << std::endl;
        //std::cout << "compare " << *this << " to " << a << " = " << v << std::endl;

        return v;
    }

    string toString() const
    {
        if (_neg) return "-?";
        if (isZero()) return "0";
        
        string s;
        for (int i = 0; i < _n; ++i)
        {
            if (_d[i] < 10)  s += (char)(_d[i] + '0');
            else s += (char)('a' + _d[i]);
        }
        return s;
    }

    friend std::ostream& operator<<(std::ostream& os, const BigInt& a)
    { return os << a.toString(); }

private:

    void _init(const char* s)
    {
        _n = 0;
        _d = 0;
        
        // zero will be null 
        while (u_isspace(*s) || *s == '0') ++s;

        const char* q = s;
        while (u_ishex(*s))
        {
            ++s;
            ++_n;
        }

        if (_n)
        {
            _d = new char[_n];
            for (int i = 0; i < _n; ++i)
            {
                int v;
                if (u_isdigit(*q)) v = *q - '0';
                else v = u_tolower(*q) - 'a' + 10;
                _d[i] = v;
                ++q;
            }
        }
    }

    void _norm()
    {
        if (_neg) return;

        int i;
        for (i = 0; i < _n; ++i) if (_d[i]) break;
        if (i > 0)
        {
            if (i >= _n) clear();
            else
            {
                _n -= i;
                memmove(_d, _d + i, _n*sizeof(*_d));
            }
        }
    }

    void _sub(const BigInt& a)
    {
        assert(!_neg && !a._neg);

        if (a.isZero()) return;
        if (a._n <= _n)
        {
            int i = _n-1;
            int c = 0;
            for (int j = a._n-1; j >= 0; --j, --i)
            {
                int v = _d[i] - a._d[j] - c;
                c = 0;
                if (v < 0) 
                {
                    c = 1;
                    v += _base;
                }
                _d[i] = v;
            }

            // propagate borrow
            while (c && i >= 0)
            {
                int v = _d[i] - c;
                c = 0;
                if (v < 0) 
                {
                    c = 1;
                    v += _base;
                }
                _d[i] = v;
                --i;
            }
            if (c) _neg = true;
            else _norm(); // kill zeros
        }
        else _neg = true; 
    }
};
