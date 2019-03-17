#pragma once

#ifndef ZoneController_H
#define ZoneController_H

//#include "QGCFencePolygon.h"
//#include "QGCFenceCircle.h"
#include "QGCMapPolygon.h"
#include "QGCMapCircle.h"

#include "Stratege.h"

#define MAX_ALTITUDE            120
#define POLYGON_INFO_WIDTH      100
#define POLYGON_INFO_HEIGHT     100
#define POLYGON_INFO_DEF_WIDTH  25
#define POLYGON_INFO_DEF_HEIGHT 25
#define M_DEG_TO_RAD (M_PI / 180.0)
#define MAIN_CIRCLE_RADIUS      50
#define LITTLE_CIRCLE_RADIUS    5

class ZoneController : public QObject
{
    Q_OBJECT

public:
    ZoneController(QObject* parent = NULL);

    /// Should be called immediately upon Component.onCompleted.
    Q_INVOKABLE void start(Stratege* stratege, bool flyView);

    Q_PROPERTY(QmlObjectListModel*  zonePolygon                 READ zonePolygon                                        CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zonePolygonDefense          READ zonePolygonDefense                                 CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zonePolygonAttack           READ zonePolygonAttack                                  CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneCircle                  READ zoneCircle                                         CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneCircleDefense           READ zoneCircleDefense                                  CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneCircleAttack            READ zoneCircleAttack                                   CONSTANT)

    /// Adds a square area defining the boundaries of the challenge
    Q_INVOKABLE void addMainPolygonZone         (QGeoCoordinate center, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);
    Q_INVOKABLE void addZonePolygonDefense      (int numberInAltitude = 3, int numberInHeight = 2); // create 6 zones of defense (default)
    Q_INVOKABLE void addZonePolygonAttack       (int numberInAltitude = 6, int numberInHeight = 2); // create 12 zones of attack (default)

    Q_INVOKABLE void addMainCircleZone          (QGeoCoordinate center, int radius = MAIN_CIRCLE_RADIUS);
    Q_INVOKABLE void addZoneCircleDefense       (int numberInAltitude = 6, int radius = LITTLE_CIRCLE_RADIUS);
    Q_INVOKABLE void addZoneCircleAttack        (int numberInAltitude = 12, int radius = LITTLE_CIRCLE_RADIUS);


    /// Rotate all the polygon zones that are present on the map
    Q_INVOKABLE void rotateZones                (int index, int rotation, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);
    Q_INVOKABLE void rotateZonesCircle          (int rotation);

    /// Clears the interactive bit from all Zone items
    Q_INVOKABLE void clearAllInteractive        (void);
    Q_INVOKABLE void deleteAll                  (void);
    Q_INVOKABLE void sendPolygonZone            (void);
    Q_INVOKABLE void sendCircleZone             (void);

    QmlObjectListModel* zonePolygon             (void) { return &_zonePolygon; }
    QmlObjectListModel* zonePolygonDefense      (void) { return &_zonePolygonDefense; }
    QmlObjectListModel* zonePolygonAttack       (void) { return &_zonePolygonAttack; }
    QmlObjectListModel* zoneCircle              (void) { return &_zoneCircle; }
    QmlObjectListModel* zoneCircleDefense       (void) { return &_zoneCircleDefense; }
    QmlObjectListModel* zoneCircleAttack        (void) { return &_zoneCircleAttack; }



signals:
    void requestZonePolygonFromStratege();
    void sendPolygonZoneToStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);
    void requestZoneCircleFromStratege();
    void sendCircleZoneToStratege(QList<QGCMapCircle*> mainZoneCircle, QList<QGCMapCircle*> zoneCircleDefense, QList<QGCMapCircle*> zoneCircleAttack);

private slots:
    void setZonePolygonFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);
    void setZoneCircleFromStratege(QList<QGCMapCircle*> mainZoneCircle, QList<QGCMapCircle*> zoneCircleDefense, QList<QGCMapCircle*> zoneCircleAttack);

private:
    QmlObjectListModel          _zonePolygon;   //Stratege will use QGCMapPolygon instead of QmlObjectListModel
    QmlObjectListModel          _zonePolygonDefense;
    QmlObjectListModel          _zonePolygonAttack;
    QmlObjectListModel          _zoneCircle;
    QmlObjectListModel          _zoneCircleDefense;
    QmlObjectListModel          _zoneCircleAttack;
    bool                        _flyView;
    int                         _rotation;

};


#endif
