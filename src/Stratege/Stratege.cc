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
    }
    qDebug() << "Abortion of mission finished";
    qDebug() << "Current time: " << QTime::currentTime().toString();
    qDebug() << "Timespan (s) : " << _time.secsTo(QTime::currentTime());
}


void Stratege::startMission()
{
    _time.restart();
    qDebug() << "Mission Started: time: " << _time.toString();

    for(auto vm : _mapVehicle2VehicleAttribut->keys())
    {
        vm->firmwarePlugin()->guidedModeTakeoff(vm, 15);
    }
    _startMission = true;
}


void Stratege::updateData(mavlink_message_t message)
{
    _parse(message);
    _mtFiltering();

    if (_startMission == true)
    {
        //Stop State ~ Start
        if (_time.secsTo(QTime::currentTime()) == COMPETITION_TIME)
        {
            _startMission = false;
            for(auto vm : _mapVehicle2VehicleAttribut->keys())
            {
                if (vm->vehicleType() != MAV_TYPE_GROUND_ROVER){ vm->firmwarePlugin()->guidedModeLand(vm); }
            }
        }
        //Stop State ~ Stop

        //Test: get main information ~ Start
        qDebug() << "Relative Altitude to home position: " << _mapVehicle2VehicleAttribut->keys().first()->coordinate().altitude() - _mapVehicle2VehicleAttribut->keys().first()->homePosition().altitude();   //relative altitude, negligate terrain altitude variation. Is home set at right position everytime ?
        qDebug() << "Latitude: " << _mapVehicle2VehicleAttribut->keys().first()->coordinate().latitude();
        qDebug() << "Longitude: " << _mapVehicle2VehicleAttribut->keys().first()->coordinate().longitude();
        qDebug() << "Heading: " << _mapVehicle2VehicleAttribut->keys().first()->heading()->rawValue().toDouble();       // Clockwise: 0->360. CounterClockWise: 360->0
        qDebug() << "UAV Type: " << _mapVehicle2VehicleAttribut->first()->role();
        //Test ~ Stop
    }
}


void Stratege::_addedVehicle(Vehicle* vehicle)
{
    //TODO
//    Modify List of unoccupied zones
    qDebug()  << "Stratege: Vehicle Added: No " << vehicle->id();
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


void Stratege::_mtFiltering()
{
    //TODO
    // Update Targets information: _mapTargetsPositions2TargetsVelocities
}

void Stratege::_taskControl()
{
    //TODO
}

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
