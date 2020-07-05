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

#define TAG_FSI  "FSI, "

struct FSI
{
    // file system interface

    typedef std::string string;
    typedef std::vector<string> Names;

    enum SortOrder
    {
        sort_any,
        sort_name,
        sort_date,
    };

    virtual bool names(Names&, SortOrder sort = sort_name) const = 0;
    virtual QImage load(const string& id) = 0;
    virtual QImage loadThumb(const string& id, int w, int h) = 0;
};
