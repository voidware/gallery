#pragma once

#include <functional>

struct RawPixels
{
    uchar*  _data;
    size_t  _size;
    int     _w;
    int     _h;
    int     _pixelSize;
};

typedef std::function<bool(RawPixels*)> PixelFilter;

