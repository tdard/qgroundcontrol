#ifndef Stratege_H
#define Stratege_H


#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "QGCToolbox.h"
#include "QGCLoggingCategory.h"

#include "QMap"
#include "QGeoCoordinate"
#include "QTime"
#include "QVector3D"
#include "QGCMapPolygon.h"
#include "QGCMapCircle.h"

class FirmwarePluginManager;
class QGCApplication;
class MAVLinkProtocol;
class Vehicle;

//Competition time
#define COMPETITION_TIME 60

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
#define GO_ATTACK_TIME_M    8       //To change to real value


class VehicleAttribut
{
public:
      VehicleAttribut(Vehicle*);

      void setRole(int role) { _role = role; }
      Vehicle* vehicle                      () { return _vehicle; }
      int role                              () { return _role; }
      QGeoCoordinate targetLonLatAltCoord   () { return _targetLonLatAltCoord; }
      mavlink_servo_output_raw_t* mainServo () { return _mainServo; }
      mavlink_servo_output_raw_t* auxServo  () { return _auxServo; }  

      int _defpatroliter;
      int _attpatroliter;
      bool  _inpatrol;
      bool  _inattack;

private:
      Vehicle* _vehicle;
      int _role;
      //Information on the last enemy spotted by this vehicle
      QGeoCoordinate _targetLonLatAltCoord;

      //Servo Information, PPM Modulation
      mavlink_servo_output_raw_t* _mainServo;
      mavlink_servo_output_raw_t* _auxServo;

};



class Stratege : public QGCTool
{
    Q_OBJECT

public:
    Stratege(QGCApplication* app, QGCToolbox* toolbox);

    virtual void setToolbox(QGCToolbox* toolbox);

signals:
    void sendPolygonToZoneController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);
    void sendCircleToZoneController(QList<QGCMapCircle*> mainZoneCircle, QList<QGCMapCircle*> zoneCircleDefense, QList<QGCMapCircle*> zoneCircleAttack);

public slots:
    void abortMission();
    void startMission();
    void updateData(mavlink_message_t message);
    void setPolygonZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);
    void setCircleZoneFromController(QList<QGCMapCircle*> mainZoneCircle, QList<QGCMapCircle*> zoneCircleDefense, QList<QGCMapCircle*> zoneCircleAttack);
    void handleZoneControllerRequestPolygon();
    void handleZoneControllerRequestCircle();

private slots:
    void _addedVehicle(Vehicle* vehicle); //Modify dynamically the size of the member variables
    void _removedVehicle(Vehicle* vehicle);
    //void _taskControl(bool activeMission);     //dictionnary associating position & speed for designated targets

private:
    void _patrol(Vehicle* vm);
    void _attack(Vehicle* vm);
    void _parse(mavlink_message_t message);

    //Member variables
    bool _abortMission;
    bool _startMission;
    //bool _activeMission;
    QTime                               _time;
    QMap<Vehicle*, VehicleAttribut*>*   _mapVehicle2VehicleAttribut;                                                   //Assign to each Vehicle* an associated VehicleAttribut*
    QMap<QGeoCoordinate, QVector3D>*    _mapTargetsPositions2TargetsVelocities;
    QList<QGCMapPolygon*>               _mainZonePolygon;
    QList<QGCMapPolygon*>               _zonePolygonDefense;
    QList<QGCMapPolygon*>               _zonePolygonAttack;
    QList<QGCMapCircle*>                _mainZoneCircle;
    QList<QGCMapCircle*>                _zoneCircleDefense;
    QList<QGCMapCircle*>                _zoneCircleAttack;
    bool                                _isCircularZone;

};



#endif
