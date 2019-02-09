
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

void Stratege::_updateData(mavlink_message_t& message)
{
    _test = 4;
}

void Stratege::_processData()
{
    _test = 5;
}


