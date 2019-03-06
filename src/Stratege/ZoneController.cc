#include "ZoneController.h"
#include "QGCQGeoCoordinate.h"
#include <QtMath>
#include "QGCGeo.h"
ZoneController::ZoneController(QObject* parent) : QObject(parent)
{
    _flyView = false;
    //Nothing to do
}

void ZoneController::start(bool flyView)
{
    _flyView = flyView;
    _rotation = 0;
}

void ZoneController::addMainPolygonZone(QGeoCoordinate center, int height, int width)
{
    qDebug() << "addMainPolygonZone";

    double halfWidthMeters = width / 2.0;
    double halfHeightMeters = height / 2.0;

    qDebug() << center;

    QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 180);
    QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 180);

    topLeft.setAltitude(0.0);
    topRight.setAltitude(0.0);
    bottomLeft.setAltitude(0.0);
    bottomRight.setAltitude(0.0);

    QGCMapPolygon* zonePolygon = new QGCMapPolygon(this);
    zonePolygon->appendVertex(topLeft);
    zonePolygon->appendVertex(topRight);
    zonePolygon->appendVertex(bottomRight);
    zonePolygon->appendVertex(bottomLeft);
    _zonePolygon.append(zonePolygon);

    qDebug() << "Number of zonePolygon: " << _zonePolygon.count();
    clearAllInteractive();
    zonePolygon->setInteractive(true);
}

void ZoneController::addZonePolygonDefense(int numberInAltitude, int numberInHeight)
{
    qDebug() << "addZonePolygonDefense" ;
    if (_zonePolygon.count() < 0) { return; }

    QGCMapPolygon* mainPolygon =                _zonePolygon.value<QGCMapPolygon*>(0);   //normally, this method cannot be appealed if zonePolygon does not have 1 element
    qDebug() << mainPolygon->center();
    QGeoCoordinate mainTopLeft =                mainPolygon->coordinateList().at(0);
    QGeoCoordinate mainBottomRight =            mainPolygon->coordinateList().at(2);
    QGeoCoordinate mainBottomLeft =             mainPolygon->coordinateList().at(3);


    double halfWidthMeters =                    POLYGON_INFO_DEF_WIDTH / 2.0;
    double halfHeightMeters =                   POLYGON_INFO_HEIGHT / (numberInHeight * 2.0);

    QGeoCoordinate bottom =                     mainBottomLeft.atDistanceAndAzimuth(halfWidthMeters, 90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        for (int j = 0; j < numberInHeight; ++j)
        {
            QGeoCoordinate center =            bottom.atDistanceAndAzimuth( (2*j + 1)*(halfHeightMeters), -_rotation);
            center.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));

            QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, - _rotation);
            QGeoCoordinate topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, 0 - _rotation);
            QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, 180 - _rotation );
            QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90 - _rotation).atDistanceAndAzimuth(halfHeightMeters,  180 - _rotation);

            QGCMapPolygon* zonePolygonDefense = new QGCMapPolygon(this);
            zonePolygonDefense->appendVertex(topLeft);
            zonePolygonDefense->appendVertex(topRight);
            zonePolygonDefense->appendVertex(bottomRight);
            zonePolygonDefense->appendVertex(bottomLeft);

            _zonePolygonDefense.append(zonePolygonDefense);
        }
    }
    clearAllInteractive();
}


void ZoneController::addZonePolygonAttack(int numberInAltitude, int numberInHeight)
{
    qDebug() << "addZonePolygonAttack" ;
    if (_zonePolygon.count() < 0) { return; }

    QGCMapPolygon* mainPolygon =                _zonePolygon.value<QGCMapPolygon*>(0);   //normally, this method cannot be appealed if zonePolygon does not have 1 element
    qDebug() << mainPolygon->center();
    QGeoCoordinate mainTopLeft =                mainPolygon->coordinateList().at(0);
    QGeoCoordinate mainBottomRight =            mainPolygon->coordinateList().at(2);
    QGeoCoordinate mainBottomLeft =             mainPolygon->coordinateList().at(3);


    double halfWidthMeters =                    POLYGON_INFO_DEF_WIDTH / 2.0;
    double halfHeightMeters =                   POLYGON_INFO_HEIGHT / (numberInHeight * 2.0);

    QGeoCoordinate bottom =                     mainBottomRight.atDistanceAndAzimuth(halfWidthMeters, -90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        for (int j = 0; j < numberInHeight; ++j)
        {
            QGeoCoordinate center =            bottom.atDistanceAndAzimuth( (2*j + 1)*(halfHeightMeters), -_rotation);
            center.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));

            QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, - _rotation);
            QGeoCoordinate topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, 0 - _rotation);
            QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90 - _rotation).atDistanceAndAzimuth(halfHeightMeters, 180 - _rotation );
            QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90 - _rotation).atDistanceAndAzimuth(halfHeightMeters,  180 - _rotation);

            QGCMapPolygon* zonePolygonAttack = new QGCMapPolygon(this);
            zonePolygonAttack->appendVertex(topLeft);
            zonePolygonAttack->appendVertex(topRight);
            zonePolygonAttack->appendVertex(bottomRight);
            zonePolygonAttack->appendVertex(bottomLeft);

            _zonePolygonAttack.append(zonePolygonAttack);
        }
    }
    clearAllInteractive();
}

void ZoneController::rotateZones(int index, int rotation, int height, int width)
{
    qDebug() << "Rotate Zones";
    _rotation = rotation;

    double halfWidthMeters = width / 2.0;
    double halfHeightMeters = height / 2.0;

    //Get center
    QGCMapPolygon* zonePolygon = _zonePolygon.value<QGCMapPolygon*>(index);
    QGeoCoordinate center = zonePolygon->center();

    //Move each vertex of the desired angle
    const QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(sqrt(halfWidthMeters*halfWidthMeters + halfHeightMeters*halfHeightMeters), -45 - rotation);
    const QGeoCoordinate topRight =          center.atDistanceAndAzimuth(sqrt(halfWidthMeters*halfWidthMeters + halfHeightMeters*halfHeightMeters), 45 - rotation);
    const QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(sqrt(halfWidthMeters*halfWidthMeters + halfHeightMeters*halfHeightMeters), -135 - rotation);
    const QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(sqrt(halfWidthMeters*halfWidthMeters + halfHeightMeters*halfHeightMeters), 135 - rotation);

    zonePolygon->adjustVertex(0, topLeft);
    zonePolygon->adjustVertex(1, topRight);
    zonePolygon->adjustVertex(2, bottomRight);
    zonePolygon->adjustVertex(3, bottomLeft);

    clearAllInteractive();
}

void ZoneController::deleteAll()
{
    qDebug() << "deleteAll";
    _zonePolygon.clearAndDeleteContents();
    _zonePolygonDefense.clearAndDeleteContents();
    _zonePolygonAttack.clearAndDeleteContents();
}

void ZoneController::clearAllInteractive(void)
{
    qDebug() << "clearAllInteractive";
    for (int i=0; i<_zonePolygon.count(); i++) {
        _zonePolygon.value<QGCMapPolygon*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zonePolygonDefense.count(); i++) {
        _zonePolygonDefense.value<QGCMapPolygon*>(i)->setInteractive(false);
    }
}


