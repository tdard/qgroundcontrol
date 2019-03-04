#include "ZoneController.h"
#include "QGCQGeoCoordinate.h"

ZoneController::ZoneController(QObject* parent)
{
    _flyView = false;
    //Nothing to do
}

void ZoneController::start(bool flyView)
{

}

void ZoneController::addMainPolygonZone(QGeoCoordinate center, int height, int width)
{
    qDebug() << "addMainPolygonZone";

    double halfWidthMeters = width / 2.0;
    double halfHeightMeters = height / 2.0;

    QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 180);
    QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 180);

    QGCMapPolygon* zonePolygon = new QGCMapPolygon(this);
    zonePolygon->appendVertex(topLeft);
    zonePolygon->appendVertex(topRight);
    zonePolygon->appendVertex(bottomRight);
    zonePolygon->appendVertex(bottomLeft);
    _zonePolygons.append(zonePolygon);

    qDebug() << "Number of zonePolygons: " << _zonePolygons.count();

    clearAllInteractive();
//    zonePolygon->setInteractive(true);
}

void ZoneController::rotateZones(int index, int rotation, int height, int width)
{
    qDebug() << "rotateZones";
    qDebug() << rotation;

    double halfWidthMeters = width / 2.0;
    double halfHeightMeters = height / 2.0;

    //Get center
    QGCMapPolygon* zonePolygon = _zonePolygons.value<QGCMapPolygon*>(index);
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
    //zonePolygon->setInteractive(true);
}

void ZoneController::deleteAll()
{
    qDebug() << "deleteAll";
}

void ZoneController::clearAllInteractive()
{
    qDebug() << "clearAllInteractive";
}
