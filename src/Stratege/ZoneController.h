#pragma once

#ifndef ZoneController_H
#define ZoneController_H

#include "QGCFencePolygon.h"
#include "QGCFenceCircle.h"

#define POLYGON_INFO_WIDTH  100
#define POLYGON_INFO_HEIGHT 100
#define M_DEG_TO_RAD (M_PI / 180.0)

class ZoneController : public QObject
{
    Q_OBJECT

public:
    ZoneController(QObject* parent = NULL);

    /// Should be called immediately upon Component.onCompleted.
    Q_INVOKABLE void start(bool flyView);

//    Q_PROPERTY(QmlObjectListModel*  zonePolygons        READ zonePolygons                                   CONSTANT)

    /// Adds a square area defining the boundaries of the challenge
    Q_INVOKABLE void addMainPolygonZone     (QGeoCoordinate center, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);


    /// Rotate all the zones that are present on the map
    Q_INVOKABLE void rotateZones             (int index, int rotation, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);

    /// Clears the interactive bit from all Zone items
    Q_INVOKABLE void clearAllInteractive    (void);

    Q_INVOKABLE void deleteAll();

    QmlObjectListModel* zonePolygons        (void) { return &_zonePolygons; }

signals:
    void updateZones();     //To connect with the Stratege

private:
    QmlObjectListModel _zonePolygons;   //Stratege will use QGCMapPolygon instead of QmlObjectListModel
    bool                    _flyView;


};


#endif
