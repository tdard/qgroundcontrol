#ifndef Stratege_H
#define Stratege_H

#include "Vehicle.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "QGCToolbox.h"
#include "QGCLoggingCategory.h"


class FirmwarePluginManager;
class QGCApplication;
class MAVLinkProtocol;


class Stratege : public QGCTool
{
    Q_OBJECT

public:
    Stratege(QGCApplication* app, QGCToolbox* toolbox);

public slots:
    void abortMission();
    void startMission();
    void updateData(mavlink_message_t& message);

private slots:
    void _addedVehicle(Vehicle* vehicle); //Modify dynamically the size of the member variables
    void _removedVehicle(Vehicle* vehicle);

private:
    void _mtFiltering();    //is it a void variable ? Yes, let's say yes.
    void _taskControl();

    //Member variables

    bool _abortMission;
    bool _startMission;


    //classification of mavlink messages based on their IP -> A vehicle will have 2 LinkInterface* that have IP information. The associated member variable is _link : add public function link()

    //distinction attack/defense/UGV by vehicle based on their IP
    //->map: IP / role

    //Time
    //->QTime ? for end of the round or at 8/9' strategy
    //QTime* _missionTime;

    //Last measured enemy coordinates
    //List of QGeoCoordinates ??

    //Estimated enemy coordinates
    //Idem ?

    //Estimated UAV/UGV coordinates
    //Idem ?


};

#endif
