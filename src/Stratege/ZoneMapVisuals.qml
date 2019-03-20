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
    property var    _zoneDefenseRound1:         myZoneController.zoneDefenseRound1
    property var    _zoneAttackRound1:          myZoneController.zoneAttackRound1
    property var    _zoneDefenseRound2:         myZoneController.zoneDefenseRound2
    property var    _zoneAttackRound2:          myZoneController.zoneAttackRound2
    property var    _zoneDefenseRound3:         myZoneController.zoneDefenseRound3
    property var    _zoneAttackRound3:          myZoneController.zoneAttackRound3
    property color  _borderColor:               "grey"
    property int    _borderWidthInclusion:      2
    property int    _borderWidthExclusion:      0
    property color  _interiorColorExclusion:    "grey"
    property color  _interiorColorInclusion:    "transparent"
    property real   _interiorOpacityExclusion:  0.25
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
        model: _zoneDefenseRound1

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
        model: _zoneAttackRound1

        delegate : QGCMapPolygonVisuals {
            mapControl:         map
            mapPolygon:         object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        _borderColor
            interiorColor:      "red"
            interiorOpacity:    0.15
        }
    }



    Instantiator {
        model: _zoneDefenseRound2

        delegate : QGCMapCircleVisuals {
            mapControl:         map
            mapCircle:          object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        "blue"
            interiorColor:      "blue"
            interiorOpacity:    0.10
        }
    }

    Instantiator {
        model: _zoneAttackRound2

        delegate : QGCMapCircleVisuals {
            mapControl:         map
            mapCircle:          object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        "red"
            interiorColor:      "red"
            interiorOpacity:    0.07
        }
    }

    Instantiator {
        model: _zoneDefenseRound3

        delegate : QGCMapCircleVisuals {
            mapControl:         map
            mapCircle:          object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        "blue"
            interiorColor:      "blue"
            interiorOpacity:    0.20
        }
    }

    Instantiator {
        model: _zoneAttackRound3

        delegate : QGCMapCircleVisuals {
            mapControl:         map
            mapCircle:          object
            borderWidth:        object.inclusion ? _borderWidthInclusion : _borderWidthExclusion
            borderColor:        "red"
            interiorColor:      "red"
            interiorOpacity:    0.15
        }
    }



}
