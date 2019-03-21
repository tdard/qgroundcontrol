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

    connect(this, &ZoneController::requestZoneRound1FromStratege, stratege, &Stratege::handleZoneControllerRequestRound1);
    connect(this, &ZoneController::sendRound1ZoneToStratege, stratege, &Stratege::setRound1ZoneFromController);
    connect(stratege, &Stratege::sendRound1ToZoneController, this, &ZoneController::setRound1ZoneFromStratege);

    connect(this, &ZoneController::requestZoneRound2FromStratege, stratege, &Stratege::handleZoneControllerRequestRound2);
    connect(this, &ZoneController::sendRound2ZoneToStratege, stratege, &Stratege::setRound2ZoneFromController);
    connect(stratege, &Stratege::sendRound2ToZoneController, this, &ZoneController::setRound2ZoneFromStratege);

    connect(this, &ZoneController::requestZoneRound3FromStratege, stratege, &Stratege::handleZoneControllerRequestRound3);
    connect(this, &ZoneController::sendRound3ZoneToStratege, stratege, &Stratege::setRound3ZoneFromController);
    connect(stratege, &Stratege::sendRound3ToZoneController, this, &ZoneController::setRound3ZoneFromStratege);

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

void ZoneController::addZoneDefenseRound1(int numberInAltitude, int numberInHeight)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    qDebug() << "addzoneDefenseRound1" ;

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

            QGCMapPolygon* zoneDefenseRound1 = new QGCMapPolygon(this);
            zoneDefenseRound1->appendVertex(topLeft);
            zoneDefenseRound1->appendVertex(topRight);
            zoneDefenseRound1->appendVertex(bottomRight);
            zoneDefenseRound1->appendVertex(bottomLeft);

            _zoneDefenseRound1.append(zoneDefenseRound1);
        }
    }
}


void ZoneController::addZoneAttackRound1(int numberInAltitude, int numberInHeight)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    qDebug() << "addzoneAttackRound1" ;
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

            QGCMapPolygon* zoneAttackRound1 = new QGCMapPolygon(this);
            zoneAttackRound1->appendVertex(topLeft);
            zoneAttackRound1->appendVertex(topRight);
            zoneAttackRound1->appendVertex(bottomRight);
            zoneAttackRound1->appendVertex(bottomLeft);

            _zoneAttackRound1.append(zoneAttackRound1);
        }
    }
}

void ZoneController::addZoneDefenseRound2(int numberInAltitude, int radius)
{    
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    QGCMapPolygon* mainPolygon =        _zonePolygon.value<QGCMapPolygon*>(0);
    QGeoCoordinate mainCenter =         mainPolygon->center();
    QGeoCoordinate defenseCenter =      mainCenter.atDistanceAndAzimuth(POLYGON_INFO_DEF_WIDTH + LITTLE_CIRCLE_RADIUS, -90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        defenseCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneDefenseRound2 = new QGCMapCircle(defenseCenter, radius);
        _zoneDefenseRound2.append(zoneDefenseRound2);
    }
}

void ZoneController::addZoneAttackRound2(int numberInAltitude, int radius)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    QGCMapPolygon* mainPolygon =        _zonePolygon.value<QGCMapPolygon*>(0);
    QGeoCoordinate mainCenter =         mainPolygon->center();
    QGeoCoordinate attackCenter =       mainCenter.atDistanceAndAzimuth(POLYGON_INFO_DEF_WIDTH + LITTLE_CIRCLE_RADIUS, 90 - _rotation);

    for (int i = 0; i < numberInAltitude; ++i)
    {
        attackCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneAttackRound2 = new QGCMapCircle(attackCenter, radius);
        _zoneAttackRound2.append(zoneAttackRound2);
    }
}

void ZoneController::addZoneDefenseRound3(int numberInAltitude, double radius)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    QGCMapPolygon* mainPolygon =        _zonePolygon.value<QGCMapPolygon*>(0);
    QGeoCoordinate center =             mainPolygon->center();

    for (int i = 0; i < numberInAltitude; ++i)
    {
        QGeoCoordinate topCenter    = center.atDistanceAndAzimuth(POLYGON_INFO_DEF_WIDTH + ROUND_3_CIRCLE_RADIUS, -90 - _rotation).atDistanceAndAzimuth(15, -_rotation);
        topCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneDefenseRound3 = new QGCMapCircle(topCenter, radius);
        _zoneDefenseRound3.append(zoneDefenseRound3);

        QGeoCoordinate bottomCenter = center.atDistanceAndAzimuth(POLYGON_INFO_DEF_WIDTH + ROUND_3_CIRCLE_RADIUS, -90 - _rotation).atDistanceAndAzimuth(15, -180 - _rotation);
        bottomCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        zoneDefenseRound3 = new QGCMapCircle(bottomCenter, radius);
        _zoneDefenseRound3.append(zoneDefenseRound3);
    }
}

