import QtQuick          2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts  1.2
import QtPositioning    5.2

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0

QGCFlickable {
    id:             root
    contentHeight:  geoFenceEditorRect.height
    clip:           true

    property var    myGeoFenceController
    property var    flightMap

    readonly property real  _editFieldWidth:    Math.min(width - _margin * 2, ScreenTools.defaultFontPixelWidth * 15)
    readonly property real  _margin:            ScreenTools.defaultFontPixelWidth / 2
    readonly property real  _radius:            ScreenTools.defaultFontPixelWidth / 2

    // GDP - Start
    function setMouseCoord(coordinate)
    {
        textFieldLatitude.text = coordinate.latitude
        textFieldLongitude.text = coordinate.longitude
    }
    // GDP - Stop

    Rectangle {
        id:     geoFenceEditorRect
        anchors.left:   parent.left
        anchors.right:  parent.right
        height: geoFenceItems.y + geoFenceItems.height + (_margin * 2)
        radius: _radius
        color:  qgcPal.missionItemEditor

        QGCLabel {
            id:                 geoFenceLabel
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.top:        parent.top
            text:               qsTr("GeoFence")
            anchors.leftMargin: ScreenTools.defaultFontPixelWidth
        }

        Rectangle {
            id:                 geoFenceItems
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.right:      parent.right
            anchors.top:        geoFenceLabel.bottom
            height:             fenceColumn.y + fenceColumn.height + (_margin * 2)
            color:              qgcPal.windowShadeDark
            radius:             _radius

            Column {
                id:                 fenceColumn
                anchors.margins:    _margin
                anchors.top:        parent.top
                anchors.left:       parent.left
                anchors.right:      parent.right
                spacing:            ScreenTools.defaultFontPixelHeight / 2

                QGCLabel {
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    wrapMode:           Text.WordWrap
                    font.pointSize:     myGeoFenceController.supported ? ScreenTools.smallFontPointSize : ScreenTools.defaultFontPointSize
                    text:               myGeoFenceController.supported ?
                                            qsTr("GeoFencing allows you to set a virtual ‘fence’ around the area you want to fly in.") :
                                            qsTr("This vehicle does not support GeoFence.")
                }

                Column {
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    spacing:        ScreenTools.defaultFontPixelHeight / 2
                    visible:        myGeoFenceController.supported

                    Repeater {
                        model: myGeoFenceController.params

                        Item {
                            width:  fenceColumn.width
                            height: textField.height

                            property bool showCombo: modelData.enumStrings.length > 0

                            QGCLabel {
                                id:                 textFieldLabel
                                anchors.baseline:   textField.baseline
                                text:               myGeoFenceController.paramLabels[index]
                            }

                            FactTextField {
                                id:             textField
                                anchors.right:  parent.right
                                width:          _editFieldWidth
                                showUnits:      true
                                fact:           modelData
                                visible:        !parent.showCombo
                            }

                            FactComboBox {
                                id:             comboField
                                anchors.right:  parent.right
                                width:          _editFieldWidth
                                indexModel:     false
                                fact:           showCombo ? modelData : _nullFact
                                visible:        parent.showCombo

                                property var _nullFact: Fact { }
                            }
                        }
                    }

                    SectionHeader {
                        id:     insertSection
                        text:   qsTr("Insert GeoFence")
                    }

                    QGCButton {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        text:           qsTr("Polygon Fence")

                        onClicked: {
                            var rect = Qt.rect(flightMap.centerViewport.x, flightMap.centerViewport.y, flightMap.centerViewport.width, flightMap.centerViewport.height)
                            var topLeftCoord = flightMap.toCoordinate(Qt.point(rect.x, rect.y), false /* clipToViewPort */)
                            var bottomRightCoord = flightMap.toCoordinate(Qt.point(rect.x + rect.width, rect.y + rect.height), false /* clipToViewPort */)
                            myGeoFenceController.addInclusionPolygon(topLeftCoord, bottomRightCoord)
                        }
                    }

                    QGCButton {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        text:           qsTr("Circular Fence")

                        onClicked: {
                            var rect = Qt.rect(flightMap.centerViewport.x, flightMap.centerViewport.y, flightMap.centerViewport.width, flightMap.centerViewport.height)
                            var topLeftCoord = flightMap.toCoordinate(Qt.point(rect.x, rect.y), false /* clipToViewPort */)
                            var bottomRightCoord = flightMap.toCoordinate(Qt.point(rect.x + rect.width, rect.y + rect.height), false /* clipToViewPort */)
                            myGeoFenceController.addInclusionCircle(topLeftCoord, bottomRightCoord)
                        }
                    }

                    SectionHeader {
                        id:     polygonSection
                        text:   qsTr("Polygon Fences")
                    }

                    QGCLabel {
                        text:       qsTr("None")
                        visible:    polygonSection.checked && myGeoFenceController.polygons.count === 0
                    }

                    GridLayout {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        columns:        3
                        flow:           GridLayout.TopToBottom
                        visible:        polygonSection.checked && myGeoFenceController.polygons.count > 0

                        QGCLabel {
                            text:               qsTr("Inclusion")
                            Layout.column:      0
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.polygons

                            QGCCheckBox {
                                checked:            object.inclusion
                                onClicked:          object.inclusion = checked
                                Layout.alignment:   Qt.AlignHCenter
                            }
                        }

                        QGCLabel {
                            text:               qsTr("Edit")
                            Layout.column:      1
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.polygons

                            QGCRadioButton {
                                checked:            _interactive
                                Layout.alignment:   Qt.AlignHCenter

                                property bool _interactive: object.interactive

                                on_InteractiveChanged: checked = _interactive

                                onClicked: {
                                    myGeoFenceController.clearAllInteractive()
                                    object.interactive = checked
                                }
                            }
                        }

                        QGCLabel {
                            text:               qsTr("Delete")
                            Layout.column:      2
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.polygons

                            QGCButton {
                                text:               qsTr("Del")
                                Layout.alignment:   Qt.AlignHCenter
                                onClicked:          myGeoFenceController.deletePolygon(index)
                            }
                        }
                    } // GridLayout



                    SectionHeader {
                        id:     circleSection
                        text:   qsTr("Circular Fences")
                    }

                    QGCLabel {
                        text:       qsTr("None")
                        visible:    circleSection.checked && myGeoFenceController.circles.count === 0
                    }

                    GridLayout {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        columns:        4
                        flow:           GridLayout.TopToBottom
                        visible:        polygonSection.checked && myGeoFenceController.circles.count > 0

                        QGCLabel {
                            text:               qsTr("Inclusion")
                            Layout.column:      0
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.circles

                            QGCCheckBox {
                                checked:            object.inclusion
                                onClicked:          object.inclusion = checked
                                Layout.alignment:   Qt.AlignHCenter
                            }
                        }

                        QGCLabel {
                            text:               qsTr("Edit")
                            Layout.column:      1
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.circles

                            QGCRadioButton {
                                checked:            _interactive
                                Layout.alignment:   Qt.AlignHCenter

                                property bool _interactive: object.interactive

                                on_InteractiveChanged: checked = _interactive

                                onClicked: {
                                    myGeoFenceController.clearAllInteractive()
                                    object.interactive = checked
                                }
                            }
                        }

                        QGCLabel {
                            text:               qsTr("Radius")
                            Layout.column:      2
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.circles

                            FactTextField {
                                fact:               object.radius
                                Layout.fillWidth:   true
                                Layout.alignment:   Qt.AlignHCenter
                            }
                        }

                        QGCLabel {
                            text:               qsTr("Delete")
                            Layout.column:      3
                            Layout.alignment:   Qt.AlignHCenter
                        }

                        Repeater {
                            model: myGeoFenceController.circles

                            QGCButton {
                                text:               qsTr("Del")
                                Layout.alignment:   Qt.AlignHCenter
                                onClicked:          myGeoFenceController.deleteCircle(index)
                            }
                        }
                    } // GridLayout

                    // GDP - Start
                    SectionHeader {
                        id:     rectangleSection
                        text:   qsTr("GeoFence Competition")
                    }

                    GridLayout {
                        anchors.left:   parent.left
                        anchors.right:  parent.right
                        rows:           4
                        flow:           GridLayout.TopToBottom

                        QGCTextField {
                            id:                 textFieldLatitude
                            placeholderText:    qsTr("Origin Latitude")

                            validator:          RegExpValidator{ regExp: /^(\+|-)?(?:90(?:(?:\.0{1,6})?)|(?:[0-9]|[1-8][0-9])(?:(?:\.[0-9]{1,6})?))$/ }
                            Layout.row:         0
                        }

                        QGCTextField {
                            id:                 textFieldLongitude
                            placeholderText:    qsTr("Origin Longitude")
                            validator:          RegExpValidator{ regExp: /^(\+|-)?(?:180(?:(?:\.0{1,6})?)|(?:[0-9]|[1-9][0-9]|1[0-7][0-9])(?:(?:\.[0-9]{1,6})?))$/}
                            Layout.row:         1
                        }

                        QGCTextField {
                            id:                 textFieldRotation
                            placeholderText:    qsTr("Rotation (°)")
                            validator:          IntValidator{}
                            Layout.row  :       2
                        }


                        QGCButton {
                            anchors.left:   parent.left
                            anchors.right:  parent.right
                            text:           qsTr("Build Polygon Fence")

                            onClicked: {
                                var lon = textFieldLongitude.text
                                var lat = textFieldLatitude.text
                                var rot = textFieldRotation.text
                                myGeoFenceController.buildCompetitionPolygonFence(lat, lon, rot)

                                var def_lat = -120.083923
                                var def_lon = 38.965767

                                var point = flightMap.fromCoordinate(QtPositioning.coordinate(def_lat, def_lon), false /* clipToViewPort */)

                                var rect = Qt.rect(point.x, point.y, flightMap.centerViewport.width, flightMap.centerViewport.height)



//                                var topLeftCoord = flightMap.toCoordinate(Qt.point(rect.x, rect.y), false /* clipToViewPort */)
//                                var bottomRightCoord = flightMap.toCoordinate(Qt.point(rect.x + rect.width, rect.y + rect.height), false /* clipToViewPort */)
//                                myGeoFenceController.addInclusionPolygon(topLeftCoord, bottomRightCoord)

                                console.log("position desiree", rect.x, rect.y)

                                rect = Qt.rect(flightMap.centerViewport.x, flightMap.centerViewport.y, flightMap.centerViewport.width, flightMap.centerViewport.height)


                                var coordinateLog = flightMap.toCoordinate(Qt.point(rect.x, rect.y), false /* clipToViewPort */)
                                console.log(coordinateLog)
                                console.log(coordinateLog.latitude, coordinateLog.longitude)

                                //atDistanceAndAzimuth(distance, azimuth)
                                //map.toCoordinate(Qt.point(mouse.x, mouse.y), false /* clipToViewPort */)
                                console.log("position reelle ",rect.x, rect.y)
                                var topLeftCoord = flightMap.toCoordinate(Qt.point(rect.x, rect.y), false /* clipToViewPort */)
                                var bottomRightCoord = flightMap.toCoordinate(Qt.point(rect.x + rect.width, rect.y + rect.height), false /* clipToViewPort */)
                                myGeoFenceController.addInclusionPolygon(topLeftCoord, bottomRightCoord)

                            }
                        }
                    }
                    // GDP - Stop
                }
            }
        }
    } // Rectangle
    Text {
        id: info
        anchors.left:   parent.left
        anchors.right:  parent.right
        text: qsTr("text")
    }
    MouseArea {
        anchors.fill: flightMap
        onPressed: {
            if (mouse.button == Qt.LeftButton) {info.text = "yess"}
        }
    }
}
