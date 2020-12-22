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
import QtQml 2.12

import com.voidware.myapp 1.0

Item
{
    id: tile
    property var m
    property int margin: 2
    property bool selected: GridView.isCurrentItem

    width: grid.cellWidth
    height: grid.cellHeight

    // see
    //https://doc.qt.io/qt-5/qquickimageprovider.html

    Component.onDestruction:
    {
        QControl.tileDestroyed(m.id)
    }

    Item
    {
        anchors.fill: parent
        anchors.margins: margin
        
        Image
        {
            width: parent.width
            height: parent.height - label.height
            source: m.thumb
            //autoTransform: true // use EXIF
            fillMode: Image.PreserveAspectFit

            // force image provider to scale image before we load
            sourceSize.width: width
            sourceSize.height: height 
            asynchronous: true
            cache: false
            verticalAlignment: Image.AlignBottom
        }

        Label
        {
            // caption text
            id: label
            width: parent.width
            anchors.bottom: parent.bottom
            elide: Text.ElideRight
            text: m.label
            background: Rectangle 
            { 
                color: QControl.colorForType(m.label)
            }
        }
    }

    MouseArea
    {
        anchors.fill: parent
        onClicked: 
        {
            grid.currentIndex = m.index
            grid.forceActiveFocus();
        }

        onDoubleClicked:
        {
            //app.stack.push("ImageView.qml", { "imgname": m.name })
            app.view(m.index);
        }
    }
}
