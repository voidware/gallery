#
#  
#  
#    ______              _
#    | ___ \            | |
#    | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
#    | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
#    | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
#    \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
#  
#  
#    "The creative principle which lies realized in the whole world"
#  
#   Copyright (c) Voidware 2016-2017.
#  
#   This program is free software: you can redistribute it and/or modify it
#   under the terms of the GNU Lesser General Public License (LGPL) as published
#   by the Free Software Foundation, either version 3 of the License, or (at
#   your option) any later version.
#   
#   This program is distributed in the hope that it will be useful, but WITHOUT
#   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#   FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
#   for more details.
#   
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.
#  
#   contact@voidware.com
#  
#  

TEMPLATE=lib
TARGET=exif
CONFIG += static
QT -= core gui

PROJ = $$PWD/../../..
include($$PROJ/app.pri)

INCLUDEPATH += ..

#DEFINES +=GETTEXT_PACKAGE=\"womabat\"

### ////////////////// BASE ///////////////////////////////////////////

SOURCES = \
exif-byte-order.c \
exif-content.c \
exif-data.c \
exif-entry.c \
exif-format.c \
exif-ifd.c \
exif-loader.c \
exif-log.c \
exif-mem.c \
exif-mnote-data.c \
exif-tag.c \
exif-utils.c \
canon/exif-mnote-data-canon.c \
canon/mnote-canon-entry.c \
canon/mnote-canon-tag.c \
olympus/exif-mnote-data-olympus.c \
olympus/mnote-olympus-entry.c \
olympus/mnote-olympus-tag.c \
fuji/exif-mnote-data-fuji.c \
fuji/mnote-fuji-entry.c \
fuji/mnote-fuji-tag.c \
pentax/exif-mnote-data-pentax.c \
pentax/mnote-pentax-entry.c \
pentax/mnote-pentax-tag.c \



    HEADERS = \

