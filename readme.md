# Gallery

A better picture gallery

* usage
enter directory in top line entry box

* keys
F1 = email current picture (see left slide drawer for setup)
letter key = skip to image
arrow left & right = next/prev in image view
enter = open image
esc = close image
page up/down = top and bottom in gallery view




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

cmake -G"MSYS Makefiles" ..
make

NB: need nasm

libs are:
build\libjpeg.a
build\libturbojpeg.a

exit shell

manually copy *.a *.dll to lib
copy /Y *.dll d:\hugh\apps\gallery\tools\libjpeg-turbo\lib
copy /Y *.a d:\hugh\apps\gallery\tools\libjpeg-turbo\lib

### webp
copy makefile.unix to makefile
comment out 4 lines starting with EXTRA_FLAGS

ensure `gcc` maps to the version from qt

then;
make

generates src\libwebp.a

### app
cd to root dir

qmake -r
make

results in app\debug and app\release




