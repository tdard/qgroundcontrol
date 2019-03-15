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
}



Stratege::Stratege(QGCApplication* app, QGCToolbox* toolbox): QGCTool(app, toolbox)
{
    _abortMission = false;
    _startMission = false;
    _time = QTime();
    _time.start();
    _mapVehicle2VehicleAttribut = new QMap<Vehicle*, VehicleAttribut*>();
    _mapTargetsPositions2TargetsVelocities = new QMap<QGeoCoordinate, QVector3D>();

    _mainZonePolygon = QList<QGCMapPolygon*>();
    _zonePolygonAttack = QList<QGCMapPolygon*>();
    _zonePolygonDefense = QList<QGCMapPolygon*>();
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
    qDebug() << "Mission Started: time: " << _time.toString();
    int attiter = 0;
    int defiter = 0;
    for(auto vm : _mapVehicle2VehicleAttribut->keys())
    {

        //Filling of the attack zones
        _mapVehicle2VehicleAttribut->value(vm)->_attpatroliter = attiter;
        qDebug() << "Vehicle :" << vm->id() << " | attack zone : " << attiter+1;
        attiter ++;

        //Filling of the defense zones
        if(vm->vehicleType() == MAV_TYPE_HEXAROTOR)
        {
            _mapVehicle2VehicleAttribut->value(vm)->_defpatroliter = defiter;
            qDebug() << "Vehicle :" << vm->id() << " | defense zone : " << defiter+1;
            defiter ++;
        }

        //Take off command
        vm->firmwarePlugin()->guidedModeTakeoff(vm, 15);
        qDebug() << "Mission started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
    }
    _startMission = true;
}

void Stratege::_attack(Vehicle* vm)
{
    //Attack command
    _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = false;
    if(!_mapVehicle2VehicleAttribut->value(vm)->_inattack)
    {
        QGeoCoordinate waypoint = _zonePolygonAttack[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
        double alt = vm->homePosition().altitude() + waypoint.altitude();
        vm->guidedModeOrbit(waypoint, 10, alt);
        _mapVehicle2VehicleAttribut->value(vm)->_inattack = true;
        qDebug() << "Attack started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
    }

//    //Back to attack if outside of the zone
//    if(_mapVehicle2VehicleAttribut->value(vm)->_inattack && !_zonePolygonAttack[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->containsCoordinate(vm->coordinate()))
//    {
//        QGeoCoordinate waypoint = _zonePolygonAttack[_mapVehicle2VehicleAttribut->value(vm)->_attpatroliter]->center();
//        double alt = vm->homePosition().altitude() + waypoint.altitude();
//        vm->guidedModeOrbit(waypoint, 10, alt);
//        _mapVehicle2VehicleAttribut->value(vm)->_inattack = true;
//        qDebug() << "Vehicle: " << vm->id() << " out of attack zone : return to attack mode | time: " << _time.secsTo(QTime::currentTime());
//    }
}

void Stratege::_patrol(Vehicle* vm)
{
    //Patrol command
    _mapVehicle2VehicleAttribut->value(vm)->_inattack = false;
    if(!_mapVehicle2VehicleAttribut->value(vm)->_inpatrol)
    {
        //verify if inferiour layers are full -> change the altitude
        QGeoCoordinate waypoint = _zonePolygonDefense[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
        double alt = vm->homePosition().altitude() + waypoint.altitude();
        vm->guidedModeOrbit(waypoint, 10, alt);
        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = true;
        qDebug() << "Patrol started for vehicle :" << vm->id() << " | time: " << _time.secsTo(QTime::currentTime());
    }

//    //Back to attack if outside of the zone
//    if(_mapVehicle2VehicleAttribut->value(vm)->_inpatrol && !_zonePolygonDefense[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->containsCoordinate(vm->coordinate()))
//    {
//        QGeoCoordinate waypoint = _zonePolygonDefense[_mapVehicle2VehicleAttribut->value(vm)->_defpatroliter]->center();
//        double alt = vm->homePosition().altitude() + waypoint.altitude();
//        vm->guidedModeOrbit(waypoint, 10, alt);
//        _mapVehicle2VehicleAttribut->value(vm)->_inpatrol = true;
//        qDebug() << "Vehicle: " << vm->id() << " out of patrol zone : return to patrol mode | time: " << _time.secsTo(QTime::currentTime());
//    }

    //head towards the enemy drone that is at the same altitude
    /*double rel_alt = vm->coordinate().altitude() - vm->homePosition().altitude();
    double nmy_alt = _mapVehicle2VehicleAttribut->value(vm)->targetLonLatAltCoord().altitude();
    if (nmy_alt < rel_alt+20 || nmy_alt > rel_alt-20)
    {    }*/

}

void Stratege::updateData(mavlink_message_t message)
{
    _parse(message);

    if (_startMission == true)
    {
        //Stop State ~ Start

        for(auto vm : _mapVehicle2VehicleAttribut->keys())
        {
            switch (vm->vehicleType()){
            case MAV_TYPE_HEXAROTOR:{
                if (vm->altitudeRelative()->rawValue() > 5 && _time.secsTo(QTime::currentTime()) < 30) { _patrol(vm); }
                if (_time.secsTo(QTime::currentTime()) >= 30) { _attack(vm); }
                break;
            }
            case MAV_TYPE_QUADROTOR:{
                if (_time.secsTo(QTime::currentTime()) >= 15) { _attack(vm); }
                break;
            }}
        }

        if (_time.secsTo(QTime::currentTime()) >= 90)
        {
            _startMission = false;
            for(auto vm : _mapVehicle2VehicleAttribut->keys())
            {
                if (vm->vehicleType() != MAV_TYPE_GROUND_ROVER)
                {
                    vm->firmwarePlugin()->guidedModeLand(vm);
                    qDebug() << "End of the timer | Land started for vehicle :" << vm << " | time: " << _time.secsTo(QTime::currentTime());
                }
            }
        }
        //Stop State ~ Stop
    }
}


void Stratege::_addedVehicle(Vehicle* vehicle)
{
    //TODO
//    Modify List of unoccupied zones
    qDebug()  << "Stratege: Vehicle Added: No " << vehicle->id() << " | Type: " << vehicle->vehicleType();
    _mapVehicle2VehicleAttribut->insert(vehicle, new VehicleAttribut(vehicle));
}

void Stratege::_removedVehicle(Vehicle* vehicle)
{
    //TODO
//    Modify List of unoccupied zones
    qDebug()  << "Stratege: Vehicle Removed: No " << vehicle->id();
    qDebug() << _mapVehicle2VehicleAttribut->remove(vehicle);
}

void Stratege::setPolygonZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack)
{
    qDebug() << "setPolygonZoneFromController";
    _mainZonePolygon = mainZonePolygon;
    _zonePolygonDefense = zonePolygonDefense;
    _zonePolygonAttack = zonePolygonAttack;

    emit sendPolygonToZoneController(mainZonePolygon, zonePolygonDefense, zonePolygonAttack);
}

void Stratege::handleZoneControllerRequest()
{
    qDebug() << "handleZoneControllerRequest";
    emit sendPolygonToZoneController(_mainZonePolygon, _zonePolygonDefense, _zonePolygonAttack);
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
//                    qDebug() << "SERVO_OUTPUT_RAW message received";
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
