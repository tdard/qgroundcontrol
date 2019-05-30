//FINAL VERSION

#include "Stratege.h"
#include "AutoPilotPlugin.h"
#include "MAVLinkProtocol.h"
#include "UAS.h"
#include "QGCApplication.h"
#include "FollowMe.h"
#include "QGroundControlQmlGlobal.h"
#include "ParameterManager.h"
#include "SettingsManager.h"
#include "QGCCorePlugin.h"
#include "QGCOptions.h"

#include "Vehicle.h"

#include <QQmlEngine>

#include "PlanManager.h"
#include "MissionItem.h"

VehicleAttribut::VehicleAttribut(Vehicle* vehicle)
{
    _vehicle = vehicle;
    _role = UNDEFINED_UAV;
    _targetLonLatAltCoord = QGeoCoordinate();
    _mainServo = new mavlink_servo_output_raw_t;
    _auxServo = new mavlink_servo_output_raw_t;
    _attpatroliter = 0;
    _defpatroliter = 0;
    _inpatrol = false;
    _inattack = false;
    _infinal = false;
}



Stratege::Stratege(QGCApplication* app, QGCToolbox* toolbox): QGCTool(app, toolbox)
{
    _abortMission                           =       false;
    _startMission                           =       false;
    _time                                   =       QTime();
    _time.start();
    _mapVehicle2VehicleAttribut             =       new QMap<Vehicle*, VehicleAttribut*>();
    _mapTargetsPositions2TargetsVelocities  =       new QMap<QGeoCoordinate, QVector3D>();

    _mainZonePolygon                        =       QList<QGCMapPolygon*>();

    _zoneAttackRound1                       =       QList<QGCMapPolygon*>();
    _zoneDefenseRound1                      =       QList<QGCMapPolygon*>();

    _zoneAttackRound2                       =       QList<QGCMapCircle*>();
    _zoneDefenseRound2                      =       QList<QGCMapCircle*>();

    _zoneAttackRound3                       =       QList<QGCMapCircle*>();
    _zoneDefenseRound3                      =       QList<QGCMapCircle*>();

    _isCircularZone                         =       false;
}

void Stratege::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);
    connect(toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleAdded, this, &Stratege::_addedVehicle);
    connect(toolbox->multiVehicleManager(), &MultiVehicleManager::vehicleRemoved, this, &Stratege::_removedVehicle);
}

void Stratege::abortMission()
{
    _startMission = false;
    _abortMission = true; //eventually, instead of having a variable here implement the solution right here
    qDebug() << "Abortion of mission started";
    for(auto vm : _mapVehicle2VehicleAttribut->keys())
    {
        vm->firmwarePlugin()->guidedModeLand(vm);
        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;
        _mapVehicle2VehicleAttribut->value(vm)->_inattack = false;
    }
    qDebug() << "Abortion of mission finished";
    qDebug() << "Current time: " << QTime::currentTime().toString();
    qDebug() << "Timespan (s) : " << _time.secsTo(QTime::currentTime());
}


