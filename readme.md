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
* F1 = email current picture view
* esc = close image
* c = copy image to destdir (defined in side panel)

### Mouse
* Scrollwheel to zoom
* Ctrl+scroll, rotate
* left button pans
* double click to close image

### Panels
* Left slide panel
  Settings.
* Bottom slide panel
  nondestructive image adjustment (gamma & sharpen)


## Building

Dependencies Qt5.13.X or Qt5.14.X

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

Right now, webp loads the whole image always. So we're no better than the built-in qt image. But, i want to do something similar with webp. not sure how at the moment, but it will be needed.

Does webp have exif? Well it appears the container format can have meta data, which _might_ have exif and _might_ have a thumb. However, I've not seen any image program that puts it in!

So one idea later is to have an option to actually insert the EXIF + thumb into an existing webp _without_ changing the image bits at all. Ie non-destructive with respect to the image.

Also we get to use the latest webp lib, which supports a "nearly lossless" format. Maybe add some re-compress options later. eg convert to lossless/nearly lossless etc.

Plan to add libpng as well at some point. The same issues with exif and fast thumbnail apply here too. For some weird reasons EXIF support wasn't added to png until recently, so no one supports it! Have you noticed that PNGs don't have thumbnails. :-/

Maybe have an option to nondestructively add them, either that or some ripping code to fast load a low-res version like we do for jpg. There's also the progressive storage mode for png, which is ideal for thumbnail generation. Although this is a compression choice and we don't want to have to lossless recompress images.























