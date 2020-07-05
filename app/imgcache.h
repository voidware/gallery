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

#define TAG_CACHE   "image cache, "

struct ImgCache
{
    typedef std::string string;
    string _name;
    
    ImgCache(const string& name, int size)
    {
        _name = name;
        _lruSizeMax = size;
    }
    
    ~ImgCache()
    {
        //LOG4(TAG_CACHE, _name << " ~ImageCache");
    }
    
    struct CacheItem: public DL<CacheItem>
    {
        string          _id;
        QImage          _image;

        ~CacheItem()
        {
            if (!_id.empty())
            {
                LOG4(TAG_CACHE, "~CacheItem " << _id);
            }
        }

        bool operator==(const CacheItem& i) const { return _id == i._id; }
        bool operator<(const CacheItem& i) const { return _id < i._id; }
    };

    struct Compare
    {
        bool operator()(const CacheItem* a, const CacheItem* b)
        {
            return *a < *b;
        }
    };

    typedef std::set<CacheItem*, Compare> Cache;
    typedef DL<CacheItem>::ListO LRU;
    
    Cache       _cache;     
    LRU         _lru;  // owns
    int         _lruSize = 0;
    int         _lruSizeMax;

    CacheItem* _find(const string& id)
    {
        CacheItem t;
        t._id = id;
        Cache::iterator it = _cache.find(&t);
        t._id.clear(); // debugging
        if (it == _cache.end()) return 0;
        return *it;
    }

    QImage  _get(CacheItem* item)
    {
        if (!item) return QImage(); // null;
        
        //LOG4(TAG_CACHE, _name << " cache hit " << id);

        // remove from whereever and replace at back of list
        item->remove();
        _lru.add(item);

        return item->_image;
    }

    QImage  find(const string& id)
    {
        return _get(_find(id));
    }

    bool   add(const string& id, QImage& img)
    {
        if (_find(id)) return false; // already present
        
        CacheItem* it = new CacheItem;
        it->_id = id;
        it->_image = img;

        if (_lruSize == _lruSizeMax)
        {
            // chuck one
            // oldest at start
            CacheItem* oi = _lru.first();
            
            LOG4(TAG_CACHE, _name << " chucking " << oi->_id);
            
            // remove from cache
            _cache.erase(oi); 
            
            oi->removeAndDelete();
            
            --_lruSize;
        }

        LOG4(TAG_CACHE, _name << " adding to cache " << id);

        // add to back of list, front is oldest
        _lru.add(it);
        ++_lruSize;

        _cache.insert(it);
        return true;
    }
};
