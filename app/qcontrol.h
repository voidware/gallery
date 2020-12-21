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

#include <QQmlApplicationEngine>
#include <QGuiApplication>
#include <QObject>
#include <QString>
#include <QScreen>
#include <QFileSystemWatcher>
#include <qqml.h>
#include <QDebug>
#include <QUrl>

#include "control.h"
#include "qdefs.h"
#include "qlog.h"
#include "gmodel.h"
#include "fsfiles.h"
#include "provider.h"
#include "permissions.h"

#define QREGISTER_CONTROL  QREGISTER_SINGLE(QControl)

#define TAG "Control, "

class QControl : public QObject, public Control
{
    Q_OBJECT
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged);
    Q_PROPERTY(QString startDirectory READ startDirectory);

public:

    typedef std::string string;

    static QControl* theControl()
    {
        if (!_theControl) _theControl = new QControl();
        return _theControl;
    }

    QControl() { _init(); }
    
    ~QControl()
    {
        _theControl = 0;
        // no need to delete providers
    }

    void setupLogging()
    {
#if defined(Q_OS_ANDROID)
        setupAndroidLog();
#endif
    }

    // initialise the control object
    void startUp(QGuiApplication*);
                                         
    void handleOptions(int argc, char** argv);
    
    void _init()
    {
        _app = 0;
    }

    void setLogLevel(int level)
    {
        // set here since there is a different value per DLL
        if (level >= 0 && level < 100)
            Logged::_logLevel = level;
    }

    Q_INVOKABLE int dpi() const { return _dpi; }
    Q_INVOKABLE int dp() const { return _dp; }

    Q_INVOKABLE bool isMobile() const
    {
        bool res = false;

#if defined(Q_OS_ANDROID) || defined(Q_OS_BLACKBERRY) || defined(Q_OS_IOS)
        res = true;
#endif

        return res;
    }

    // implement our button text property
    QString directory() const { return QSTR(_directory); }
    QString startDirectory() const { return QSTR(_startDirectory); }

    void _updateDirectory()
    {
        if (_gModel) _gModel->changed();
    }

    void setDirectory(const QString& dir)
    {
        string t = STRQ(dir);

        if (startsWith(t, "file://"))        
            t = STRQ(QUrl(dir).toLocalFile());

        LOG3("set directory ", t);

        if (t != _directory)
        {
            if (_perms.requestExternalStoragePermission())
            {
                _watcher.removePath(QSTR(_directory));
                _directory = t;
                _fsFiles._baseDir = t;

                _updateDirectory();

                _watcher.addPath(QSTR(_directory));
                
                emit directoryChanged();

                QObject::connect(&_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(notifyDirectoryChanged(QString)));
            }
            else
            {
                LOG1("Permission denied for ", t);
            }
        }
    }

    void setDP(QGuiApplication* app)
    {
        QScreen* s = app->primaryScreen();
        _dpi = (int)s->physicalDotsPerInch();

#if defined(Q_OS_ANDROID) || defined(Q_OS_BLACKBERRY) || defined(Q_OS_IOS)
        _dp = _dpi/160;
#else
        _dp = _dpi/100;
#endif
        if (_dp < 1) _dp = 1;

        if (_dp > 1)
        {
            std::string scaleAsString = std::to_string(_dp);
            QByteArray scaleAsQByteArray(scaleAsString.c_str(), (int)scaleAsString.length());
            qputenv("QT_SCALE_FACTOR", scaleAsQByteArray);
        }
    }


    GalleryProvider* createGalleryProvider()
    {
        assert(!_gProvider);
        _gProvider = new GalleryProvider(&_fsFiles, false);
        return _gProvider;
    }

    GalleryProvider* createThumbProvider()
    {
        assert(!_gThumbProvider);
        _gThumbProvider = new GalleryProvider(&_fsFiles, true);
        return _gThumbProvider;
    }

    Q_INVOKABLE QColor colorForType(const QString& name) const
    {
        QColor c(0,0,0,0); // transparent
        string n = STRQ(name);
        if (FSITraits::isPNG(n))
        {
            c = QColor("aliceblue");
        }
        else if (FSITraits::isWEBP(n))
        {
            c = QColor("lightyellow");
        }
        return c;
    }

    Q_INVOKABLE bool copyFile(const QString& qsrc, const QString& qdst)
    {
        bool res = false;
        string dst = STRQ(qdst);
        if (!dst.empty())
        {
            string src = STRQ(qsrc);
            if (!src.empty())
            {
                string name = filenameOf(src);
                string dpath = makePath(dst, name);
                //LOG3(TAG "copy file src:", src << " dest:" << dst << " name:" << name);

                // in case it exists already we will overwrite
                QFile::remove(QSTR(dpath));
                
                res = QFile::copy(QSTR(src), QSTR(dpath));
                if (res)
                {
                    LOG3(TAG "copied file ", src << " to " << dpath);
                }
                else
                {
                    LOG1(TAG "FAILED file copy ", src << " to " << dpath);
                }
            }
        }
        return res;
    }

    Q_INVOKABLE void enableLevelFilter(bool v)
    {
        _gProvider->_enableLevelFilter = v;
    }

    Q_INVOKABLE void tileDestroyed(const QString& qid)
    {
        if (_gThumbProvider)
            _gThumbProvider->notifyNotNeeded(qid);
    }


signals:

    void directoryChanged();

public slots:

    void notifyDirectoryChanged(const QString& dir)
    {
        LOG3(TAG "notity directory changed ", STRQ(dir));
        _updateDirectory();
    }

public:    


    FSFiles               _fsFiles;

    // set this in main
    GalleryModel*         _gModel = 0;

    // providers, normal and thumb
    GalleryProvider*      _gProvider = 0;
    GalleryProvider*      _gThumbProvider = 0;

    QFileSystemWatcher    _watcher;

private:

    static QControl*    _theControl;
    QGuiApplication*    _app;
    int                 _dpi;
    int                 _dp;
        
    int                 _uiOptionWidth = 0;
    int                 _uiOptionHeight = 0;

    Permissions         _perms;

};