void ZoneController::addZoneAttackRound3(int numberInAltitude, double radius)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

    QGCMapPolygon* mainPolygon =        _zonePolygon.value<QGCMapPolygon*>(0);
    QGeoCoordinate center =         mainPolygon->center();

    for (int i = 0; i < numberInAltitude; ++i)
    {
        QGeoCoordinate topCenter    = center.atDistanceAndAzimuth(POLYGON_INFO_DEF_WIDTH + ROUND_3_CIRCLE_RADIUS, 90 - _rotation).atDistanceAndAzimuth(15, -_rotation);
        topCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        QGCMapCircle* zoneAttackRound3 = new QGCMapCircle(topCenter, radius);
        _zoneAttackRound3.append(zoneAttackRound3);

        QGeoCoordinate bottomCenter = center.atDistanceAndAzimuth( POLYGON_INFO_DEF_WIDTH + ROUND_3_CIRCLE_RADIUS, 90 - _rotation).atDistanceAndAzimuth(15, -180 - _rotation);
        bottomCenter.setAltitude((i + 0.5)*(MAX_ALTITUDE / numberInAltitude));
        zoneAttackRound3 = new QGCMapCircle(bottomCenter, radius);
        _zoneAttackRound3.append(zoneAttackRound3);
    }
}


void ZoneController::rotateZones(int index, int rotation, int height, int width)
{
    //Safety
    if (_zonePolygon.count() == 0) { return; }

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


void ZoneController::deleteAll(void)
{
    qDebug() << "deleteAll";
    _zonePolygon.clear();

    _zoneAttackRound2.clear();
    _zoneDefenseRound2.clear();
    sendRound2Zone();

    _zoneAttackRound3.clear();
    _zoneDefenseRound3.clear();
    sendRound3Zone();

    _zoneAttackRound1.clear();
    _zoneDefenseRound1.clear();
    sendRound1Zone();
}

void ZoneController::sendRound1Zone(void)
{
    qDebug() << "sendRound1ZoneToStratege";

    QList<QGCMapPolygon*> zonePolygon = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygon.count(); ++i)
    {
        zonePolygon.append(_zonePolygon.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapPolygon*> zoneDefenseRound1 = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zoneDefenseRound1.count(); ++i)
    {
        zoneDefenseRound1.append(_zoneDefenseRound1.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapPolygon*> zoneAttackRound1 = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zoneAttackRound1.count(); ++i)
    {
        zoneAttackRound1.append(_zoneAttackRound1.value<QGCMapPolygon*>(i));
    }

    emit sendRound1ZoneToStratege(zonePolygon, zoneDefenseRound1, zoneAttackRound1);
}

void ZoneController::sendRound2Zone(void)
{
    qDebug() << "sendRound2ZoneToStratege";

    QList<QGCMapPolygon*> zonePolygon = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygon.count(); ++i)
    {
        zonePolygon.append(_zonePolygon.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapCircle*> zoneDefenseRound2 = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneDefenseRound2.count(); ++i)
    {
        zoneDefenseRound2.append(_zoneDefenseRound2.value<QGCMapCircle*>(i));
    }

    QList<QGCMapCircle*> zoneAttackRound2 = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneAttackRound2.count(); ++i)
    {
        zoneAttackRound2.append(_zoneAttackRound2.value<QGCMapCircle*>(i));
    }

    emit sendRound2ZoneToStratege(zonePolygon, zoneDefenseRound2, zoneAttackRound2);
}

void ZoneController::sendRound3Zone(void)
{
    qDebug() << "sendRound3ZoneToStratege";

    QList<QGCMapPolygon*> zonePolygon = QList<QGCMapPolygon*>();
    for (int i = 0; i < _zonePolygon.count(); ++i)
    {
        zonePolygon.append(_zonePolygon.value<QGCMapPolygon*>(i));
    }

    QList<QGCMapCircle*> zoneDefenseRound3 = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneDefenseRound3.count(); ++i)
    {
        zoneDefenseRound3.append(_zoneDefenseRound3.value<QGCMapCircle*>(i));
    }

    QList<QGCMapCircle*> zoneAttackRound3 = QList<QGCMapCircle*>();
    for (int i = 0; i < _zoneAttackRound3.count(); ++i)
    {
        zoneAttackRound3.append(_zoneAttackRound3.value<QGCMapCircle*>(i));
    }

    emit sendRound3ZoneToStratege(zonePolygon, zoneDefenseRound3, zoneAttackRound3);
}

