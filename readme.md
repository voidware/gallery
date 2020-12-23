# Gallery

A better picture gallery

## Quickstart

### Usage
enter directory in top line entry box

### Gallery Keys
* letter key = skip to image
* arrow left & right = next/prev in image view
* enter = open image
* page up/down 
* home/end = top and bottom in gallery view

### Picture View
* esc = close image
* c = copy image to destdir (defined in settings panel)

### Mouse
* Scrollwheel to zoom
* Ctrl+scroll, rotate
* left button pans
* double click to close image

## Building

Dependencies Qt5.14.X

These instructions as for Windows mingw64.

### epg
```
cd tools\epeg\src\lib
make
```

### jpeg-turbo
NOTE: must make a `.pro` file for this!

NB: need `nasm`

```
cd tools\libjpeg-turbo
rm -rf build
mkdir build
cd build
```

```
d:\msys64\mingw32
cd /d/hugh/apps/gallery/tools/libjpeg-turbo/build
export PATH=/d/Qt/Qt5.12.8/Tools/mingw730_64/bin:$PATH
```

```
cd /i/hugh/apps/gallery/tools/libjpeg-turbo
mkdir build
cd build
export PATH=/i/Qt/Qt5.12.9/Tools/mingw730_64/bin:$PATH

cmake -G"MSYS Makefiles" ..
make
```

libs are:
* build\libjpeg.a
* build\libturbojpeg.a

exit shell

manually copy *.a to lib (from build)
```
mkdir ..\lib
copy /Y *.a ..\lib
```


### webp
* copy makefile.unix to makefile
* comment out 4 lines starting with EXTRA_FLAGS, change to:

```
EXTRA_FLAGS= -DWEBP_HAVE_PNG -I../libpng
DWEBP_LIBS= -L../libpng/release -l:libpng.a -L../zlib/release -l:libz.a
CWEBP_LIBS= $(DWEBP_LIBS)
```

ensure `gcc` maps to the version from qt

then;
`make`

generates `src\libwebp.a`

### App
cd to `gallery` root dir;

```
qmake -r
make
```

results in `app\debug` and `app\release`

### Distribution

cd `gallery` root dir;

`mkdist.bat`

## Misc Notes

Turns out Qt already has libjpeg-turbo _and_ webp. Which means the loading of images with these should be built in, **but**;

Gallery wants to make fast thumbnails on the fly, ideally without loading the whole image. For Jpeg, we look for EXIF thumbs, which is nothing to do with libjpg, but often they aren't there. So in that case we perform a high-speed jpeg low-res load. This is done using the libjpeg low-level API supplying all the fast, low-quality flags and 1/8 scaling. You can't do this from the qt image level.

Use webp scaling for webp thumbnails. not sure if this more efficient than full load & scale, but at least the webp lib has the problem at the low level. In theory, it should be better.

Does webp have exif? Well it appears the container format can have meta data, which _might_ have exif and _might_ have a thumb. However, I've not seen any image program that puts it in!

uses libpng directly. fast thumbnails can be generated providing the png is "interlaced", which is the same idea as progressive jpeg. However, most pngs are not, in which case the whole image must be loaded just for the thumbnail :-/


























