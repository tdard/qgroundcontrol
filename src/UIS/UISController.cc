#include "UISController.h"
#include "QDebug"

UISController::UISController(QObject* parent) : QObject(parent)
{
    qDebug() << "UIS Controller";
}

void UISController::receiveStatusTextFromMVM(QJsonDocument jsonDoc)
{
    qDebug() << "Json document received";
    // Parse the jsonDocument given the keys: Letter, Lon, Lat
    // Cast each of the letters in _letters into Letter, Lon, Lat
    // Iterate on each letter in _letters, and compare 1) The letter 2) the relative distance from one to another
    // Given the result, 1) update the position of the letter with Kalman filtering 2) or add a new element to _letters
}
