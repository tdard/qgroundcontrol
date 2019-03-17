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
    contentHeight:  zoneEditorRect.height
    clip:           true
    visible:        false
    property var    myZoneController
    property var    flightMap

    readonly property real  _editFieldWidth:    Math.min(width - _margin * 2, ScreenTools.defaultFontPixelWidth * 15)
    readonly property real  _margin:            ScreenTools.defaultFontPixelWidth / 2
    readonly property real  _radius:            ScreenTools.defaultFontPixelWidth / 2

    // GDP - Start
    property  var       _coordinate
    property  bool      _clicked:               false
    function setMouseCoord(coordinate)
    {
        textFieldLatitude.text = coordinate.latitude
        textFieldLongitude.text = coordinate.longitude
        _coordinate = coordinate
        _clicked = true
    }
    // GDP - Stop

    Rectangle {
        id:     zoneEditorRect
        anchors.left:   parent.left
        anchors.right:  parent.right
        height: zoneItems.y + zoneItems.height + (_margin * 2)
        radius: _radius
        color:  qgcPal.missionItemEditor

        QGCLabel {
            id:                 zoneLabel
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.top:        parent.top
            text:               qsTr("Zone")
            anchors.leftMargin: ScreenTools.defaultFontPixelWidth
        }

        Rectangle {
            id:                 zoneItems
            anchors.margins:    _margin
            anchors.left:       parent.left
            anchors.right:      parent.right
            anchors.top:        zoneLabel.bottom
            height:             zoneColumn.y + zoneColumn.height + (_margin * 2)
            color:              qgcPal.windowShadeDark
            radius:             _radius

            Column {
                id:                 zoneColumn
                anchors.margins:    _margin
                anchors.top:        parent.top
                anchors.left:       parent.left
                anchors.right:      parent.right
                spacing:            ScreenTools.defaultFontPixelHeight / 2

                QGCLabel {
                    anchors.left:       parent.left
                    anchors.right:      parent.right
                    wrapMode:           Text.WordWrap
                    font.pointSize:     ScreenTools.defaultFontPointSize
                    text:               qsTr("Define the Zone information for BAE Competition Challenge.")
                }

                Column {
                    anchors.left:   parent.left
                    anchors.right:  parent.right
                    spacing:        ScreenTools.defaultFontPixelHeight / 2
                    visible:        true
                    GridLayout{
                        anchors.left:           parent.left
                        anchors.right:          parent.right
                        rows:                   3
                        flow:                   GridLayout.TopToBottom


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
                    }
                    // GDP - Start
                    SectionHeader {
                        id:     rectangleSection
                        text:   qsTr("Polygonal Zones")
                    }

                    GridLayout {
                        anchors.left:           parent.left
                        anchors.right:          parent.right
                        rows:                   3
                        flow:                   GridLayout.TopToBottom

                        QGCButton {
                            id:                 buttonBuildMainZone
                            text:               qsTr("Construct Main")
                            Layout.row:         0
                            onClicked: {
                                var lon = textFieldLongitude.text
                                var lat = textFieldLatitude.text
                                console.log(lon, lat)
                                myZoneController.addMainPolygonZone((_clicked) ? _coordinate : QtPositioning.coordinate(parseFloat(lat), parseFloat(lon)))
                            }
                        }

                        QGCButton {
                            id:                 buttonRotateZone
                            text:               qsTr("Rotate")
                            Layout.row:         1
                            onClicked: {
                                var rot = (textFieldRotation.text)? parseInt(textFieldRotation.text) : 0
                                console.log(rot)
                                myZoneController.rotateZones(0, rot)
                            }
                        }

                        QGCButton {
                            id:                 buttonBuildAttackAndDefense
                            text:               qsTr("Construct Additionals")
                            Layout.row:         2
                            onClicked: {
                                console.log("Build Attack & Defense zones")
                                myZoneController.clearAllInteractive()
                                myZoneController.addZonePolygonAttack()
                                myZoneController.addZonePolygonDefense()
                                myZoneController.sendPolygonZone()
                            }
                        }
                    }
                    // GDP - Stop
                    SectionHeader {
                        id:     circleSection
                        text:   qsTr("Circular Zones")
                    }
                    GridLayout {
                        anchors.left:           parent.left
                        anchors.right:          parent.right
                        rows:                   3
                        flow:                   GridLayout.TopToBottom

                        QGCButton {
                            id:                     buttonBuildMainCircleZone
                            text:                   qsTr("Construct Main")
                            Layout.row:             0
                            onClicked: {
                                var lon = textFieldLongitude.text
                                var lat = textFieldLatitude.text
                                console.log(lon, lat)
                                myZoneController.addMainCircleZone((_clicked) ? _coordinate : QtPositioning.coordinate(parseFloat(lat), parseFloat(lon)))
                            }

                        }
                        QGCButton {
                            id:                 buttonRotateZoneCircle
                            text:               qsTr("Rotate")
                            Layout.row:         1
                            onClicked: {
                                var rot = (textFieldRotation.text)? parseInt(textFieldRotation.text) : 0
                                console.log(rot)
                                myZoneController.rotateZonesCircle(rot)
                            }
                        }
                        QGCButton {
                            id:                 buttonBuildAttackAndDefenseCircle
                            text:               qsTr("Construct Additionals")
                            Layout.row:         2
                            onClicked: {
                                console.log("Build Attack & Defense circular zones")
                                myZoneController.clearAllInteractive()
                                myZoneController.addZoneCircleAttack()
                                myZoneController.addZoneCircleDefense()
                                myZoneController.sendCircleZone()
                            }
                        }
                    }
                    SectionHeader {
                        id:         deleteSection
                        text:       qsTr("Delete All zones")
                    }
                    QGCButton {
                        id:                 buttonDelete
                        text:               qsTr("Delete")
                        onClicked: {
                            console.log("Delete")
                            myZoneController.deleteAll()
                        }
                    }

                }
            }
        }
    } // Rectangle
}