void Stratege::startMission()
{
    _time.restart();

    //See the patrol zones
//    if(!_zoneAttackRound1.empty())
//    {
//        qDebug() << "-ROUND 1 ZONES-";
//        qDebug() << "Zone of Attack :" << _zoneAttackRound1.size();
//        qDebug() << "Zone of Defense :" << _zoneDefenseRound1.size();
//        for(auto zone : _zoneAttackRound1)
//        {
//            qDebug() << "Polygon attack zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//        for(auto zone : _zoneDefenseRound1)
//        {
//            qDebug() << "Polygon defense zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//    }

//    if(!_zoneAttackRound2.empty())
//    {
//        qDebug() << "-ROUND 2 ZONES-";
//        qDebug() << "Zone of Attack :" << _zoneAttackRound2.size();
//        qDebug() << "Zone of Defense :" << _zoneDefenseRound2.size();
//        for(auto zone : _zoneAttackRound2)
//        {
//            qDebug() << "Circular attack zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//        for(auto zone : _zoneDefenseRound2)
//        {
//            qDebug() << "Circular  defense zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//    }

//    if(!_zoneAttackRound3.empty())
//    {
//        qDebug() << "-ROUND 3 ZONES-";
//        qDebug() << "Zone of Attack :" << _zoneAttackRound3.size();
//        qDebug() << "Zone of Defense :" << _zoneDefenseRound3.size();
//        for(auto zone : _zoneAttackRound3)
//        {
//            qDebug() << "Circular attack zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//        for(auto zone : _zoneDefenseRound3)
//        {
//            qDebug() << "Circular  defense zone : " << zone;
//            qDebug() << "longitude : " << zone->center().longitude();
//            qDebug() << "latitude : " << zone->center().latitude();
//            qDebug() << "altitude : " << zone->center().altitude() << "\n";
//        }
//    }

    int attiter = 0;
    int defiter = 0;

    for(auto vm : _mapVehicle2VehicleAttribut->keys())
    {

        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;
        _mapVehicle2VehicleAttribut->value(vm)->_inattack = false;

        //        //Filling of the attack zones
//        qDebug() << "VEHICLE :" << vm->id();
//        //_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter = attiter;
//        _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter = vm->id()-1;
//        qDebug() << "Vehicle :" << vm->id() << " | attack zone : " << attiter+1;
//        attiter ++;

//        //Filling of the defense zones
//        if (vm->id() <= 6) //vm->vehicleType() == MAV_TYPE_HEXAROTOR
//        {
//            //_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter = defiter;
//            _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter = vm->id()-1;
//            qDebug() << "Vehicle :" << vm->id() << " | defense zone : " << defiter+1;
//            defiter ++;
//        }

        qDebug() << "VEHICLE :" << vm->id();
        if (vm->id() > 6)
        {
           _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter = vm->id()-7;
           qDebug() << "Vehicle :" << vm->id() << " | attack zone : " << vm->id()-6;
        }

        if (vm->id() <= 6)
        {
            _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter = vm->id()+5;
            qDebug() << "Vehicle :" << vm->id() << " | attack zone : " << vm->id()+6;
            _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter = vm->id()-1;
            qDebug() << "Vehicle :" << vm->id() << " | defense zone : " << vm->id();
        }

        //Take off command
        vm->setArmed(true);

        double alt = 2;
//        if(!_zoneAttackRound1.empty() && vm->id() <= 6) { alt = _zoneDefenseRound1[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center().altitude(); }
//        if(!_zoneAttackRound1.empty() && vm->id() > 6) { alt = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
//        if(!_zoneAttackRound2.empty() && vm->id() <= 6) { alt = _zoneDefenseRound2[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center().altitude(); }
//        if(!_zoneAttackRound2.empty() && vm->id() > 6) { alt = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
//        if(!_zoneAttackRound3.empty() && vm->id() <= 6) { alt = _zoneDefenseRound3[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center().altitude(); }
//        if(!_zoneAttackRound3.empty() && vm->id() > 6) { alt = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound1.empty() && vm->id() <= 6) { alt = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound1.empty() && vm->id() > 6) { alt = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound2.empty() && vm->id() <= 6) { alt = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound2.empty() && vm->id() > 6) { alt = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound3.empty() && vm->id() <= 6) { alt = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
        if(!_zoneAttackRound3.empty() && vm->id() > 6) { alt = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }

        vm->firmwarePlugin()->guidedModeTakeoff(vm, alt);
        qDebug() << "Vehicle :" << vm->id() << " takes off to " << alt << "m | time: " << _time.secsTo(QTime::currentTime());
        qDebug() << "Mission started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime()) << "\n";

    }
    _startMission = true;
    qDebug() << "\n Stratege: -MISSION STARTED-";
}

