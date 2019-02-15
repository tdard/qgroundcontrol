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
#include "QMap"

#include <QQmlEngine>

VehicleAttribut::VehicleAttribut()
{
    _role = -1;
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


void Stratege::abortMission()
{
    _startMission = false;
    _abortMission = true; //eventually, instead of having a variable here implement the solution right here
    qDebug() << "Mission Aborted";
    qDebug() << "Time in minutes: " << _time.minute();

}


void Stratege::startMission()
{
    _startMission = true;
    _time.restart();
    qDebug() << "Mission Started";
}


void Stratege::updateData(mavlink_message_t& message)
{
    if (_startMission == true)
    {
        //TODO
        /*
         * Receives a mavlink_message_t and update the member variables
         */


        //If we have started the mission, then this should appears in the console
        Vehicle* activeVehicle = _toolbox->multiVehicleManager()->activeVehicle();
        qDebug() << "Confirmation of active vehicle: " << activeVehicle->id();

    }
}


void Stratege::_addedVehicle(Vehicle* vehicle)
{
    //TODO
    /*
     * Increase the size of the member variables (List or something...)
     */
}

void Stratege::_removedVehicle(Vehicle* vehicle)
{
    //TODO
    /*
     * Decrease the size of the member variables (List or something...)
     */
}

void Stratege::_mtFiltering()
{
    //TODO
}

void Stratege::_taskControl()
{
    //TODO
}

