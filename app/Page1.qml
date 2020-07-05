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
import QtQuick.Dialogs 1.3


import com.voidware.myapp 1.0

Item
{
    //property bool selectFolder: true
    //property bool selectMultiple: false
    //property bool selectExisting: true

    FileDialog 
    {
        id: fileDialog
        //visible: fileDialogVisible.checked
        //modality: fileDialogModal.checked ? Qt.WindowModal : Qt.NonModal
        title: selectFolder ? "Choose a folder" :
            (selectMultiple ? "Choose some files" : "Choose a file")

        selectExisting: true
        selectMultiple: false
        selectFolder: true
        nameFilters: [ "Image files (*.png *.jpg)", "All files (*)" ]
        selectedNameFilter: "All files (*)"
        sidebarVisible: false

        onAccepted: 
        {
            console.log("Accepted: " + fileUrls)
            QControl.directory = fileUrl;
        }
        
        onRejected: { console.log("Rejected") }
    }

    Column
    {
        width: parent.width
        Button
        {
            text: "select folder"
            font.pixelSize: 32
            onClicked: fileDialog.open();
        }

        Button
        {
            text: "pictures folder"
            font.pixelSize: 32
            onClicked: fileDialog.folder = fileDialog.shortcuts.pictures
        }
    }

}
