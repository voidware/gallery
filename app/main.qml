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

import QtQuick.Controls.Material 2.3
//import Fluid.Core 1.0 as FluidCore
import Fluid.Controls 1.0 as FluidControls

import Email 1.0
import com.voidware.myapp 1.0

FluidControls.ApplicationWindow
{
    id: app
    visible: true
    width: QControl.isMobile() ? Screen.desktopAvailableWidth : Math.min(1024, Screen.desktopAvailableWidth * 6.0)
    height: QControl.isMobile() ? Screen.desktopAvailableHeight : Math.min(1024, Screen.desktopAvailableHeight * 0.90)

    title: "Gallery 2.0.0"    

    property string directory: settings.startfolder 
    property color bgCol: "#f0f0f0"
    property bool drawerActive: true
    property bool editorActive: false

    Material.primary: Material.LightBlue
    Material.accent: Material.Blue

    appBar.maxActionCount: 3

    //onDirectoryChanged: console.log("## Directory ", directory);

    Component.onCompleted:
    {
        // command line override
        var v = QControl.startDirectory;
        if (v.length > 0) settings.startfolder = v;

        var oo = QControl.orderByOverride;
        if (oo >= 0) settings.sortOrder = oo
        
        // restore from settings
        QControl.orderByCode = settings.sortOrder
    }

    Binding
    {
        target: QControl
        property: 'directory'
        value: directory
    }

    function view(ix)
    {
        pageStack.push("Swiper.qml", { "startindex": ix });
        pageStack.currentItem.forceActiveFocus();
    }

    function showSettings()
    {
        //console.log("Settings clicked")
        pageStack.push("SettingsPage.qml",
        { "startfolder" : "moose" } // settings.startfolder 
        )

        pageStack.currentItem.forceActiveFocus();
    }

    function showEditor()
    {
        editorActive = !editorActive
        if (pageStack.currentItem instanceof Swiper)
        {
            pageStack.currentItem.controlsVisible = editorActive
        }
    }

    function pop()
    {
        pageStack.pop()
        title = directory
        drawerActive = true
    }
    
    function sendEmail()
    {
        var it = pageStack.currentItem;
        if (it instanceof Swiper)
        { 
            it = it.theitem
            if (it && it.theimage)
            {
                it.theimage.grabToImage(function(res) 
                {
                    email.body = it.label
                    if (settings.mailto) email.mailto = settings.mailto
                    if (settings.subject) email.subject = settings.subject
                    if (settings.bcc) email.bcc = settings.bcc
                    email.open(res.image);
                });
            }
        }
    }

    function copyDest(name)
    {
        var d = settings.destdir;
        if (d.length > 0) QControl.copyFile(name, d);
    }

    function saveForEmail(img)
    {
        email.save(img);
    }

    initialPage: mainpage

    FluidControls.Page 
    {
        id: mainpage
        title: "Main Page"

        x: navDrawer.modal ? 0 : navDrawer.position * navDrawer.width
        width: parent.width - x

        actions: 
        [
            FluidControls.Action {
                text: qsTr("Email")
                icon.source: FluidControls.Utils.iconUrl("communication/email")
                toolTip: qsTr("Email Picture")
                onTriggered: sendEmail()
            },
            FluidControls.Action {
                text: qsTr("Edit")
                icon.source: FluidControls.Utils.iconUrl("image/edit")
                toolTip: qsTr("Show Editor")
                onTriggered: showEditor()
            },
            FluidControls.Action {
                text: qsTr("Settings")
                icon.source: FluidControls.Utils.iconUrl("action/settings")
                toolTip: qsTr("Settings")
                hoverAnimation: true
                onTriggered: showSettings()
            }
        ]

        leftAction: FluidControls.Action {
            icon.source: FluidControls.Utils.iconUrl("navigation/menu")
            visible: navDrawer.modal
            onTriggered: navDrawer.visible ? navDrawer.close() : navDrawer.open()
        }

        Gallery 
        {
            anchors
             {
                 fill: parent
                 leftMargin: 4
                 rightMargin: 4
             }
        }
    }

    FluidControls.NavigationDrawer
    {
        id: navDrawer
        //width: 0.66 * app.width
        //height: app.height
        //dragMargin: 32
        //interactive: drawerActive

        readonly property bool mobileAspect: true // app.width < 1000

        modal: mobileAspect
        interactive: mobileAspect
        position: mobileAspect ? 0.0 : 1.0
        visible: !mobileAspect
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

        property string startfolder
        
        // email
        property string mailto
        property string subject
        property string bcc

        // files
        property string destdir

        property int sortOrder
        
        onSortOrderChanged:
        {
            //console.log("sort order changed to", sortOrder)
            QControl.orderByCode = sortOrder
        }

    }

}
