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
import QtQuick.Layouts 1.12

import com.voidware.myapp 1.0

Item
{
    property string startfolder

    property string mailto
    property string subject
    property string bcc
    property string destdir

    anchors.margins: 16

    GridLayout {
        columns: 2
        columnSpacing: 8
        rowSpacing: 8
        width: parent.width

        PropLabel {text: "Start Folder" }
        PropInput
        {
            text: startfolder
            onChanged: startfolder = text
        }

        PropLabel {text: "mailto" }
        PropInput
        {
            text: mailto
            onChanged: mailto = text
        }

        PropLabel {text: "Subject" }
        PropInput
        {
            text: subject
            onChanged: subject = text
        }

        PropLabel {text: "BCC" }
        PropInput
        {
            text: bcc
            onChanged: bcc = text
        }
        
        //PropLabel { text: "mailfrom" }
        //PropInput { id: mailfrom }

        PropLabel { text: "move/copy destination" }
        PropInput
        {
            text: destdir
            onChanged: destdir = text
        }
        
    }
}

