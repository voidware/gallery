
TEMPLATE=lib
TARGET=qtemail
CONFIG += staticlib
QT += qml


PROJ = $$PWD/..
include($$PROJ/app.pri)

### ////////////////// BASE ///////////////////////////////////////////

#INCLUDEPATH += $$RJSONDIR/include $$IFIDIR
#DEFINES += IFI_BUILD IFI_IMPORT


SOURCES = \
  email.cpp \
  mimetypemanager.cpp 
  
HEADERS = \
  email.h \
  mimetypemanager.h

