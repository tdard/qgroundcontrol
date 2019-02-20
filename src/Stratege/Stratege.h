#ifndef Stratege_H
#define Stratege_H

//#include "Vehicle.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "QGCToolbox.h"
#include "QGCLoggingCategory.h"

#include "QMap"
#include "QGeoCoordinate"
#include "QTime"

class FirmwarePluginManager;
class QGCApplication;
class MAVLinkProtocol;
class Vehicle;

//Zones
#define DEFENSE_ZONE     0
#define ATTACK_ZONE      1
#define NEUTRAL_ZONE     2
#define OUTSIDE_ZONE     3


//Roles
#define UNDEFINED_UAV   -1
#define DEFENSE_UAV     0
#define ATTACK_UAV      1
#define DEFENSE_UGV     2

//Value of PWM
#define PWM_OPEN_SERVO  1800        //To change to real value
#define PWM_CLOSE_SERVO 1100        //To change to real value
#define MOTOR_MAX_PWM   1800        //To change to real value

//Time values for Strategy
#define START_TIME_M        0
#define STOP_TIME_M         10
#define GO_ATTACK_TIME_M    8       //To change to real value


class VehicleAttribut
{
public:
      VehicleAttribut();                                                                                        // Implementation in Stratege.cc

//      bool isInZone(int zone);                                                                                  // Implementation in Stratege.cc
//      bool isHookOpened();                                                                                      // Implementation in Stratege.cc
//      bool isThrottleTooLarge();                                                                                // Implementation in Stratege.cc


      void setRole(int role) { _role = role; }
//      void setlonLatAltCoord(float north, float east, float down);                                              // Implementation in Stratege.cc        //Need to convert from NED to Lon Lat Alt
      void setHdg(uint16_t hdg) { _hdg = hdg; }
//      void setTargetLonLatAltCoord(int32_t lat, int32_t lon, float alt);                                        // Implementation in Stratege.cc
//      void setMainServo(int16_t servo1_raw = 0, int16_t servo2_raw = 0, int16_t servo3_raw = 0, int16_t servo4_raw = 0, int16_t servo5_raw = 0, int16_t servo6_raw = 0, int16_t servo7_raw = 0, int16_t servo8_raw = 0);      // Implementation in Stratege.cc
//      void setAuxServo(int16_t servo1_raw = 0, int16_t servo2_raw = 0, int16_t servo3_raw = 0, int16_t servo4_raw = 0, int16_t servo5_raw = 0, int16_t servo6_raw = 0, int16_t servo7_raw = 0, int16_t servo8_raw = 0);       // Implementation in Stratege.cc


      int role                              () { return _role; }
      QGeoCoordinate lonLatAltCoord         () { return _lonLatAltCoord; }
      uint16_t hdg                          () { return _hdg; }
      QGeoCoordinate targetLonLatAltCoord   () { return _targetLonLatAltCoord; }
      //float targetVelocity[3]               () { return _targetVelocity; }
      QList<int16_t> mainServo              () { return _mainServo; }
      QList<int16_t> auxServo               () { return _auxServo; }


private:
      int _role;
      QGeoCoordinate _lonLatAltCoord;
      //Heading
      uint16_t _hdg;

      //Information on the last enemy spotted by this vehicle
      QGeoCoordinate _targetLonLatAltCoord;
      //float _targetVelocity[3];

      //Servo Information, PPM Modulation
      QList<int16_t> _mainServo;
      QList<int16_t> _auxServo;
};



class Stratege : public QGCTool
{
    Q_OBJECT

public:
    Stratege(QGCApplication* app, QGCToolbox* toolbox);

    virtual void setToolbox(QGCToolbox* toolbox);

    //TODO: correctly initialize the coordinates describing the area of interest

    //Geofence Information                                                                          //To modify: set the real coordinates for the competition. Hypothesis: zones are rectangles

//    //Area of interest: DEFENSE + NEUTRAL + ATTACK
//    static const QGeoCoordinate globalTopLeft = QGeoCoordinate();                                   //Hint: to access to these variables, use Stratege::globalTopLeft
//    static const QGeoCoordinate globalTopRight = QGeoCoordinate();
//    static const QGeoCoordinate globalBottomLeft = QGeoCoordinate();
//    static const QGeoCoordinate globalBottomRight = QGeoCoordinate();

//    //Defense zone coordinates
//    static const QGeoCoordinate defenseTopLeft = QGeoCoordinate();
//    static const QGeoCoordinate defenseTopRight = QGeoCoordinate();
//    static const QGeoCoordinate defenseBottomLeft = QGeoCoordinate();
//    static const QGeoCoordinate defenseBottomRight = QGeoCoordinate();

//    //Attack zone coordinates
//    static const QGeoCoordinate attackTopLeft = QGeoCoordinate();
//    static const QGeoCoordinate attackTopRight = QGeoCoordinate();
//    static const QGeoCoordinate attackBottomLeft = QGeoCoordinate();
//    static const QGeoCoordinate attackBottomRight = QGeoCoordinate();

//    //Neutral zone coordinates
//    static const QGeoCoordinate neutralTopLeft = QGeoCoordinate();
//    static const QGeoCoordinate neutralTopRight = QGeoCoordinate();
//    static const QGeoCoordinate neutralBottomLeft = QGeoCoordinate();
//    static const QGeoCoordinate neutralBottomRight = QGeoCoordinate();

public slots:
    void abortMission();
    void startMission();
    void updateData(mavlink_message_t& message);

private slots:
    void _addedVehicle(Vehicle* vehicle); //Modify dynamically the size of the member variables
    void _removedVehicle(Vehicle* vehicle);

private:
    void _mtFiltering();                                                                            //To modify: return list of QGeoGoordinates representing true enemy position. Return also speed indication-> not this type then ??
    void _taskControl();

    //Member variables

    bool _abortMission;
    bool _startMission;

    QTime _time;

    QMap<Vehicle*, VehicleAttribut*>* _vehicleMap;                                                   //Assign to each Vehicle* an associated VehicleAttribut*

};



#endif
