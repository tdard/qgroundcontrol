#pragma once

#ifndef ZoneController_H
#define ZoneController_H

#include "QGCFencePolygon.h"
#include "QGCFenceCircle.h"

#define MAX_ALTITUDE            120
#define POLYGON_INFO_WIDTH      100
#define POLYGON_INFO_HEIGHT     100
#define POLYGON_INFO_DEF_WIDTH  25
#define POLYGON_INFO_DEF_HEIGHT 25
#define M_DEG_TO_RAD (M_PI / 180.0)

class ZoneController : public QObject
{
    Q_OBJECT

public:
    ZoneController(QObject* parent = NULL);

    /// Should be called immediately upon Component.onCompleted.
    Q_INVOKABLE void start(bool flyView);

    Q_PROPERTY(QmlObjectListModel*  zonePolygon                 READ zonePolygon                                        CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zonePolygonDefense          READ zonePolygonDefense                                 CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zonePolygonAttack           READ zonePolygonAttack                                  CONSTANT)

    /// Adds a square area defining the boundaries of the challenge
    Q_INVOKABLE void addMainPolygonZone         (QGeoCoordinate center, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);
    Q_INVOKABLE void addZonePolygonDefense      (int numberInAltitude = 3, int numberInHeight = 2); // create 6 zones of defense (default)
    Q_INVOKABLE void addZonePolygonAttack       (int numberInAltitude = 4, int numberInHeight = 2); // create 8 zones of attack (default)
    /// Rotate all the zones that are present on the map
    Q_INVOKABLE void rotateZones                (int index, int rotation, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);

    /// Clears the interactive bit from all Zone items
    Q_INVOKABLE void clearAllInteractive        (void);
    Q_INVOKABLE void deleteAll();

    QmlObjectListModel* zonePolygon             (void) { return &_zonePolygon; }
    QmlObjectListModel* zonePolygonDefense      (void) { return &_zonePolygonDefense; }
    QmlObjectListModel* zonePolygonAttack       (void) { return &_zonePolygonAttack; }
signals:
    void updateZones();     //To connect with the Stratege. Only notifys the stratege that the zone has changed. The stratege will then access to the value through its private variable _zoneController

private:
    QmlObjectListModel          _zonePolygon;   //Stratege will use QGCMapPolygon instead of QmlObjectListModel
    QmlObjectListModel          _zonePolygonDefense;
    QmlObjectListModel          _zonePolygonAttack;
    bool                        _flyView;
    int                         _rotation;


};


#endif
