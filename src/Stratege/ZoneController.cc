#include "ZoneController.h"
#include "QGCQGeoCoordinate.h"
#include <QtMath>
#include "QGCGeo.h"

ZoneController::ZoneController(QObject* parent) : QObject(parent)
{
    _flyView = false;
}

void ZoneController::start(Stratege* stratege, bool flyView)
{
    qDebug() << "ZoneController start";

    _flyView = flyView;
    _rotation = 0;

    connect(this, &ZoneController::requestZonePolygonFromStratege, stratege, &Stratege::handleZoneControllerRequestPolygon);
    connect(this, &ZoneController::sendPolygonZoneToStratege, stratege, &Stratege::setPolygonZoneFromController);
    connect(stratege, &Stratege::sendPolygonToZoneController, this, &ZoneController::setZonePolygonFromStratege);

    connect(this, &ZoneController::requestZoneCircleFromStratege, stratege, &Stratege::handleZoneControllerRequestCircle);
    connect(this, &ZoneController::sendCircleZoneToStratege, stratege, &Stratege::setCircleZoneFromController);
    connect(stratege, &Stratege::sendCircleToZoneController, this, &ZoneController::setZoneCircleFromStratege);
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
}

void ZoneController::addMainCircleZone(QGeoCoordinate center, int radius)
{
    QGCMapCircle* zoneCircle = new QGCMapCircle(center, radius);
    center.setAltitude(0.0);
    _zoneCircle.append(zoneCircle);
    clearAllInteractive();
    zoneCircle->setInteractive(true);
}

void ZoneController::addZoneCircleDefense(int numberInAltitude, int radius)
{
    QGeoCoordinate center = _zoneCircle.value<QGCMapCircle*>(0)->center();

    qDebug() << "rotation defense:" << _rotation;

    double halfRadius = _zoneCircle.value<QGCMapCircle*>(0)->radius()->rawValue().toDouble() / 2.0;
    QGeoCoordinate defenseCenter = center.atDistanceAndAzimuth(halfRadius, -90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        defenseCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneCircleDefense = new QGCMapCircle(defenseCenter, radius);
        _zoneCircleDefense.append(zoneCircleDefense);
    }
}

void ZoneController::addZoneCircleAttack(int numberInAltitude, int radius)
{
    QGeoCoordinate center = _zoneCircle.value<QGCMapCircle*>(0)->center();

    double halfRadius = _zoneCircle.value<QGCMapCircle*>(0)->radius()->rawValue().toDouble() / 2.0;
    QGeoCoordinate attackCenter = center.atDistanceAndAzimuth(halfRadius, 90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        attackCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneCircleAttack = new QGCMapCircle(attackCenter, radius);
        _zoneCircleAttack.append(zoneCircleAttack);
    }
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
}

void ZoneController::rotateZonesCircle(int rotation)
{
    qDebug() << "Rotate Zones Circle";
    _rotation = rotation;
}

void ZoneController::deleteAll(void)
{
    qDebug() << "deleteAll";
    _zoneCircle.clear();
    _zoneCircleAttack.clear();
    _zoneCircleDefense.clear();
    sendCircleZone();
    _zonePolygon.clear();
    _zonePolygonAttack.clear();
    _zonePolygonDefense.clear();
    sendPolygonZone();
}

void ZoneController::sendPolygonZone(void)
{
    qDebug() << "sendPolygonZoneToStratege";

    QList<QGCMapPolygon*> zonePolygon = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygon.count(); ++i)
    {
        zonePolygon.append(_zonePolygon.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapPolygon*> zonePolygonDefense = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygonDefense.count(); ++i)
    {
        zonePolygonDefense.append(_zonePolygonDefense.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapPolygon*> zonePolygonAttack = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygonAttack.count(); ++i)
    {
        zonePolygonAttack.append(_zonePolygonAttack.value<QGCMapPolygon*>(i));
    }

    emit sendPolygonZoneToStratege(zonePolygon, zonePolygonDefense, zonePolygonAttack);
}

void ZoneController::sendCircleZone(void)
{
    qDebug() << "sendCircleZoneToStratege";

    QList<QGCMapCircle*> zoneCircle = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneCircle.count(); ++i)
    {
        zoneCircle.append(_zoneCircle.value<QGCMapCircle*>(i));
    }

    QList<QGCMapCircle*> zoneCircleDefense = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneCircleDefense.count(); ++i)
    {
        zoneCircleDefense.append(_zoneCircleDefense.value<QGCMapCircle*>(i));
    }

    QList<QGCMapCircle*> zoneCircleAttack = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneCircleAttack.count(); ++i)
    {
        zoneCircleAttack.append(_zoneCircleAttack.value<QGCMapCircle*>(i));
    }

    emit sendCircleZoneToStratege(zoneCircle, zoneCircleDefense, zoneCircleAttack);
}

void ZoneController::clearAllInteractive(void)
{
    qDebug() << "clearAllInteractive";

    //Polygons
    for (int i=0; i<_zonePolygon.count(); i++) {
        _zonePolygon.value<QGCMapPolygon*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zonePolygonDefense.count(); i++) {
        _zonePolygonDefense.value<QGCMapPolygon*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zonePolygonAttack.count(); i++) {
        _zonePolygonAttack.value<QGCMapPolygon*>(i)->setInteractive(false);
    }

    //Circles
    for (int i=0; i<_zoneCircle.count(); i++) {
        _zoneCircle.value<QGCMapCircle*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zoneCircleDefense.count(); i++) {
        _zoneCircleDefense.value<QGCMapCircle*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zoneCircleAttack.count(); i++) {
        _zoneCircleAttack.value<QGCMapCircle*>(i)->setInteractive(false);
    }
}

void ZoneController::setZonePolygonFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack)
{
    qDebug() << "setZonePolygonFromStratege, flyView:" << _flyView;
    if (_flyView == false) { return; }

    _zonePolygon.clear();
    _zonePolygonAttack.clear();
    _zonePolygonDefense.clear();

    for (int i = 0; i < mainZonePolygon.count(); ++i)
    {
        _zonePolygon.append(mainZonePolygon.at(i));
    }

    for (int i = 0; i < zonePolygonDefense.count(); ++i)
    {
        _zonePolygonDefense.append(zonePolygonDefense.at(i));
    }

    for (int i = 0; i < zonePolygonAttack.count(); ++i)
    {
        _zonePolygonAttack.append(zonePolygonAttack.at(i));
    }
}

void ZoneController::setZoneCircleFromStratege(QList<QGCMapCircle*> mainZoneCircle, QList<QGCMapCircle*> zoneCircleDefense, QList<QGCMapCircle*> zoneCircleAttack)
{
    qDebug() << "setZoneCircleFromStratege, flyView:" << _flyView;
    if (_flyView == false) { return; }

    _zoneCircle.clear();
    _zoneCircleAttack.clear();
    _zoneCircleDefense.clear();

    for (int i = 0; i < mainZoneCircle.count(); ++i)
    {
        _zoneCircle.append(mainZoneCircle.at(i));
    }

    for (int i = 0; i < zoneCircleDefense.count(); ++i)
    {
        _zoneCircleDefense.append(zoneCircleDefense.at(i));
    }

    for (int i = 0; i < zoneCircleAttack.count(); ++i)
    {
        _zoneCircleAttack.append(zoneCircleAttack.at(i));
    }
}

