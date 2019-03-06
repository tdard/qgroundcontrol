/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

import QtQuick          2.3
import QtQuick.Controls 1.2
import QtLocation       5.3
import QtPositioning    5.3

import QGroundControl               1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.Controls      1.0
import QGroundControl.FlightMap     1.0

/// GeoFence map visuals
Item {
    z: QGroundControl.zOrderMapItems

    property var    map
    property var    myZoneController
    property bool   interactive:            false   ///< true: user can interact with items
    property bool   planView:               false   ///< true: visuals showing in plan view
    property var    homePosition


    property var    _paramCircleFenceComponent
    property var    _zonePolygon:               myZoneController.zonePolygon
    property var    _zonePolygonDefense:        myZoneController.zonePolygonDefense
    property var    _zonePolygonAttack:         myZoneController.zonePolygonAttack
    property color  _borderColor:               "grey"
    property int    _borderWidthInclusion:      2
    property int    _borderWidthExclusion:      0
    property color  _interiorColorExclusion:    "grey"
    property color  _interiorColorInclusion:    "transparent"
    property real   _interiorOpacityExclusion:  0.2
    property real   _interiorOpacityInclusion:  1


    Instantiator {
        model: _zonePolygon

        delegate : QGCMapPolygonVisuals {
            mapControl:         map
            mapPolygon:         object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        _borderColor
            interiorColor:      object.inclusion ? _interiorColorInclusion : _interiorColorExclusion
            interiorOpacity:    object.inclusion ? _interiorOpacityInclusion : _interiorOpacityExclusion
        }
    }

    Instantiator {
        model: _zonePolygonDefense

        delegate : QGCMapPolygonVisuals {
            mapControl:         map
            mapPolygon:         object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        _borderColor
            interiorColor:      "blue"
            interiorOpacity:    0.2
        }
    }

    Instantiator {
        model: _zonePolygonAttack

        delegate : QGCMapPolygonVisuals {
            mapControl:         map
            mapPolygon:         object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        _borderColor
            interiorColor:      "red"
            interiorOpacity:    0.15
        }
    }

//    Instantiator {
//        model: _circles

//        delegate : QGCMapCircleVisuals {
//            mapControl:         map
//            mapCircle:          object
//            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
//            borderColor:        _borderColor
//            interiorColor:      object.inclusion ? _interiorColorInclusion : _interiorColorExclusion
//            interiorOpacity:    object.inclusion ? _interiorOpacityInclusion : _interiorOpacityExclusion
//        }
//    }

//    // GDP - Start
//    Instantiator {
//        model: _zonePolygonInfo

//        delegate : QGCMapPolygonVisuals {
//            mapControl:         map
//            mapPolygon:         object
//            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
//            borderColor:        "darkmagenta"
//            interiorColor:      object.inclusion ? _interiorColorInclusion : _interiorColorExclusion
//            interiorOpacity:    object.inclusion ? _interiorOpacityInclusion : _interiorOpacityExclusion
//        }
//    }
    // GDP - Stop

//    // Circular geofence specified from parameter
//    Component {
//        id: paramCircleFenceComponent

//        MapCircle {
//            color:          _interiorColorInclusion
//            opacity:        _interiorOpacityInclusion
//            border.color:   _borderColor
//            border.width:   _borderWidthInclusion
//            center:         homePosition
//            radius:         _radius
//            visible:        homePosition.isValid && _radius > 0

//            property real _radius: myGeoFenceController.paramCircularFence

//            on_RadiusChanged: console.log("_radius", _radius, homePosition.isValid, homePosition)
//        }
//    }

}
