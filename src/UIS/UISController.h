#pragma once
#ifndef UISController_H
#define UISController_H


#include <QObject>


#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"

class UISController: public QObject
{
    Q_OBJECT

public:
    UISController(QObject* parent = NULL);
};
#endif
