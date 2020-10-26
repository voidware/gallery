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

import QtQuick 2.13
import QtQuick.Controls 2.5
import QtGraphicalEffects 1.13

import com.voidware.myapp 1.0

Rectangle
{
    id: root
    property string name
    property string label
    property string filepath
    property double sc: 1.0
    property alias gamma: setgamma.gamma
    property alias sharpen: sharpeneffect.coeff_blur

    // used for grabbing final result
    property var theimage: fillsCanvas ? canvas : sharpeneffect

    property bool pinching: false
    property bool zoomed: false
    property bool sharpenActive: sharpen > 0

    property int margin : 2
    border.width: margin
    border.color: allscale <= 1 ? "black" : "red"
    color: app.bgCol

    property double pscale: Math.min(canvas.width/pic.sourceSize.width, canvas.height/pic.sourceSize.height)
    property int pw : pic.sourceSize.width * pscale * sc
    property int ph : pic.sourceSize.height * pscale * sc
    property int dw : canvas.width - pw
    property int dh : canvas.height - ph
    property double allscale: pw/pic.sourceSize.width
    property alias hqpic: hqpic
    property bool fillsCanvas: dw < 0 || dh < 0

    property string info: label + ' ' + pic.sourceSize.width + 'x' + pic.sourceSize.height + ' ' + (allscale*100).toFixed(1) + '%'

    function bound()
    {
        frame.x = Math.min(Math.max(Math.min(dw, 0), frame.x), Math.max(dw, 0))
        frame.y = Math.min(Math.max(Math.min(dh, 0), frame.y), Math.max(dh, 0))
    }

    function setscale(s)
    {
        if (s < 1) reset();
        else sc = s;
        zoomed = sc > 1
        bound();
    }

    function naturalSize()
    {
        setscale(1/pscale)
    }

    function autoLevel()
    {
        hqpic.source = name + "&l";
        //console.log("apply autolevel", pic.source)
    }

    function reset()
    {
        sc = 1
        frame.rotation = 0
        centrex.start()
        centrey.start()
    }

    NumberAnimation
    {
        id: centrex
        target: frame
        property: "x"
        to: dw/2
        duration: 200
    }
    
    NumberAnimation
    {
        id: centrey
        target: frame
        property: "y"
        to: dh/2
        duration: 200
    }
    
    Item
    {
        id: canvas
        anchors.fill: parent
        anchors.margins: margin
        clip: true

        onWidthChanged: bound()
        onHeightChanged: bound()

        Item
        {
            // the frame is a tight bounds of the image
            id: frame
            width: pw
            height: ph
            anchors.centerIn: parent
            visible: false

            Image
            {
                id: pic
                anchors.fill: parent
                source: name
                //autoTransform: true // use EXIF
                fillMode: Image.PreserveAspectFit
                cache: false

                // should not be needed as we replace by
                // high quality resize, but when resizing without it
                // there can be aliasing.
                mipmap: true

                // if synchronous then loads twice! 
                // one here and one for hqpic
                asynchronous: true
            }

            Image
            {
                id: hqpic

                // background high quality resize
                anchors.fill: parent
                source: name
                cache: false
                //autoTransform: true // use EXIF

                asynchronous: true
                sourceSize.width: pw 
                sourceSize.height: ph
                smooth: false
                z:1

                //visible: !pinching
            }

            /*
            BusyIndicator
            {
                anchors.fill: parent
                running: pic.status === Image.Loading
            }
            */
        }

        GammaAdjust 
        {
            id: setgamma
            anchors.fill: frame
            source: frame
            gamma: gamma

            // so we can apply shader
            layer.enabled: true
            visible: !sharpenActive // !pinching
        }

        ShaderEffect 
        {
            id: sharpeneffect
            anchors.fill: setgamma
            property variant src: setgamma
            property double imgWidth: width
            property double imgHeight: height
            property double coeff_blur: 0
            fragmentShader: "qrc:/unsharp.glsl"
            blending: false
            visible: sharpenActive
        }

        PinchArea
        {
            id: apinch
            anchors.fill: frame
            pinch.target: frame
            pinch.minimumRotation: 0
            pinch.maximumRotation: 0
            pinch.minimumScale: 0.1
            pinch.maximumScale: 10
            //pinch.dragAxis: Pinch.NoDrag
            pinch.dragAxis: Pinch.XAndYAxis
            
            onPinchStarted: pinching = true

            onPinchFinished:
            {
                console.log("pinch scale ", pinch.scale, sc*pinch.scale);
                frame.scale = 1.0;
                setscale(sc*pinch.scale);
                pinching = false
            }

            /*
            onSmartZoom:
            {
                if (pinch.scale > 0) 
                {
                    frame.rotation = 0;
                    setscale(Math.min(root.width, root.height) / Math.max(pic.sourceSize.width, pic.sourceSize.height) * 0.85);
                } else {
                    frame.rotation = pinch.previousAngle
                    setscale(pinch.previousScale);
                }
            }
            */

            MouseArea 
            {
                id: dragArea
                anchors.fill: parent
                drag.target: dragEnable ? frame : undefined
                drag.minimumX: Math.min(dw, 0)
                drag.minimumY: Math.min(dh, 0)
                drag.maximumX: Math.max(dw, 0)
                drag.maximumY: Math.max(dh, 0)

                scrollGestureEnabled: false  // 2-finger-flick gesture should pass through to the Flickable

                onDoubleClicked: app.pop()

                property bool dragEnable: false

                // break bindings so we can drag
                onPressed:
                {
                    if (zoomed)
                    {
                        frame.anchors.centerIn = undefined
                        dragEnable = true
                    }
                }

                /*
                onPressAndHold:
                {
                    console.log("press hold")
                    dragEnable = true
                }
                */

                /*
                //onClicked:
                onDoubleClicked:
                {
                    dragEnable = false
                    reset()
                }
                */

                onWheel: 
                {
                    if (wheel.modifiers & Qt.ControlModifier)
                    {
                        canvas.rotation += wheel.angleDelta.y / 120 * 5;
                        if (Math.abs(canvas.rotation) < 4) canvas.rotation = 0;
                    } else {
                        frame.rotation += wheel.angleDelta.x / 120;
                        if (Math.abs(frame.rotation) < 0.6) frame.rotation = 0;
                        setscale(sc * (1 + wheel.angleDelta.y / 120 / 10));
                    }
                }
            }
        }
    }
}

