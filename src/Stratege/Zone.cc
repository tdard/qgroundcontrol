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


    //Construct all QGeoCoordinates using getLocationMetres & Coordinate transformations
    _topLeft = QGeoCoordinate();
    _topRight = QGeoCoordinate();
    _bottomLeft = QGeoCoordinate();
    _bottomRight = QGeoCoordinate();
    _centre = QGeoCoordinate();
}


QGeoCoordinate Zone::_getLocationMetres(QGeoCoordinate origin, float dNorth, float dEast, float alt)
{
    double dLat = dNorth / EARTH_RADIUS;
    //double dLon = dEast / (EARTH_RADIUS * cos())

    //TODO
}


bool Zone::inside(QGeoCoordinate location)
{
    //TODO
}


ZoneManager::ZoneManager(QObject* parent)
{
    _ready = false;
    _zones = QList<QGeoCoordinate>();
}

void ZoneManager::createZones(QGeoCoordinate origin, QGeoCoordinate orientationPoint)
{

}

