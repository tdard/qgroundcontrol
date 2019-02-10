/// @file
///
///
#ifndef Stratege_H
#define Stratege_H

#include "Vehicle.h"
#include "QGCMAVLink.h"
#include "QmlObjectListModel.h"
#include "QGCToolbox.h"
#include "QGCLoggingCategory.h"

#include <QList>

class FirmwarePluginManager;
class QGCApplication;
class MAVLinkProtocol;


class Stratege : public QGCTool
{
    Q_OBJECT

    //properties

public:
    Stratege(QGCApplication* app, QGCToolbox* toolbox);

signals:
    void missionChanged(/*list of mavlink messages ? patrol = multiple mavlink messages*/);

public slots:
    void abortMission();
    void startMission();
    void updateData(mavlink_message_t& message);

private slots:
    void _dataReceiver();
    void _processData();


private:
    void _mtFiltering();    //is it a void variable ? Yes, let's say yes.
    void _taskControl();

    //Member variables
//List of MAVLink messages ?
//classification of mavlink messages based on their ID !
//slot: onVehicleAdded, onVehicleDestroyed (or in the list of the vehicles) that will modify a member variable
//distinction attack/defense/UGV by vehicle ID

    //QList or QMap ??


    //Time

    //Last measured enemy coordinates

    //Estimated enemy coordinates

    //Estimated UAV/UGV coordinates

    //Key-Value: ID -> type (GDP_Attack / GDP_Defense /GDP_UGV)

    //test variable
    uint32_t _test;

};

#endif