void Stratege::_attack(Vehicle* vm)
{
    //ATTACK COMMAND
    _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;
    int ID = _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter;

    if(_time.msecsTo(QTime::currentTime())%500 == 0 && _time.secsTo(QTime::currentTime()) < PLACEMENT_TIME){_mapVehicle2VehicleAttribut->value(vm)->_inattack = false;}

    if(!_mapVehicle2VehicleAttribut->value(vm)->_inattack && _time.secsTo(QTime::currentTime()) > 3*ID)
    {
        if (!_zoneAttackRound1.empty())
        {
            QGeoCoordinate center = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(3+ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        if (!_zoneAttackRound2.empty())
        {
            QGeoCoordinate center = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(3+1.5*ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        if (!_zoneAttackRound3.empty())
        {
            QGeoCoordinate center = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(3+0.5*ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        _mapVehicle2VehicleAttribut->value(vm)->_inattack = true;
        qDebug() << "Attack started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
    }

    //PLACEMENT BEFORE LANDING
    if (!_mapVehicle2VehicleAttribut->value(vm)->_infinal && _time.secsTo(QTime::currentTime()) > PLACEMENT_TIME)
    {
        if (!_zoneAttackRound1.empty())
        {
            int angle = 360/12 * _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter;
            QGeoCoordinate waypoint = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(10, angle);
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
        }
        if (!_zoneAttackRound2.empty())
        {
            int angle = 360/12 * _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter;
            QGeoCoordinate waypoint = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(4, angle);
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
        }
        if (!_zoneAttackRound3.empty())
        {
            int angle = 360/12 * _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter;
            QGeoCoordinate waypoint = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(2, angle);
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
        }
        _mapVehicle2VehicleAttribut->value(vm)->_infinal = true;
        qDebug() << "Vehicle :" << vm->id() << " placed for landing | time: " << _time.secsTo(QTime::currentTime());
    }
}


void Stratege::_patrol(Vehicle* vm)
{
    //PATROL COMMAND

    int ID = _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter;

    if(_time.msecsTo(QTime::currentTime())%500 == 0){_mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;}

    if(!_mapVehicle2VehicleAttribut->value(vm)->_inpatrol && _time.secsTo(QTime::currentTime()) > 3*ID)
    {
        if (!_zoneDefenseRound1.empty())
        {
            QGeoCoordinate center = _zoneDefenseRound1[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(5+ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneDefenseRound1[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        if (!_zoneDefenseRound2.empty())
        {
            QGeoCoordinate center = _zoneDefenseRound2[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(3+ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneDefenseRound2[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        if (!_zoneDefenseRound3.empty())
        {
            QGeoCoordinate center = _zoneDefenseRound3[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
            QGeoCoordinate waypoint = center.atDistanceAndAzimuth(3+0.5*ID, 360/10 * _time.secsTo(QTime::currentTime()));
            vm->firmwarePlugin()->guidedModeGotoLocation(vm, waypoint);
//            QGeoCoordinate waypoint = _zoneDefenseRound3[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
//            double alt = vm->homePosition().altitude() + waypoint.altitude();
//            vm->guidedModeOrbit(waypoint, 3 + ID, alt);
        }
        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = true;
        qDebug() << "Patrol started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
    }

    //TETRIS STATE
    if(_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter > 1 && _time.secsTo(QTime::currentTime()) > TETRIS_TIME && !_zoneDefenseRound1.empty())
    {
        bool replace = true;
        for (auto othervehicle : _mapVehicle2VehicleAttribut->keys())
        {
            int underdefpatroliter = _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter - 2;
            if(_mapVehicle2VehicleAttribut->value(othervehicle)->_defpatroliter == underdefpatroliter && _zoneDefenseRound1[underdefpatroliter]->containsCoordinate(othervehicle->coordinate()))
            {
                replace = false;
            }
        }
        if(replace)
        {
            _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter -= 2;
            qDebug() << "Vehicle: " << vm->id() << " descent to zone " << _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter+1;
            _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;
        }
    }

//    //Back to parol if outside of the zone
//    if(_mapVehicle2VehicleAttribut->value(vm)->_inpatrol && !_zoneDefenseRound1[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->containsCoordinate(vm->coordinate()))
//    {
//        QGeoCoordinate waypoint = _zoneDefenseRound1[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
//        double alt = vm->homePosition().altitude() + waypoint.altitude();
//        vm->guidedModeOrbit(waypoint, 10, alt);
//        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = true;
//        qDebug() << "Vehicle: " << vm->id() << " out of patrol zone : return to patrol mode | time: " << _time.secsTo(QTime::currentTime());
//    }

}

void Stratege::updateData(mavlink_message_t message)
{
    _parse(message);

    //Main update loop
    if (_startMission == true)
    {
        for(auto vm : _mapVehicle2VehicleAttribut->keys())
        {
            //DEFENSE DRONES
            if (vm->id() <= 6) //vm->vehicleType() == MAV_TYPE_HEXAROTOR
            {
                double limalt = 1;
                if(!_zoneAttackRound1.empty()) { limalt = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if(!_zoneAttackRound2.empty()) { limalt = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if(!_zoneAttackRound3.empty()) { limalt = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if (vm->altitudeRelative()->rawValue() >= limalt-1 && _time.secsTo(QTime::currentTime()) < DEFENSE_TIME) { _patrol(vm); }
                int ID = vm->id();
                if (_time.secsTo(QTime::currentTime()) >= DEFENSE_TIME + 3*ID) { _attack(vm); }
            }

            //ATTACK DRONES

            if (vm->id() > 6) //vm->vehicleType() == MAV_TYPE_QUADROTOR
            {
                double limalt = 1;
                if(!_zoneAttackRound1.empty()) { limalt = _zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if(!_zoneAttackRound2.empty()) { limalt = _zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if(!_zoneAttackRound3.empty()) { limalt = _zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().altitude(); }
                if (vm->altitudeRelative()->rawValue() >= limalt-1) { _attack(vm); }
            }

            //DEFINE THE HOME IN CASE OF COM_LOSS #thank you Utkarsh
            if (_time.secsTo(QTime::currentTime()) < HOME_TIME)
            {
                int angle = 360/12 * _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter;
                if(!_zoneAttackRound1.empty()) { vm->homePositionChanged(_zoneAttackRound1[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(10, angle)); }
                if(!_zoneAttackRound2.empty()) { vm->homePositionChanged(_zoneAttackRound2[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(4, angle)); }
                if(!_zoneAttackRound3.empty()) { vm->homePositionChanged(_zoneAttackRound3[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center().atDistanceAndAzimuth(2, angle)); }
            }
        }

        if (_time.secsTo(QTime::currentTime()) >= COMPETITION_TIME)
        {
            _startMission = false;
            qDebug() << "\n -END OF ROUND TIMER-";
            for(auto vm : _mapVehicle2VehicleAttribut->keys())
            {
                vm->firmwarePlugin()->guidedModeLand(vm);
                qDebug() << "Land started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
            }
        }
    }
}

void Stratege::_addedVehicle(Vehicle* vehicle)
{
    qDebug()  << "Stratege: Vehicle Added: No " << vehicle->id() << " | Type: " << vehicle->vehicleType();
    _mapVehicle2VehicleAttribut->insert(vehicle, new VehicleAttribut(vehicle));
    qDebug() << "\n -VEHICLE LIST OF " << _mapVehicle2VehicleAttribut->size() << " UAV-";
    for(auto vm : _mapVehicle2VehicleAttribut->keys())
    {
        qDebug() << "Vehicle :" << vm->id()<< " | Type: " << vehicle->vehicleType();
    }
}

void Stratege::_removedVehicle(Vehicle* vehicle)
{
    //TODO
//    Modify List of unoccupied zones
    qDebug()  << "Stratege: Vehicle Removed: No " << vehicle->id();
    qDebug() << _mapVehicle2VehicleAttribut->remove(vehicle);
}

void Stratege::setRound1ZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zoneDefenseRound1, QList<QGCMapPolygon*> zoneAttackRound1)
{
    qDebug() << "setRound1ZoneFromController";
    _mainZonePolygon = mainZonePolygon;
    _zoneDefenseRound1 = zoneDefenseRound1;
    _zoneAttackRound1 = zoneAttackRound1;
    _isCircularZone = false;

    emit sendRound1ToZoneController(mainZonePolygon, zoneDefenseRound1, zoneAttackRound1);
}

void Stratege::setRound2ZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound2, QList<QGCMapCircle*> zoneAttackRound2)
{
    qDebug() << "setRound2ZoneFromController";
     _mainZonePolygon = mainZonePolygon;
    _zoneDefenseRound2 = zoneDefenseRound2;
    _zoneAttackRound2 = zoneAttackRound2;
    _isCircularZone = true;

    emit sendRound2ToZoneController(mainZonePolygon, zoneDefenseRound2, zoneAttackRound2);
}

void Stratege::setRound3ZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapCircle*> zoneDefenseRound3, QList<QGCMapCircle*> zoneAttackRound3)
{
    qDebug() << "setRound3ZoneFromController";
     _mainZonePolygon = mainZonePolygon;
    _zoneDefenseRound3 = zoneDefenseRound3;
    _zoneAttackRound3 = zoneAttackRound3;
    _isCircularZone = true;

    emit sendRound3ToZoneController(mainZonePolygon, zoneDefenseRound3, zoneAttackRound3);
}

void Stratege::handleZoneControllerRequestRound1()
{
    qDebug() << "handleZoneControllerRequestRound1";
    emit sendRound1ToZoneController(_mainZonePolygon, _zoneDefenseRound1, _zoneAttackRound1);
    _isCircularZone = false;
}

void Stratege::handleZoneControllerRequestRound2()
{
    qDebug() << "handleZoneControllerRequestRound2";
    emit sendRound2ToZoneController(_mainZonePolygon, _zoneDefenseRound2, _zoneAttackRound2);
    _isCircularZone = true;
}

void Stratege::handleZoneControllerRequestRound3()
{
    qDebug() << "handleZoneControllerRequestRound3";
    emit sendRound3ToZoneController(_mainZonePolygon, _zoneDefenseRound3, _zoneAttackRound3);
    _isCircularZone = true;
}

//void Stratege::_taskControl()
//{
//    //TODO
//}

void Stratege::_parse(mavlink_message_t message)    //To modify, does not work as expected. Or, check in Vehicle.cc
{
    //TODO
    //Parse the different incoming mavlink messages: servo_output_raw & follow_target
    switch (message.msgid)
    {
        case MAVLINK_MSG_ID_FOLLOW_TARGET:
            mavlink_follow_target_t follow_target;
            mavlink_msg_follow_target_decode(&message, &follow_target);
            for(auto vm: _mapVehicle2VehicleAttribut->keys())
            {
                if(vm->id() == message.sysid)
                {
//                    qDebug() << "FOLLOW_TARGET message received";
                    //TODO: Update Position & Velocity of the target using _mapVehicle2VehicleAttribut->value(vm)
                }
            }
            break;
        case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
            mavlink_servo_output_raw_t servo_output_raw;
            mavlink_msg_servo_output_raw_decode(&message, &servo_output_raw);
            for(auto vm: _mapVehicle2VehicleAttribut->keys())
            {
                if(vm->id() == message.sysid)
                {
                    qDebug() << "SERVO_OUTPUT_RAW message received";
                    //TODO: Update Servo Outputs of AUX or MAIN using _mapVehicle2VehicleAttribut->value(vm)
//                    if (servo_output_raw.port == 0){ _mapVehicle2VehicleAttribut->value(vm)->setMainServo(servo_output_raw) = servo_output_raw; }
//                    else {_mapVehicle2VehicleAttribut->value(vm)->setAuxServo(servo_output_raw) = ;}

                }
            }
                break;
//        default:
//            qDebug() << "Stratege: Not desired Message ID: " << message.sysid;
    }
}
