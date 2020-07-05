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

/// intrusive Dlist
template<class T> class DL
{
public:

    struct iterator
    {
        iterator() {}
        iterator(const DL* pos) : _pos(pos) {}

        /// get object
        T& operator*() const { return *(T*)_pos; }
        T* operator->() const { return (T*)_pos; }

        operator const T*() const { return (const T*)_pos; }
        operator T*() { return (T*)_pos; }
        
        /// comparison
        bool operator==(const iterator& it) const { return _pos == it._pos; }
        bool operator!=(const iterator& it) const { return _pos != it._pos; }
        
        /// bump
        iterator& operator++() { _pos=_pos->_next; return *this; }

        iterator operator++(int)
        {
            iterator i(_pos);
            return ++i;
        }

        iterator& operator--() { _pos = _pos->_prev; return *this; }

        DL* remove()
        {
            // ASSUME valid and not at end()
            // removes element at current position from list
            // and moves iterator to the next element (which could be end())
            DL* p = const_cast<DL*>(_pos); 
            _pos = p->_next;
            p->remove();
            return (T*)p;
        }

    private:
        const DL*     _pos;
    };

    struct List
    {
        typedef typename DL::iterator iterator;
        typedef T value_type;

        /// iteration
        iterator   begin() const { return _head._next; }
        iterator   end() const { return &_head; }

        /// are we empty
        bool empty() const { return _head._next == &_head; }

        // ASSUME not empty
        T* first() const { return (T*)_head._next; }
        T* last() const { return (T*)_head._prev; }

        /// add to end of list
        void        add(DL* r) { r->insert(&_head); }
        
        /// long-winded size measure.
        unsigned int size() const
        {
            unsigned int n = 0;
            for (iterator i = begin(); i != end(); ++i) ++n;
            return n;
        }

        void clear() { _head._init(); }

        List& operator=(const List& l)
        {
            // donate semantics
            clear();
            if (!l.empty())
            {
                _head = l._head;
                first()->_prev = &_head;
                last()->_next = &_head;

                // disconnect other list
                const_cast<List&>(l).clear();
            }
            return *this;
        }

    protected:
        DL    _head;
    };

    // list of owned DLs
    struct ListO: public List
    {
        ~ListO() { clear(); }

        ListO& operator=(const ListO& l)
        {
            clear();
            List::operator=(l);
            return *this;
        }

        void clear()
        {
            while (!this->empty()) this->first()->removeAndDelete();
        }
    };

    DL() { _init(); }

    virtual ~DL() { remove(); }

    /// insert in front of `r'
    void        insert(DL* r)
    { _next = r; _prev = r->_prev; _prev->_next = this; r->_prev = this; }

    /// remove from list
    void        remove() 
    { _prev->_next = _next; _next->_prev = _prev; _init(); }

    void        removeAndDelete()
    {
        _prev->_next = _next;
        _next->_prev = _prev;
        delete this;
    }

    T*      next() { return (T*)_next; }
    T*      prev() { return (T*)_prev; }
    

private:
    
    void _init() { _next = this; _prev = this; }

    DL*          _next;
    DL*          _prev;

};

