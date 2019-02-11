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


#include <QQmlEngine>

Stratege::Stratege(QGCApplication* app, QGCToolbox* toolbox): QGCTool(app, toolbox)
{
    _abortMission = false;
    _startMission = false;
}


void Stratege::abortMission()
{
    _startMission = false;
    _abortMission = true; //eventually, instead of having a variable here implement the solution right here
    qDebug() << "Mission Aborted";
}


void Stratege::startMission()
{
    _startMission = true;
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