void ZoneController::clearAllInteractive(void)
{
    qDebug() << "clearAllInteractive";

    for (int i=0; i<_zonePolygon.count(); i++) {
        _zonePolygon.value<QGCMapPolygon*>(i)->setInteractive(false);
    }

    //Round 1
    for (int i=0; i<_zoneDefenseRound1.count(); i++) {
        _zoneDefenseRound1.value<QGCMapPolygon*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zoneAttackRound1.count(); i++) {
        _zoneAttackRound1.value<QGCMapPolygon*>(i)->setInteractive(false);
    }

    //Round 2
    for (int i=0; i<_zoneDefenseRound2.count(); i++) {
        _zoneDefenseRound2.value<QGCMapCircle*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zoneAttackRound2.count(); i++) {
        _zoneAttackRound2.value<QGCMapCircle*>(i)->setInteractive(false);
    }

    //Round 3
    for (int i=0; i<_zoneDefenseRound3.count(); i++) {
        _zoneDefenseRound3.value<QGCMapCircle*>(i)->setInteractive(false);
    }
    for (int i=0; i<_zoneAttackRound3.count(); i++) {
        _zoneAttackRound3.value<QGCMapCircle*>(i)->setInteractive(false);
    }
}

void ZoneController::setRound1ZoneFromStratege(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zoneDefenseRound1, QList<QGCMapPolygon*> zoneAttackRound1)
{
    qDebug() << "setRound1FromStratege, flyView:" << _flyView;
    if (_flyView == false) { return; }

    _zonePolygon.clear();
    _zoneAttackRound1.clear();
    _zoneDefenseRound1.clear();

    for (int i = 0; i < mainZonePolygon.count(); ++i)
    {
        _zonePolygon.append(mainZonePolygon.at(i));
    }

    for (int i = 0; i < zoneDefenseRound1.count(); ++i)
    {
        _zoneDefenseRound1.append(zoneDefenseRound1.at(i));
    }

    for (int i = 0; i < zoneAttackRound1.count(); ++i)
    {
        _zoneAttackRound1.append(zoneAttackRound1.at(i));
    }
}

void ZoneController::setRound2ZoneFromStratege(QList<QGCMapPolygon*> mainzonePolygon, QList<QGCMapCircle*> zoneDefenseRound2, QList<QGCMapCircle*> zoneAttackRound2)
{
    qDebug() << "setRound2FromStratege, flyView:" << _flyView;
    if (_flyView == false) { return; }

    _zonePolygon.clear();
    _zoneAttackRound2.clear();
    _zoneDefenseRound2.clear();

    for (int i = 0; i < mainzonePolygon.count(); ++i)
    {
        _zonePolygon.append(mainzonePolygon.at(i));
    }

    for (int i = 0; i < zoneDefenseRound2.count(); ++i)
    {
        _zoneDefenseRound2.append(zoneDefenseRound2.at(i));
    }

    for (int i = 0; i < zoneAttackRound2.count(); ++i)
    {
        _zoneAttackRound2.append(zoneAttackRound2.at(i));
    }
}

void ZoneController::setRound3ZoneFromStratege(QList<QGCMapPolygon*> mainzonePolygon, QList<QGCMapCircle*> zoneDefenseRound3, QList<QGCMapCircle*> zoneAttackRound3)
{
    qDebug() << "setRound3FromStratege, flyView:" << _flyView;
    if (_flyView == false) { return; }

    _zonePolygon.clear();
    _zoneAttackRound3.clear();
    _zoneDefenseRound3.clear();

    for (int i = 0; i < mainzonePolygon.count(); ++i)
    {
        _zonePolygon.append(mainzonePolygon.at(i));
    }

    for (int i = 0; i < zoneDefenseRound3.count(); ++i)
    {
        _zoneDefenseRound3.append(zoneDefenseRound3.at(i));
    }

    for (int i = 0; i < zoneAttackRound3.count(); ++i)
    {
        _zoneAttackRound3.append(zoneAttackRound3.at(i));
    }
}

