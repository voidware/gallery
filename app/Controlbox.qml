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
import QtQuick.Controls 2.13
import QtQml 2.12
import QtQuick.Layouts 1.12

import com.voidware.myapp 1.0

Rectangle
{
    property alias gamma: gammaslider.value
    property alias sharpen: sharpslider.value
    property int margin: 16
    
    implicitHeight: grid.implicitHeight + margin*2

    /*
    Item
    {
        id: box
        width: parent.width - margin*2
        implicitHeight: grid.implicitHeight + margin*2
        anchors.leftMargin: margin
        */

        GridLayout
        {
            id: grid
            columns: 3
            columnSpacing: 8
            rowSpacing: 8
            width: parent.width - margin*2

            anchors.centerIn: parent
            
            Label
            {
                //Layout.fillHeight: true
                //Layout.fillWidth: true
                //Layout.preferredHeight: implicitHeight
                //Layout.preferredWidth: implicitWidth
                background: Rectangle { border.color: "red" }
                font.pixelSize: 24
                padding: 4
                text: "Gamma"
            }
            Slider
            {
                id: gammaslider
                from: 0.5
                to: 1.5
                value: 1.0
                //Layout.fillHeight: true
                Layout.fillWidth: true
                focusPolicy: Qt.NoFocus
            }
            Label
            {
                //Layout.fillHeight: true
                //Layout.fillWidth: true
                //Layout.preferredHeight: implicitHeight
                Layout.preferredWidth: 50
                horizontalAlignment: Text.AlignHCenter
                background: Rectangle { border.color: "black" }
                font.pixelSize: 24
                padding: 4
                text: gammaslider.value.toFixed(2);
            }

            Label
            {
                //Layout.preferredHeight: implicitHeight
                //Layout.preferredWidth: implicitWidth
                background: Rectangle { border.color: "red" }
                font.pixelSize: 24
                padding: 4
                text: "Sharpen"
            }
            Slider
            {
                id: sharpslider
                from: 0
                to: 2.0
                //Layout.fillHeight: true
                Layout.fillWidth: true
            }
            Label
            {
                //Layout.preferredHeight: implicitHeight
                Layout.preferredWidth: 50
                horizontalAlignment: Text.AlignHCenter
                background: Rectangle { border.color: "black" }
                font.pixelSize: 24
                padding: 4
                text: sharpslider.value.toFixed(2);
            }
        }
    //}
}

