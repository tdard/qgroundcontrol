
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
    _count = 1;
    _arm = true;


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

void Stratege::take_off(int ID, Position):
{
	str mode = get_mode(ID)
	zone zone_concern = get_zone_concern()
	if (mode == "defense")
		WP objective =  
}

void Statege::patrol(ID, position, NMY_position)
{
	str mode = get_mode(ID)
	zone zone_concern = get_zone_concern(ID, mode)
	WP_list travel = get_travel(ID, mode)
	WP objective = get_closest_WP(position, travel)
	while(0):
		signal send(objective)
		if (objective reached):
			objective = next(travel, objective)
		else:
			if ((have_net==0) AND (NMY_position in zone_concern.border)):
				->go to TRACK
	return(0)		
}

void TRACK(ID, Position, NMY_position)
{
	str mode = get_mode(ID)
	zone zone_concern = get_zone_concern(ID, mode)
	WP objective = NMY_position
	signal send(objective)
		if (objective reached):
			objective = next(travel, objective)
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

void Stratege::_processData()
{
    //TODO
    /*
     * Assuming that the variables of interest are the member variables, write the algorithm of multi-target filtering & task control.
     */
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


