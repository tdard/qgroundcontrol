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
      QGCMapPolygon* defpatrolzone          () { return _defpatrolzone; }
      QGCMapPolygon* attpatrolzone          () { return _attpatrolzone; }


private:
      Vehicle* _vehicle;
      int _role;
      //Information on the last enemy spotted by this vehicle
      QGeoCoordinate _targetLonLatAltCoord;

      //Servo Information, PPM Modulation
      mavlink_servo_output_raw_t* _mainServo;
      mavlink_servo_output_raw_t* _auxServo;

      QGCMapPolygon* _defpatrolzone;
      QGCMapPolygon* _attpatrolzone;
};



class Stratege : public QGCTool
{
    Q_OBJECT

public:
    Stratege(QGCApplication* app, QGCToolbox* toolbox);

    virtual void setToolbox(QGCToolbox* toolbox);

signals:
    void sendPolygonToZoneController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);

public slots:
    void abortMission();
    void startMission();
    void updateData(mavlink_message_t message);
    void setPolygonZoneFromController(QList<QGCMapPolygon*> mainZonePolygon, QList<QGCMapPolygon*> zonePolygonDefense, QList<QGCMapPolygon*> zonePolygonAttack);
    void handleZoneControllerRequest();

private slots:
    void _addedVehicle(Vehicle* vehicle); //Modify dynamically the size of the member variables
    void _removedVehicle(Vehicle* vehicle);


private:
    void _mtFiltering();                                                                            //To modify: return list of QGeoGoordinates representing true enemy position. Return also speed indication-> not this type then ??
    void _taskControl();     //dictionnary associating position & speed for designated targets
    void _patrol(Vehicle* vm);
    void _track(Vehicle* vm);
    void _replace(Vehicle* vm);
    void _attack(Vehicle* vm);
    void _parse(mavlink_message_t message);

    //Member variables
    bool _abortMission;
    bool _startMission;
    QTime _time;
    QMap<Vehicle*, VehicleAttribut*>* _mapVehicle2VehicleAttribut;                                                   //Assign to each Vehicle* an associated VehicleAttribut*
    QMap<QGeoCoordinate, QVector3D>* _mapTargetsPositions2TargetsVelocities;
    QList<QGCMapPolygon*> _mainZonePolygon;
    QList<QGCMapPolygon*> _zonePolygonDefense;
    QList<QGCMapPolygon*> _zonePolygonAttack;

};



#endif
