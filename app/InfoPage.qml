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

Item
{
    id: infopage

    Item
    {
        anchors.fill: parent
        anchors.margins: 8

        Column
        {
            width: parent.width
            spacing: 8

            Text
            {
                text: "dpi is " + QControl.dpi() + " dp is " + QControl.dp()
                font.pointSize: 36
            }

            Button
            {
                width: Math.max(100, implicitWidth)
                height: 100
                text: QControl.buttonText
                font.pixelSize: 12
                onClicked: QControl.handleClick()
            }
        }
    }
}
