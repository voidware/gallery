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

import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.2
import Qt.labs.settings 1.0

import com.voidware.myapp 1.0

import Email 1.0

ApplicationWindow
{
    id: app
    visible: true
    width: QControl.isMobile() ? Screen.desktopAvailableWidth : Math.min(1024, Screen.desktopAvailableWidth * 6.0)
    height: QControl.isMobile() ? Screen.desktopAvailableHeight : Math.min(1024, Screen.desktopAvailableHeight * 0.90)

    title: "Gallery 1.0.7"
    
    property string directory: settings.startfolder 
    property color bgCol: "#f0f0f0"
    property bool drawerActive: true

    //onDirectoryChanged: console.log("## Directory ", directory);

    Component.onCompleted:
    {
        // command line override
        var v = QControl.startDirectory;
        if (v.length > 0) settings.startfolder = v;
    }

    Binding
    {
        target: QControl
        property: 'directory'
        value: directory
    }

    function view(ix)
    {
        pagestack.push("Swiper.qml", { "startindex": ix });
        pagestack.currentItem.forceActiveFocus();
    }

    function pop()
    {
        pagestack.pop()
        title = directory
        drawerActive = true
    }
    
    function sendEmail(img, label)
    {
        email.body = label
        if (settings.mailto) email.mailto = settings.mailto
        if (settings.subject) email.subject = settings.subject
        if (settings.bcc) email.bcc = settings.bcc
        email.open(img);
    }

    function copyDest(name)
    {
        QControl.copyFile(name, settings.destdir);
    }

    function saveForEmail(img)
    {
        email.save(img);
    }

    StackView
    {
        id: pagestack
        anchors.fill: parent
        initialItem: Gallery {}
    }

    Email 
    {
        id: email
    }

    Settings 
    {
        id: settings

        // ui
        property alias x: app.x
        property alias y: app.y
        property alias width: app.width
        property alias height: app.height

        property alias startfolder: props.startfolder

        // email
        property alias mailto: props.mailto
        property alias subject: props.subject
        property alias bcc: props.bcc

        // files
        property alias destdir: props.destdir

    }

    Drawer
    {
        id: drawer
        width: 0.66 * app.width
        height: app.height
        dragMargin: 32
        interactive: drawerActive

        Props
        {
            id: props
            anchors.fill: parent
        }
    }
}
