
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
    _test = 0;
}


void Stratege::abortMission()
{
    _test = 1;
}

void Stratege::startMission()
{
    _test = 2;
}

void Stratege::_dataReceiver()
{
    _test = 3;
}

void Stratege::updateData(mavlink_message_t& message)
{
    //If a drone is connected & send a message to the handlers we modified, then we can ensure that the message is treated.
    qDebug() << "Received signal from UAV: " << message.sysid << " .";
    qDebug() << "Message ID: " << message.msgid << " .";
}

void Stratege::_processData()
{
    _test = 5;
}


