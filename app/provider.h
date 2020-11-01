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

    const char* thumbstr() { return _thumb ? "thumb" : "main"; }

    FSI*        _fs;
    bool        _thumb;
    ImgCache    _thumbCache;
    ImgCache    _fullCache;
    bool        _enableLevelFilter = false;

    // prevent full images from being loaded at the same time
    mutex       _loadLock;
};

class AsyncImageResponseRunnable: public QObject, public QRunnable
{
    Q_OBJECT;
    
    typedef std::string string;

signals:
    
    void done(QImage image);

public:

    GalleryProviderBase*    _host;
    QString                 _id;
    QSize                   _requestedSize;

    AsyncImageResponseRunnable(GalleryProviderBase* host,
                               const QString &id,
                               const QSize &requestedSize)
        : _host(host), _id(id), _requestedSize(requestedSize)
    {
        LOG4(TAG_PROV "create runnable for ", STRQ(id));
    }

    string makeCacheID(const string& name) const
    {
        string parts = STRQ(_id);
        size_t pos = parts.find('&');
        if (pos == string::npos) return name;
        
        // form file name + '&' options
        return name + parts.substr(pos);
    }

    void lock() { _host->_loadLock.lock(); }
    void unlock() { _host->_loadLock.unlock(); }

    QImage loadFull(const string& cid)
    {
        ImgCache::CacheItem* ci;
        QImage img;
        
        // in cache?
        lock();
        ci = _host->_fullCache.intern(cid);
        img = ci->_image; // maybe null
        unlock();

        if (img.isNull())
        {
            // need to load, get the loading lock
            ci->_loading.lock();

            // we have the lock, do we still need to load?
            lock();
            ci = _host->_fullCache.intern(cid);
            img = ci->_image;
            unlock();

            if (img.isNull())
            {
                // load
                LOG3(TAG_PROV, " loading full image " << cid);
        
                // pass in the original ID with qualified '&' options
                img = _host->_fs->load(STRQ(_id));

                if (!img.isNull())
                {
                    LOG4(TAG_PROV, " loaded " << cid << ' ' << img.width() << 'x' << img.height());

                    lock();
                    ci = _host->_fullCache.intern(cid);
                    unlock();

                    // we have the lock!
                    assert(ci && !ci->valid());
                    
                    // add to full cache
                    ci->_image = img;
                }
            }
            
            ci->_loading.unlock();
        }
        return img;
    }

    void run() override
    {
        QImage img;

        int width = 0;
        int height = 0;

        // we can signal various image options on the end of the ID
        // separated by '&'
        QStringList idparts = _id.split('&', Qt::SkipEmptyParts);
        if (!idparts.size()) return;  // bail
        
        string id = STRQ(idparts.at(0));
        FSI::Name& name = _host->_fs->namefor(id);
        string fname = name._name;
        int rw = _requestedSize.width();
        int rh = _requestedSize.height();

        if (rw > 0 && rh > 0)
        {
            LOG4(TAG_PROV, _host->thumbstr() << " request " << fname << " scaled " << rw << "x" << rh);
        }
        else
        {
            LOG4(TAG_PROV, _host->thumbstr() << " request " << fname);
        }

        string cid = makeCacheID(fname);

        if (_host->_thumb)
        {
            ImgCache::CacheItem* ci;
            
            // first look in cache
            lock();
            ci = _host->_thumbCache.intern(cid);
            img = ci->_image; // maybe null
            unlock();
            
            if (img.isNull())
            {
                // need to load, get the loading lock
                ci->_loading.lock();

                // we have the lock, do we still need to load?
                lock();
                ci = _host->_thumbCache.intern(cid);
                img = ci->_image;
                unlock();

                if (img.isNull())
                {
                    // load
                    img = _host->_fs->loadThumb(id, rw, rh);

                    if (!img.isNull())
                    {
                        // assign to cache
                        // XX ASSUME ci still valid
                        ci->_image = img;
                    }
                    else
                    {
                        //fall back to full image
                        img = loadFull(cid);
                    }
                }
                ci->_loading.unlock();
            }
        }
        else
        {
            img = loadFull(cid);
        }

        if (!img.isNull())
        {
            width = img.width();
            height = img.height();

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
                    img = img.scaled(rw, rh);
                }
                else
                {
                    LOG3(TAG_PROV, "high quality scale of " << fname << ' ' << width << 'x' << height << " to " << rw << "x" << rh << " scale " << s);

                    bool abort = false;
                    img = QResample(img, rw, rh, &abort);
                }
               
                width = rw;
                height = rh;
            }
        }
        
        if (img.isNull())
        {
            LOG1(TAG_PROV, "failed to load " << fname);
        }

        emit done(img);
    }

};

class AsyncImageResponse: public QQuickImageResponse
{
    typedef std::string string;

public:

    GalleryProviderBase*    _host;
    QString                 _id;
    QImage                  _img;

    AsyncImageResponse(GalleryProviderBase* host,
                       const QString &id,
                       const QSize &requestedSize, QThreadPool* pool)
        : _host(host), _id(id)
    {
        auto runnable = new AsyncImageResponseRunnable(host, id, requestedSize);
        connect(runnable, &AsyncImageResponseRunnable::done, this, &AsyncImageResponse::handleDone);
        pool->start(runnable);
    }

    void handleDone(QImage image)
    {
        _img = image;
        emit finished();
    }

    QQuickTextureFactory *textureFactory() const override
    {
        return QQuickTextureFactory::textureFactoryForImage(_img);
    }
};

class GalleryProvider : public GalleryProviderBase
{
public:

    GalleryProvider(FSI* fs, bool thumb) : GalleryProviderBase(fs, thumb)
    {
        LOG3(TAG_PROV, thumbstr() << " Thread pool using " << _pool.maxThreadCount() << " threads");
    }

    static QString combineOption(const QString& id, const QString& opt)
    {
        QStringList idparts = id.split('&', Qt::SkipEmptyParts);
        for (int i = 0; i < idparts.size(); ++i)
            if (idparts.at(i) == opt) return id; // already present
        
        return id + '&' + opt;
    }

    QQuickImageResponse* requestImageResponse(const QString &reqID,
                                              const QSize &requestedSize) override
    {

        QString id;
        if (_enableLevelFilter)
        {
            // add to request id, if not present
            id = combineOption(reqID, "l");
            //LOG4(TAG_PROV "request ID ", STRQ(id));
        }
        else
        {
            id = reqID;
        }
        
        AsyncImageResponse *response = 
            new AsyncImageResponse(this, id, requestedSize, &_pool);

        return response;
    }

    QThreadPool _pool;
};

