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
#define ROUND_3_CIRCLE_RADIUS   2.5

class ZoneController : public QObject
{
    Q_OBJECT

public:
    ZoneController(QObject* parent = NULL);

    /// Should be called immediately upon Component.onCompleted.
    Q_INVOKABLE void start(Stratege* stratege, bool flyView);

    Q_PROPERTY(QmlObjectListModel*  zonePolygon                 READ zonePolygon                                        CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneDefenseRound1           READ zoneDefenseRound1                                  CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneAttackRound1            READ zoneAttackRound1                                   CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneDefenseRound2           READ zoneDefenseRound2                                  CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneAttackRound2            READ zoneAttackRound2                                   CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneDefenseRound3           READ zoneDefenseRound3                                  CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  zoneAttackRound3            READ zoneAttackRound3                                   CONSTANT)

    /// Adds a square area defining the boundaries of the challenge
    Q_INVOKABLE void addMainPolygonZone         (QGeoCoordinate center,     int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);
    Q_INVOKABLE void addZoneDefenseRound1       (int numberInAltitude = 3,  int numberInHeight = 2);
    Q_INVOKABLE void addZoneAttackRound1        (int numberInAltitude = 6,  int numberInHeight = 2);
    Q_INVOKABLE void addZoneDefenseRound2       (int numberInAltitude = 6,  int radius = LITTLE_CIRCLE_RADIUS);
    Q_INVOKABLE void addZoneAttackRound2        (int numberInAltitude = 12, int radius = LITTLE_CIRCLE_RADIUS);
    Q_INVOKABLE void addZoneDefenseRound3       (int numberInAltitude = 3,  double radius = ROUND_3_CIRCLE_RADIUS);
    Q_INVOKABLE void addZoneAttackRound3        (int numberInAltitude = 6,  double radius = ROUND_3_CIRCLE_RADIUS);

    /// Rotate all the polygon zones that are present on the map
    Q_INVOKABLE void rotateZones                (int index, int rotation, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);

    /// Clears the interactive bit from all Zone items
    Q_INVOKABLE void clearAllInteractive        (void);
    Q_INVOKABLE void deleteAll                  (void);
    Q_INVOKABLE void sendRound1Zone             (void);
    Q_INVOKABLE void sendRound2Zone             (void);
    Q_INVOKABLE void sendRound3Zone             (void);

    QmlObjectListModel* zonePolygon             (void) { return &_zonePolygon; }
    QmlObjectListModel* zoneDefenseRound1       (void) { return &_zoneDefenseRound1; }
    QmlObjectListModel* zoneAttackRound1        (void) { return &_zoneAttackRound1; }
    QmlObjectListModel* zoneDefenseRound2       (void) { return &_zoneDefenseRound2; }
    QmlObjectListModel* zoneAttackRound2        (void) { return &_zoneAttackRound2; }
    QmlObjectListModel* zoneDefenseRound3       (void) { return &_zoneDefenseRound3; }
    QmlObjectListModel* zoneAttackRound3        (void) { return &_zoneAttackRound3; }



signals:
    void requestZoneRound1FromStratege();
    void sendRound1ZoneToStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zoneDefenseRound1, QList<QGCMapPolygon*> zoneAttackRound1);
    void requestZoneRound2FromStratege();
    void sendRound2ZoneToStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound2, QList<QGCMapCircle*> zoneAttackRound2);
    void requestZoneRound3FromStratege();
    void sendRound3ZoneToStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound3, QList<QGCMapCircle*> zoneAttackRound3);

private slots:
    void setRound1ZoneFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zoneDefenseRound1, QList<QGCMapPolygon*> zoneAttackRound1);
    void setRound2ZoneFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound2, QList<QGCMapCircle*> zoneAttackRound2);
    void setRound3ZoneFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound3, QList<QGCMapCircle*> zoneAttackRound3);

private:
    QmlObjectListModel          _zonePolygon;
    QmlObjectListModel          _zoneDefenseRound1;
    QmlObjectListModel          _zoneAttackRound1;
    QmlObjectListModel          _zoneDefenseRound2;
    QmlObjectListModel          _zoneAttackRound2;
    QmlObjectListModel          _zoneDefenseRound3;
    QmlObjectListModel          _zoneAttackRound3;

    bool                        _flyView;
    int                         _rotation;

};


#endif
