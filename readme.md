# Gallery

A better picture gallery

## Quickstart

* usage
enter directory in top line entry box

* keys
F1 = email current picture view
letter key = skip to image
arrow left & right = next/prev in image view
enter = open image
esc = close image
page up/down 
home/end = top and bottom in gallery view

* mouse
Scrollwheel to zoom
left button pans
double click to close image

* panels
Left slide panel = email settings
Bottom slide panel = picture settings (gamma & sharpen)


## Building

Dependencies Qt5.12.X

### epg
cd tools\epeg\src\lib
make

### jpeg-turbo
cd tools\libjpeg-turbo
rm -rf build
mkdir build
cd build


d:\msys64\mingw32
cd /d/hugh/apps/gallery/tools/libjpeg-turbo/build
export PATH=/d/Qt/Qt5.12.8/Tools/mingw730_64/bin:$PATH

cd /i/hugh/apps/gallery/tools/libjpeg-turbo
mkdir build
cd build
export PATH=/i/Qt/Qt5.12.9/Tools/mingw730_64/bin:$PATH

cmake -G"MSYS Makefiles" ..
make

NB: need nasm

libs are:
build\libjpeg.a
build\libturbojpeg.a

exit shell

manually copy *.a to lib (from build)
mkdir ..\lib
copy /Y *.a ..\lib


### webp
copy makefile.unix to makefile
comment out 4 lines starting with EXTRA_FLAGS, change to:

```
EXTRA_FLAGS= -DWEBP_HAVE_PNG -I../libpng
DWEBP_LIBS= -L../libpng/release -l:libpng.a -L../zlib/release -l:libz.a
CWEBP_LIBS= $(DWEBP_LIBS)
```


ensure `gcc` maps to the version from qt

then;
make

generates src\libwebp.a

### app
cd to root dir

qmake -r
make

results in app\debug and app\release




