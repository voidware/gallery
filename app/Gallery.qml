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

    function setGridPos(p)
    {
        if (p >= 0 && p < grid.count)
        {
            grid.positionViewAtIndex(p, GridView.Center);
            grid.currentIndex = p;
        }
    }

    Column
    {
        width: parent.width
        
        TextField
        {
            // directory entry bar
            id: textin
            width: parent.width

            font.pixelSize: 32
            //bottomPadding: 4

            inputMethodHints: Qt.ImhUrlCharactersOnly 
            placeholderText: "enter directory"
            onAccepted: settings.startfolder = text
        }
        
        Rectangle
        {
            // main gallery area
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
                    setGridPos(c);
                }
                else
                {
                    if (event.key == Qt.Key_Home)  grid.positionViewAtBeginning();
                    if (event.key == Qt.Key_End) grid.positionViewAtEnd();
                    
                    if (event.key == Qt.Key_Return) app.view(grid.currentIndex);

                    if (event.key == Qt.Key_PageDown)
                    {
                        var cols = Math.floor(grid.width/grid.cellWidth)
                        var rows = Math.ceil(grid.height/grid.cellHeight);
                        setGridPos(grid.currentIndex + cols*rows);
                    }
                    if (event.key == Qt.Key_PageUp)
                    {
                        var cols = Math.floor(grid.width/grid.cellWidth)
                        var rows = Math.ceil(grid.height/grid.cellHeight);
                        setGridPos(grid.currentIndex - cols*rows);
                    }
                    else if (event.key == Qt.Key_F3) 
                    {
                        console.log("activating level filter")
                        QControl.enableLevelFilter(true);
                    }
                    else if (event.key == Qt.Key_F4) 
                    {
                        console.log("de-activating level filter")
                        QControl.enableLevelFilter(false);
                    }
                }
            }

            Component.onCompleted: grid.forceActiveFocus();
        }
    }
}
