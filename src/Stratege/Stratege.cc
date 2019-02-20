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
//#include "QMap"

#include <QQmlEngine>

#include "PlanManager.h"
#include "MissionItem.h"

VehicleAttribut::VehicleAttribut()
{
    _role = UNDEFINED_UAV;
    _lonLatAltCoord = QGeoCoordinate();
    _hdg = UINT16_MAX;  //If we don't know, we set to this Value                        https://mavlink.io/en/messages/common.html#GLOBAL_POSITION_INT
    _targetLonLatAltCoord = QGeoCoordinate();
    _mainServo = QList<int16_t>();
    _auxServo = QList<int16_t>();
}



Stratege::Stratege(QGCApplication* app, QGCToolbox* toolbox): QGCTool(app, toolbox)
{
    _abortMission = false;
    _startMission = false;
    _time = QTime();
    _time.start();
    _vehicleMap = new QMap<Vehicle*, VehicleAttribut*>();
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
    for(auto vm : _vehicleMap->keys())
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

    for(auto vm : _vehicleMap->keys())
    {
        vm->firmwarePlugin()->guidedModeTakeoff(vm, 15);
    }
    _startMission = true;
}


void Stratege::updateData(mavlink_message_t& message)
{



    if (_startMission == true)
    {
        //TODO
        /*
         * Receives a mavlink_message_t and update the member variables
         */


    }
}


void Stratege::_addedVehicle(Vehicle* vehicle)
{
    //TODO
    /*
     * Increase the size of the member variables (List or something...)
     */
    qDebug()  << "Stratege: Vehicle Added";
    _vehicleMap->insert(vehicle, new VehicleAttribut());
}

void Stratege::_removedVehicle(Vehicle* vehicle)
{
    //TODO
    /*
     * Decrease the size of the member variables (List or something...)
     */
    qDebug()  << "Stratege: Vehicle Removed";
    qDebug() << _vehicleMap->remove(vehicle);
}

void Stratege::_mtFiltering()
{
    //TODO
}

void Stratege::_taskControl()
{
    //TODO
}

