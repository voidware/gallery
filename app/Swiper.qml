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

import com.voidware.myapp 1.0

Item
{
    id: swiper
    property alias startindex: swipe.currentIndex
    property var theitem;
    
    focus:true

    function next()
    {
        swipe.incrementCurrentIndex();
    }

    function previous()
    {
        swipe.decrementCurrentIndex();
    }

    SplitView
    {
        anchors.fill: parent
        orientation: Qt.Vertical
        
        Item
        {
            SplitView.fillHeight: true

            MouseArea
            {
                // this is underneath the imageview and therefore
                // gets the rest of the events
                anchors.fill: parent
                acceptedButtons: Qt.AllButtons 
                onPressed:
                {
                    //console.log("SARG!")
                    if (mouse.button == Qt.ForwardButton) next();
                    if (mouse.button == Qt.BackButton) previous();
                }
            }

            SwipeView
            {
                id: swipe
                anchors.fill: parent
                interactive: theitem ? !theitem.zoomed: true

                Repeater
                {
                    model: gModel
                    Loader 
                    {
                        //active: SwipeView.isCurrentItem || SwipeView.isNextItem || SwipeView.isPreviousItem
                        property string label: model.label
                        active: SwipeView.isCurrentItem 
                        sourceComponent: ImageView
                        {
                            // model "name" calls the provider
                            name: model.name
                            label: model.label
                            filepath: model.path
                        }
                        onLoaded: theitem = item
                    }
                }
            }
        }

        Controlbox 
        {
            id: controlbox
            SplitView.maximumHeight: swiper.height/4
        }
    }

    Binding
    {
        target: theitem
        property: 'gamma'
        value: controlbox.gamma
    }

    Binding
    {
        target: theitem
        property: 'sharpen'
        value: controlbox.sharpen
    }

    Binding
    {
        target: app
        property: 'title'
        value: theitem.info
    }

    Binding
    {
        target: app
        property: 'drawerActive'
        value: controlbox.height == 0
    }

    Keys.onPressed:
    {
        //https://doc.qt.io/qt-5/qt.html#Key-enum
        if (event.key == Qt.Key_Right) next();
        else if (event.key == Qt.Key_Left) previous();
        else if (event.key == Qt.Key_Escape) app.pop();
        else if (event.key == Qt.Key_Home) 
        {
            swipe.currentItem.item.naturalSize()
        }
        else if (event.key == Qt.Key_F1) 
        {
            if (theitem) 
            {
                theitem.theimage.grabToImage(function(res) {
                    app.sendEmail(res.image, theitem.label);
                });
            }
        }
        else if (event.key == Qt.Key_F2) 
        {
            if (theitem) 
            {
                theitem.theimage.grabToImage(function(res) {
                    app.saveForEmail(res.image);
                });
            }
        }
        else if (event.text == "c") app.copyDest(theitem.filepath);
    }
}
    
