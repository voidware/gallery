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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>
#include <qqml.h>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickview.h>
#include <QResource>
#include <QIcon>
#include <QQuickStyle>

#include "qcontrol.h"
#include "opt.h"

int main(int argc, char *_argv[])
{
    Logged initLog;
    
    // create our control object first and handle options before the UI
    QControl* qc = QControl::theControl();
    qc->setupLogging();

#if defined(Q_OS_ANDROID) && !defined(NDEBUG)
    qc->setLogLevel(3);
#endif

    char** argv = Opt::copyArgs(argc, _argv);
    qc->handleOptions(argc, argv);

    QGuiApplication* tapp = new QGuiApplication(argc, argv);
    qc->setDP(tapp);
    delete tapp;

    // NB: this must happen before the UI
    //QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // create the UI
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Voidware");
    app.setApplicationName("Gallery");

#ifndef Q_OS_ANDROID
    // under android qdebug already goes to the monitor
    qInstallMessageHandler(QLog::logMessage);
#endif

    qc->startUp(&app);

    QREGISTER_CONTROL;

    // qt-email package
    extern void registerEmail();
    registerEmail();

    //QString style = QQuickStyle::name();
    //if (style.isEmpty()) QQuickStyle::setStyle("Imagine");

    GalleryModel galleryModel(qc, &qc->_fsFiles);
    qc->_gModel = &galleryModel;

    QQmlApplicationEngine engine;
    QQmlContext* ctxt = engine.rootContext();
    ctxt->setContextProperty("gModel",  &galleryModel);

    // provider is now owned by engine
    engine.addImageProvider("provider", qc->createGalleryProvider());
    engine.addImageProvider("thumb", qc->createThumbProvider());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty()) return -1;

    int res = app.exec();
    Opt::deleteCopyArgs(argv);
    return res;
}
