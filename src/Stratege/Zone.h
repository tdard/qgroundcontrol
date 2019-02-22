#ifndef ZONE_H
#define ZONE_H

#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "QGCToolbox.h"
#include "QGCLoggingCategory.h"

#include "QMap"
#include "QGeoCoordinate"
#include "QTime"
#include "QVector3D"
#include "QVector2D"



#define ALT_DEF_1 40
#define ALT_DEF_2 80
#define ALT_DEF_3 120

#define ALT_ATTACK_1 30
#define ALT_ATTACK_2 60
#define ALT_ATTACK_3 90
#define ALT_ATTACK_4 120

#define ZONE_LEN 25
#define ZONE_WIDTH 50

#define EARTH_RADIUS 6378137



/*A zone is a rectangle shape*/


class Zone
{
public:
    Zone(QGeoCoordinate origin, QGeoCoordinate orientationPoint, float dNorth, float dEast, float alt, float width, float length, float height);

    float dNorth                        () { return _dNorth; }
    float dEast                         () { return _dEast; }
    float alt                           () { return _alt; }
    float width                         () { return _width; }
    float length                        () { return _length; }
    float height                        () { return _height; }
    QGeoCoordinate topLeft              () { return _topLeft; }
    QGeoCoordinate topRight             () { return _topRight; }
    QGeoCoordinate bottomLeft           () { return _bottomLeft; }
    QGeoCoordinate bottomRight          () { return _bottomRight; }
    QGeoCoordinate centre               () { return _centre; }
    bool inside                         (QGeoCoordinate location);

private:
    QGeoCoordinate _getLocationMetres   (QGeoCoordinate origin, float dNorth, float dEast, float alt);
    QVector2D _getRelativePosition(QGeoCoordinate origin, QGeoCoordinate point);
    float _dNorth;
    float _dEast;
    float _alt;
    float _width;
    float _length;
    float _height;
    QGeoCoordinate _topLeft;
    QGeoCoordinate _topRight;
    QGeoCoordinate _bottomLeft;
    QGeoCoordinate _bottomRight;
    QGeoCoordinate _centre;
    QGeoCoordinate _origin;
    QGeoCoordinate _orientationPoint;
};


class ZoneManager: public QObject
{
    Q_OBJECT

public:
    ZoneManager(QObject* parent = nullptr);

    bool ready          () { return _ready; }
    QList<Zone> zones   ();

private slots:
    void createZones(QGeoCoordinate origin, QGeoCoordinate orientationPoint);

private:
    QList<Zone> _zones;
    bool _ready;
};




#endif // ZONE_H
