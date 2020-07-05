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

#include <QObject>
#include <QAbstractListModel>

#include <mutex>
#include <string>
#include <assert.h>
#include "qdefs.h"
#include "logged.h"
#include "control.h"
#include "fsi.h"
#include "cutils.h"

#define TAG_MODEL "gmodel, "

class GalleryModel : public QAbstractListModel
{
    Q_OBJECT

protected:

    typedef std::string string;
    typedef QAbstractListModel parentT;

    Control*            _control;
    FSI*                _fs;
    FSI::Names          _names;

public:

    enum CatRoles
    {
        NameRole = Qt::UserRole + 1,
        ThumbRole = Qt::UserRole + 2,
        LabelRole = Qt::UserRole + 3,
    };

    GalleryModel(Control* control, FSI* fsi) : QAbstractListModel(0) 
    {
        _control = control;
        _fs = fsi;

        _update();
    }

    int size() const
    {
       return _names.size();
    }

    // compliance
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        int n = size();
        //LOG3("items model size, ", n);
        return n;
    }

    string nameof(int ix) const
    {
        string name;
        if (ix < size()) 
        {
            name = _names[ix];
        }
        return name;
    }

    // compliance
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.isValid())
        {
            int ix = index.row();
            if (ix < size())
            {
                //assert(ix < (int)_board._tiles.size());                    
                LOG4("Request for item ", ix);

                switch (role)
                {
                case NameRole:
                    {
                        string p("image://provider/");
                        p += nameof(ix);
                        return QSTR(p);
                    }
                    break;
                case ThumbRole:
                    {
                        string p("image://thumb/");
                        p += nameof(ix);
                        return QSTR(p);
                    }
                    break;
                case LabelRole:
                    return QSTR(nameof(ix));
                    break;
                    /*
                case PathRole:
                    {
                        const string& p = _board._tiles[ix]._path;
                        //LOG3("bmodel path ", p);
                        return QSTR(p);
                    }
                    break;
                case IDRole:
                    {
                        const Hex& h = _board._tiles[ix];
                        return h._id;
                    }
                    break;
                case ColRole:
                    {
                        const Hex& h = _board._tiles[ix];
                        HexCol c(h._col);
                        return QColor(c.r, c.g, c.b, c.a);
                    }
                    break;
                    */
                default:
                    LOG3("Request for item ", ix << " unknown role " << role);
                    break;
                }
            }
        }
        return QVariant();
    }

    /*
    Q_INVOKABLE void updateLayout(int w, int xm1, int xm2)
    {
        if (w > 0)
        {
            //LOG3("UI, ", "item model changed");
            beginResetModel(); 
            endResetModel();
        }
    }
    */

    Q_INVOKABLE void changed()
    {
        beginResetModel(); 
        _update();
        endResetModel();        
        
    }

    Q_INVOKABLE int indexOfKey(int k)
    {
        //LOG3(TAG_MODEL "key index for ", k);

        int c = u_tolower(k);
        
        for (int i = 0; i < (int)_names.size(); ++i)
        {
            assert(_names[i].size());
            int ci = u_tolower(_names[i][0]);
            if (ci == c) return i;
        }
        
        return -1;
    }

protected:

    void _update()
    {
        // refresh name list
        _names.clear();

        assert(_fs);
        
        bool v = _fs->names(_names);
        if (!v) 
        {
            LOG1(TAG_MODEL, "failed to load names");
        }
    }

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[ThumbRole] = "thumb";
        roles[LabelRole] = "label";
        return roles;
    }
};

