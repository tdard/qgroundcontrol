#pragma once
#ifndef UISController_H
#define UISController_H


#include <QObject>
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"

#include "QJsonDocument"

class UISController: public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal                confidence              READ confidence                                     CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  letters                 READ letters                                        CONSTANT)
public:
    UISController(QObject* parent = nullptr);



    qreal confidence                () { return _confidence; }
    QmlObjectListModel* letters     () { return &_letters; }

public slots:
    void receiveStatusTextFromMVM(QJsonDocument jsonDoc);

private:
    qreal _confidence = 5.0;
    QmlObjectListModel _letters;
};
#endif
