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

#include <QImageReader>
#include "fsi.h"
#include "fd.h"
#include "bint.h"

struct Compare: public FSITraits
{
    typedef std::string string;
    
    int _asc = 1;

    bool operator()(const string& a, const string& b) const
    {
        return compare(a, b) < 0;
    }

    int compare(const string& n1, const string& n2) const
    {
        if (n1 == n2) return 0; // identical or null

        int l1 = n1.length();
        int l2 = n2.length();

        int r = 0;

        int i = 0;
        int j = 0;

        bool begun = false;
        bool changed = false;

        while (i < l1 && j < l2)
        {
            char c1 = n1[i];
            char c2 = n2[j];

            if (!begun)
            {
                // ignore any whitespace at start of names
                if (c1 == ' ') { ++i; continue; }
                if (c2 == ' ') { ++j; continue; }

                begun = true;

                int h1 = isHexFile(n1, i);
                int h2 = isHexFile(n2, j);

                if (h1 > i)
                {
                    
                    if (h2 > j)
                    {
                        // both hex filenames
                        BigInt v1(n1.substr(i, h1-i), 16);
                        BigInt v2(n2.substr(j, h2-j), 16);
                        r = v1.compare(v2);

                        if (r != 0) break;

                        // if same, continue to compare suffix
                        i = h1;
                        j = h2;
                        continue;
                    }
                    else { r = -1; break; }  // hex < non-nex
                }
                else if (h2 > j) { r = 1; break; } // non-hex > hex
                // both non-hex
            }

            c1 = u_tolower(c1);
            c2 = u_tolower(c2);

            // underscore matches like a space but we might
            // then have a complete match with different strings
            // remember that we've changed to prevent 100% match
            if (c1 == '_') { c1 = ' '; changed = true; }
            if (c2 == '_') { c2 = ' '; changed = true; }

            if (u_isdigit(c1) && u_isdigit(c2))
            {
                int k = i + 1;
                while (k < l1 && u_isdigit(n1[k])) ++k;
                BigInt v1(n1.substr(i,k-i));
                i = k;

                k = j + 1;
                while (k < l2 && u_isdigit(n2[k])) ++k;
                BigInt v2(n2.substr(j,k-j));
                j = k;

                r = v1.compare(v2);
            }
            else
            {
                r = c1 - c2;
                ++i;
                ++j;
            }

            if (r != 0) break;
        }

        if (r == 0 && (i < l1 || j < l2))
        {
            // all same so far and some left
            if (i < l1) r = 1; // since j >= l2
            else r = -1; // since j < l2
        }

        if (r == 0 && changed)
        {
            // we have a complete match, but some characters were
            // changed.

            // perform lexical discrimination
            r = compareIgnoreCase(n1, n2);
        }

        //LOG3(TAG_FSI, "compare " << n1 << " with " << n2 << " = " << r);

        return r*_asc;
    }
};


struct FSFiles: public FSI, FSITraits
{
    bool getNames(SortOrder sortby) override
    {
        string d = _baseDir;
        if (d.empty()) d = ".";

        _names.clear();
        
        std::vector<string> fnames;
        bool v = FD::getDirectory(d.c_str(), fnames);

        if (v)
        {
            if (sortby == sort_name)
            {
                Compare c;
                std::sort(fnames.begin(), fnames.end(), c);
            }

            // filter names
            for (auto& i : fnames)
            {
                if (isImageFile(i)) 
                    _names.emplace_back(Name(i));
            }
            
#ifdef LOGGING
            if (Logged::_logLevel >= 5)
            {
                    int cc = 0;
                    printf("\n");
                    for (auto& i : _names)
                        printf("mv %s %d%s\n", i._name.c_str(), ++cc, suffixOf(i._name).c_str());
                    
                    printf("\n");
            }
#endif                
            
            LOG3(TAG_FSI, "files of directory " << d << " count " << _names.size());
        }
        else
        {
            LOG1(TAG_FSI, "failed to access directory " << d);
        }
        return v;
    }

    QImage load(const string& ix) override
    {
        Name& id = namefor(ix);
        
        string p = makePath(_baseDir, id._name);
        LOG4(TAG_FSI, "loading " << p);

        if (isWEBP(p))
        {
            QImage img = loadWebp(p);
            if (!img.isNull()) return img;
        }

        if (isJPG(p))
        {
            QImage img = loadJPEG(p, id);
            if (!img.isNull()) return img;
        }

        // TODO PNG Loader

        // otherwise use Qt to load
        QImageReader ir(QSTR(p));
        ir.setAutoTransform(true);
        return ir.read();
    }

    static QImage fixOrientation(QImage& img, int orient)
    {
        QTransform t;
        switch (orient)
        {
        case 0:
        case 1:
            orient = 0;
            break;
        case 3:
            t.rotate(180);
            break;
        case 6:
            t.rotate(90);
            break;
        case 8:
            t.rotate(-90);
            break;
        default:
            LOG2(TAG_FSI, "Unhandled orientation " << orient);
            orient = 0;
        }

        return orient ? img.transformed(t) : img;
    }

    QImage loadThumb(const string& ix, int w, int h) override
    {
        QImage r;

        Name& id = namefor(ix);
        string p = makePath(_baseDir, id._name);
        int orient = 0;

        if (loadExifThumb(p.c_str(), r, orient))
        {
            LOG3(TAG_FSI, "loaded exif thumb for " << p);
        }

        id._orient = orient;

        if (r.isNull() && isJPG(p))
        {
            // perform scan method to load jpg thumb
            uchar* data = 0;
            FD::Pos fsize;
            data = _loadFile(p, fsize);
            if (data)
            {
                LOG3(TAG_FSI, "building jpg thumb for " << p);
                loadThumbJpg(data, (unsigned int)fsize, w, h, r); // &r
                delete data;
            }
        }

        if (!r.isNull() && id._orient)
        {
            r = fixOrientation(r, id._orient);
        }
        
        return r;
    }

    QImage loadWebp(const string& path) const;
    QImage loadJPEG(const string& path, Name& n) const;

protected:

    uchar* _loadFile(const string& path, FD::Pos& fsize) const;
    
    bool loadThumbJpg(const uchar* data, unsigned int fsize,
                      int w, int h, QImage& r);

    bool loadExifThumb(const char* fname, QImage&, int& orient);

};


