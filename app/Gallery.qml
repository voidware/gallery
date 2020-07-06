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

import com.voidware.myapp 1.0

FocusScope
{
    id: gallery
    Column
    {
        width: parent.width
        
        TextInput
        {
            id: textin
            width: parent.width

            font.pointSize: 18
            bottomPadding: 4

            inputMethodHints: Qt.ImhUrlCharactersOnly 

            onAccepted:
            {
                console.log("entered", text)
                app.directory = text
            }
        }
        
        Rectangle
        {
            //anchors.fill: parent
            
            color: app.bgCol
            width: parent.width
            height: gallery.height - textin.height

            GridView
            {
                id: grid
                anchors.fill: parent
                cellWidth: 128
                cellHeight: 192
                focus: true
                snapMode: GridView.SnapToRow
                cacheBuffer: 0  // dont keep any delegates
                //keyNavigationEnabled : true
                clip: true
                
                highlight: Rectangle
                {
                    color: "transparent"
                    border.color: "blue"
                }
                
                model: gModel
                delegate: Tile 
                {
                    m: model
                }

                ScrollBar.vertical: ScrollBar { }
            }

            Keys.onPressed:
            {
                var c = gModel.indexOfKey(event.key)
                if (c >= 0) 
                {
                    grid.positionViewAtIndex(c, GridView.Center);
                    grid.currentIndex = c;
                }
                else
                {
                    if (event.key == Qt.Key_PageDown) grid.positionViewAtEnd();
                    if (event.key == Qt.Key_PageUp)  grid.positionViewAtBeginning();
                    if (event.key == Qt.Key_Return) app.view(grid.currentIndex);
                }
            }

            Component.onCompleted: grid.forceActiveFocus();
        }
    }
}