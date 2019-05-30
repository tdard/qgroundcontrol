#include "UISController.h"
#include "QDebug"

UISController::UISController(QObject* parent) : QObject(parent)
{
    qDebug() << "UIS Controller";
}
