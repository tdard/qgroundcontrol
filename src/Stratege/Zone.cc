#include "Zone.h"


Zone::Zone(QGeoCoordinate origin, QGeoCoordinate orientationPoint, float dNorth, float dEast, float alt, float width, float length, float height)
{
    _origin = origin;
    _orientationPoint = orientationPoint;
    _dNorth = dNorth;
    _dEast = dEast;
    _alt = alt;
    _width = width;
    _length = length;
    _height = height;


    QVector2D orientationPointVector = _getRelativePosition(origin, orientationPoint);
    double angle = atan2(orientationPointVector.y(), orientationPointVector.x());


    //Construct all QGeoCoordinates using getLocationMetres & Coordinate transformations
    //TOP LEFT
    //Initial relative value
    QVector2D topLeftRelative = QVector2D(dEast - width/2, dNorth + length / 2);
    //Value after rotation
    topLeftRelative.setX(topLeftRelative.x()*cos(angle) + topLeftRelative.y()*sin(angle));
    topLeftRelative.setY(topLeftRelative.y()*cos(angle) - topLeftRelative.x()*sin(angle));
    _topLeft = _getLocationMetres(origin, topLeftRelative.x(), topLeftRelative.y(), alt);


    QVector2D topRightRelative = QVector2D(dEast + width/2, dNorth + length / 2);
    //Value after rotation
    topRightRelative.setX(topRightRelative.x()*cos(angle) + topRightRelative.y()*sin(angle));
    topRightRelative.setY(topRightRelative.y()*cos(angle) - topRightRelative.x()*sin(angle));
    _topRight = _getLocationMetres(origin, topRightRelative.x(), topRightRelative.y(), alt);

    QVector2D bottomLeftRelative = QVector2D(dEast - width/2, dNorth - length / 2);
    //Value after rotation
    bottomLeftRelative.setX(bottomLeftRelative.x()*cos(angle) + bottomLeftRelative.y()*sin(angle));
    bottomLeftRelative.setY(bottomLeftRelative.y()*cos(angle) - bottomLeftRelative.x()*sin(angle));
    _bottomLeft = _getLocationMetres(origin, bottomLeftRelative.x(), bottomLeftRelative.y(), alt);



    QVector2D bottomRightRelative = QVector2D(dEast + width/2, dNorth - length / 2);
    //Value after rotation
    bottomRightRelative.setX(bottomRightRelative.x()*cos(angle) + bottomRightRelative.y()*sin(angle));
    bottomRightRelative.setY(bottomRightRelative.y()*cos(angle) - bottomRightRelative.x()*sin(angle));
    _bottomRight = _getLocationMetres(origin, bottomRightRelative.x(), bottomRightRelative.y(), alt);

    QVector2D centerRelative = QVector2D(dEast, dNorth);
    //Value after rotation
    centerRelative.setX(bottomLeftRelative.x()*cos(angle) + bottomLeftRelative.y()*sin(angle));
    centerRelative.setY(bottomLeftRelative.y()*cos(angle) - bottomLeftRelative.x()*sin(angle));
    _centre = _getLocationMetres(origin, bottomLeftRelative.x(), bottomLeftRelative.y(), alt);


}



QGeoCoordinate Zone::_getLocationMetres(QGeoCoordinate origin, float dNorth, float dEast, float alt)
{
    QGeoCoordinate locationMetres = QGeoCoordinate();
    //Coordinate offsets in radians
    double dLat = dNorth / EARTH_RADIUS;
    double dLon = dEast / (EARTH_RADIUS * cos(M_PI * origin.latitude()/180 ));
    //New position in decimal degrees
    double newlat = origin.latitude() + (dLat * 180/M_PI);
    double newlon = origin.longitude() + (dLon * 180/M_PI);
    locationMetres.setLatitude(newlat);
    locationMetres.setLongitude(newlon);
    locationMetres.setLongitude(alt);
    return locationMetres;
}


QVector2D Zone::_getRelativePosition(QGeoCoordinate origin, QGeoCoordinate locationMetres)
{
    QVector2D relativeposition = QVector2D();
    float dLat = (locationMetres.latitude()-origin.latitude())*M_PI/180;
    float dLon = (locationMetres.longitude()-origin.longitude())*M_PI/180;
    float dNorth = dLat * EARTH_RADIUS;
    float dEast = dLon * (EARTH_RADIUS * cos(M_PI * origin.latitude()/180 ));
    relativeposition.setX(dNorth);
    relativeposition.setY(dEast);
    return relativeposition;
}


bool Zone::inside(QGeoCoordinate location)
{
//    /!\ Double to float conversion could get some imprecisions in this method. Investigate about precision of floats

    QVector2D rightVector1 = QVector2D(_topRight.longitude() - location.longitude(), _topRight.latitude() - location.latitude());
    QVector2D rightVector2 = QVector2D(_bottomRight.longitude() - location.longitude(), _bottomRight.latitude() - location.latitude());
    bool condition1 = QVector2D::dotProduct(rightVector1, rightVector2) < 0;

    QVector2D topVector1 = QVector2D(_topRight.longitude() - location.longitude(), _topRight.latitude() - location.latitude());
    QVector2D topVector2 = QVector2D(_bottomLeft.longitude() - location.longitude(), _topLeft.latitude() - location.latitude());
    bool condition2 = QVector2D::dotProduct(topVector1, topVector2) < 0;

    return condition1 && condition2;
}


ZoneManager::ZoneManager(QObject* parent)
{
    _ready = false;
    _zones = QList<Zone>();
}

void ZoneManager::createZones(QGeoCoordinate origin, QGeoCoordinate orientationPoint)
{
    //Defense zones
    for (auto alt : {ALT_DEF_1, ALT_DEF_2, ALT_DEF_3})
    {
        _zones.append(Zone(origin, orientationPoint, ZONE_LEN/2, ZONE_WIDTH/2, alt, ZONE_WIDTH, ZONE_LEN, ALT_DEF_1));
        _zones.append(Zone(origin, orientationPoint, 0.75 * ZONE_LEN, ZONE_WIDTH/2, alt, ZONE_WIDTH, ZONE_LEN, ALT_DEF_1));
    }

    //Attack zones
    for (auto alt : {ALT_ATTACK_1, ALT_ATTACK_2, ALT_ATTACK_3, ALT_ATTACK_4})
    {
        _zones.append(Zone(origin, orientationPoint, ZONE_LEN/2, ZONE_WIDTH/2, alt, ZONE_WIDTH, ZONE_LEN, ALT_ATTACK_1));
        _zones.append(Zone(origin, orientationPoint, 0.75 * ZONE_LEN, ZONE_WIDTH/2, alt, ZONE_WIDTH, ZONE_LEN, ALT_ATTACK_1));
    }

    _ready = true;
}

