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

#include <QQuickImageProvider>
#include <QImage>
#include <QThreadPool>
#include <set>
#include <mutex>
#include <assert.h>
#include "dl.h"
#include "logged.h"
#include "qdefs.h"
#include "fsi.h"
#include "qresample.h"
#include "strutils.h"
#include "imgcache.h"

#define TAG_PROV  "provider, "

#define FULL_CACHE_SIZE  5
#define THUMB_CACHE_SIZE 1024

class GalleryProviderBase : public QQuickAsyncImageProvider
{
public:

    // we can be a thumb provider or a normal image provider
    
    typedef std::string string;
    typedef std::mutex mutex;

    GalleryProviderBase(FSI* fs, bool thumb)
        :// QQuickImageProvider(QQuickImageProvider::Image),
          _thumbCache("thumb", THUMB_CACHE_SIZE),
          _fullCache("full", FULL_CACHE_SIZE)
    {
        _fs = fs;
        _thumb = thumb;
        assert(_fs);
    }

    FSI*        _fs;
    bool        _thumb;
    ImgCache    _thumbCache;
    ImgCache    _fullCache;

    // prevent full images from being loaded at the same time
    mutex       _loadLock;
};

class AsyncImageResponse:
    public QQuickImageResponse,
    public QRunnable,
    public DL<AsyncImageResponse>
{
    typedef std::string string;

public:

    GalleryProviderBase*    _host;
    QString                 _id;
    QSize                   _requestedSize;
    QImage                  _img;
    bool                    _abort = false;

    AsyncImageResponse(GalleryProviderBase* host,
                       const QString &id, const QSize &requestedSize)
        : _host(host), _id(id), _requestedSize(requestedSize)
    {
        setAutoDelete(false);
    }

    ~AsyncImageResponse()
    {
        //LOG3(TAG_PROV, " ~AsyncImageResponse " << STRQ(_id));
        remove();
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(_img);
    }

    void lock() { _host->_loadLock.lock(); }
    void unlock() { _host->_loadLock.unlock(); }

    bool abort()
    {
        if (_abort)
        {
            LOG4(TAG_PROV, "ABORT!!");
        }
        return _abort;
    }

#define ABORT if (abort()) { emit finished(); return; }

    void run() override
    {
        ABORT;
        
        int width = 0;
        int height = 0;

        string id = STRQ(_id);
        FSI::Name& name = _host->_fs->namefor(id);
        string fname = name._name;
        int rw = _requestedSize.width();
        int rh = _requestedSize.height();
        bool cacheHit = false;

        if (rw > 0 && rh > 0)
        {
            LOG4(TAG_PROV, "request image " << fname << " scaled " << rw << "x" << rh);
        }
        else
        {
            LOG4(TAG_PROV, "request image " << fname);
        }

        if (_host->_thumb)
        {
            // first look in cache
            lock();
            _img = _host->_thumbCache.find(fname);
            unlock();
           
            cacheHit = !_img.isNull();

            if (!cacheHit)
            {
                // not in cache. try loading as thumb
                _img = _host->_fs->loadThumb(id, rw, rh);
            }
        }
        else
        {
            lock();
            _img = _host->_fullCache.find(fname);
            unlock();
           
            cacheHit = !_img.isNull();
        }

        // fallback load as full image
        if (_img.isNull())
        {
            ABORT;

            LOG3(TAG_PROV, " loading full image " << fname);
        
            _img = _host->_fs->load(id);

            if (!_img.isNull())
            {
                assert(!cacheHit);

                // add to full cache
                lock();
                bool v = _host->_fullCache.add(fname, _img);
                unlock();

                if (v)
                {
                    LOG4(TAG_PROV, " loaded " << fname << ' ' << _img.width() << 'x' << _img.height());
                }
                else
                {
                    LOG4(TAG_PROV, fname << " already loaded!");
                }
            }
        }

        if (!_img.isNull())
        {
            width = _img.width();
            height = _img.height();

            if (rw > 0 && rh > 0 && (rw != width || rh != height))
            {
                double sx = (double)rw/width;
                double sy = (double)rh/height;
                double s;

                if (_host->_thumb)
                {
                    // make thumb fill space
                    s = sx >= sy ? sx : sy;
                }
                else
                {
                    // normal not clipped
                    s = sx <= sy ? sx : sy;
                }
               
                rw = width*s;
                rh = height*s;
               
                //LOG3(TAG_PROV, "requested width " << rw << " actual " << width);
                //LOG3(TAG_PROV, "requested height " << rh << " actual " << height);
                if (_host->_thumb)
                {
                    // low quality
                    _img = _img.scaled(rw, rh);
                }
                else
                {
                    LOG3(TAG_PROV, "high quality scale of " << fname << ' ' << width << 'x' << height << " to " << rw << "x" << rh << " scale " << s);

                    _img = QResample(_img, rw, rh, &_abort);
                    
                    ABORT;
                }
               
                width = rw;
                height = rh;
            }
        }
        else
        {
            LOG1(TAG_PROV, "failed to load " << fname);
        }

        if (_host->_thumb && !_img.isNull() && !cacheHit)
        {
            // add to thumb cache
            lock();
            bool v = _host->_thumbCache.add(fname, _img);
            unlock();

            if (!v)
            {
                LOG4(TAG_PROV, fname << " thumb already loaded!");
            }
        }

        //if (size) *size = QSize(width, height);
        emit finished();
    }


};

class GalleryProvider : public GalleryProviderBase
{
public:

    GalleryProvider(FSI* fs, bool thumb) : GalleryProviderBase(fs, thumb) {}

    QQuickImageResponse* requestImageResponse(const QString &id,
                                              const QSize &requestedSize) override
    {
        AsyncImageResponse *response = 
            new AsyncImageResponse(this, id, requestedSize);
        
        if (!_thumb)
        {
            // prevent too many non-thumb threads
            int n =  _asyncs.size();
            if (n > 2)
            {
                //LOG3(TAG_PROV, " thread count " << n);

                for (List::iterator it = _asyncs.begin(); it != _asyncs.end(); ++it)
                    (*it)._abort = true;
            }
        }

        _asyncs.add(response);
        
        pool.start(response);
        return response;
    }

    typedef DL<AsyncImageResponse>::List List;

    List _asyncs;
    QThreadPool pool;
};

