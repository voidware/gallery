#

TEMPLATE = app
TARGET = Gallery
CONFIG += c++11
QT += qml gui quick quickcontrols2

android {
QT += androidextras
}

PROJ = $$PWD/..
include($$PROJ/app.pri)

### ////////////////// SOURCES ///////////////////////////////////////////

JPEGD = $$PROJ/tools/libjpeg-turbo
EXIFD = $$PROJ/tools/libexif
QTEMAIL = $$PROJ/qt-email
WEBPD = $$PROJ/tools/libwebp-1.1.0/src
        
INCLUDEPATH += $$JPEGD
LIBS += -L$$JPEGD/lib/ -lturbojpeg

INCLUDEPATH += $$EXIFD
LIBS += -L$$EXIFD/libexif/$$LIBDIR -lexif

INCLUDEPATH += $$WEBPD
LIBS += -L$$WEBPD -lwebp

include($$QTEMAIL/email.pri)
        
SOURCES += \
    main.cpp \
    qlog.cpp \
    qcontrol.cpp \
    control.cpp \
    resample.cpp \
    resamplemix.cpp \
    qresample.cpp \
    fsfiles.cpp \
    permissions.cpp
    
RESOURCES += qml.qrc

win32 {
   RC_ICONS = gallery.ico
}

HEADERS += \
    qdefs.h \
    qlog.h \
    qcontrol.h \
    gmodel.h \
    control.h \
    provider.h \
    fsi.h \
    fsfiles.h \
    resample.h \
    qresample.h \
    imgcache.h \
    permissions.h \
    bint.h

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}
