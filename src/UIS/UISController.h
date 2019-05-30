#pragma once
#ifndef UISController_H
#define UISController_H


#include <QObject>
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"

#include "JsonHelper.h"

class UISController: public QObject
{
    Q_OBJECT

public:
    UISController(QObject* parent = NULL);

//public slots:
//    void receiveStatusTextInJson();
};
#endif
