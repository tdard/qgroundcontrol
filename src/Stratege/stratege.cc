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
    qDebug() << "Mission Aborted";
    qDebug() << "Time in minutes: " << _time.minute();  //do not return the right time actually.
}


void Stratege::startMission()
{
    _startMission = true;
    _time.restart();
    qDebug() << "Mission Started";
}

void Stratege::take_off()
{
    if (_startMission == true)
	{
		//MAV_CMD_DO_SET_HOME(param1 = 1);
		//MAV_CMD_COMPONENT_ARM_DISARM(param1 = 1);
		//float altitude  = get_takeoff_altitude(ID);
		//MAV_CMD_NAV_TAKEOFF(param7 = altitude);
	}
}

void Stratege::patrol()
{
	if (_targetLonLatAltCoord != [])
	{
		//select the closest target to observe
		
		//gets the heading to look at the enemy and looks towards it
		//get_NMY_heading(_targetLonLatAltCoord, _lonLatAltCoord, hdg);
		//MAV_CMD_CONDITION_YAW(param1 = hdg2NMY ; param4 = 0);
	}
    //QGeoCoordinate travel[] = get_travel();
	//MAV_CMD_NAV_WAYPOINT(param1 = 0 ; param5 = travel[i] ; param6 = travel[i] ; param7 = travel[i]);
}

void Stratege::replace()
{
    //TODO Redefine proprely the zones of concern
}

void Stratege::attack()
{
    QGeoCoordinate 
}

void Stratege::track()
{
    
}

void Stratege::land()
{
    
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
    switch (_role)
	​{
		case DEFENSE_UAV:
			// Defense UAV automat;
			break;

		case ATTACK_UAV:
			// Attack UAV automat;
			break;

		case DEFENSE_UGV:
			// Defense UGV automat;
			break;
			
		default:
			// TODO the UAV is undefined return a error message
	}
}
