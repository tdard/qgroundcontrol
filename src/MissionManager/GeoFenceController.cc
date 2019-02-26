/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
///     @author Don Gagne <don@thegagnes.com>

#include "GeoFenceController.h"
#include "Vehicle.h"
#include "FirmwarePlugin.h"
#include "MAVLinkProtocol.h"
#include "QGCApplication.h"
#include "ParameterManager.h"
#include "JsonHelper.h"
#include "QGCQGeoCoordinate.h"
#include "AppSettings.h"
#include "PlanMasterController.h"

#ifndef __mobile__
#include "MainWindow.h"
#include "QGCQFileDialog.h"
#endif

#include <QJsonDocument>
#include <QJsonArray>

QGC_LOGGING_CATEGORY(GeoFenceControllerLog, "GeoFenceControllerLog")

const char* GeoFenceController::_jsonFileTypeValue =        "GeoFence";
const char* GeoFenceController::_jsonBreachReturnKey =      "breachReturn";
const char* GeoFenceController::_jsonPolygonsKey =          "polygons";
const char* GeoFenceController::_jsonCirclesKey =           "circles";
// GDP - Start
const char* GeoFenceController::_jsonPolygonsInfoKey =       "polygonsInfo";
//GDP - Stop

const char* GeoFenceController::_px4ParamCircularFence =    "GF_MAX_HOR_DIST";

GeoFenceController::GeoFenceController(PlanMasterController* masterController, QObject* parent)
    : PlanElementController     (masterController, parent)
    , _geoFenceManager          (_managerVehicle->geoFenceManager())
    , _dirty                    (false)
    , _itemsRequested           (false)
    , _px4ParamCircularFenceFact(NULL)
{
    connect(&_polygons, &QmlObjectListModel::countChanged, this, &GeoFenceController::_updateContainsItems);
    connect(&_circles,  &QmlObjectListModel::countChanged, this, &GeoFenceController::_updateContainsItems);
    // GDP - Start
    connect(&_polygonsInfo, &QmlObjectListModel::countChanged, this, &GeoFenceController::_updateContainsItems);
    //GDP - Stop

    managerVehicleChanged(_managerVehicle);
}

GeoFenceController::~GeoFenceController()
{

}

void GeoFenceController::start(bool flyView)
{
    qCDebug(GeoFenceControllerLog) << "start flyView" << flyView;

    PlanElementController::start(flyView);
    _init();
}

void GeoFenceController::_init(void)
{

}

void GeoFenceController::setBreachReturnPoint(const QGeoCoordinate& breachReturnPoint)
{
    if (_breachReturnPoint != breachReturnPoint) {
        _breachReturnPoint = breachReturnPoint;
        setDirty(true);
        emit breachReturnPointChanged(breachReturnPoint);
    }
}

void GeoFenceController::_signalAll(void)
{
    emit breachReturnPointChanged(breachReturnPoint());
    emit dirtyChanged(dirty());
    emit supportedChanged(supported());
}

void GeoFenceController::managerVehicleChanged(Vehicle* managerVehicle)
{
    if (_managerVehicle) {
        _geoFenceManager->disconnect(this);
        _managerVehicle->disconnect(this);
        _managerVehicle->parameterManager()->disconnect(this);
        _managerVehicle = NULL;
        _geoFenceManager = NULL;
    }

    _managerVehicle = managerVehicle;
    if (!_managerVehicle) {
        qWarning() << "GeoFenceController::managerVehicleChanged managerVehicle=NULL";
        return;
    }

    _geoFenceManager = _managerVehicle->geoFenceManager();
    connect(_geoFenceManager, &GeoFenceManager::loadComplete,                   this, &GeoFenceController::_managerLoadComplete);
    connect(_geoFenceManager, &GeoFenceManager::sendComplete,                   this, &GeoFenceController::_managerSendComplete);
    connect(_geoFenceManager, &GeoFenceManager::removeAllComplete,              this, &GeoFenceController::_managerRemoveAllComplete);
    connect(_geoFenceManager, &GeoFenceManager::inProgressChanged,              this, &GeoFenceController::syncInProgressChanged);

    connect(_managerVehicle,  &Vehicle::capabilityBitsChanged,                  this, &GeoFenceController::supportedChanged);

    connect(_managerVehicle->parameterManager(), &ParameterManager::parametersReadyChanged, this, &GeoFenceController::_parametersReady);
    _parametersReady();

    emit supportedChanged(supported());
    _signalAll();
}

bool GeoFenceController::load(const QJsonObject& json, QString& errorString)
{
    removeAll();

    errorString.clear();

    if (!json.contains(JsonHelper::jsonVersionKey) ||
            (json.contains(JsonHelper::jsonVersionKey) && json[JsonHelper::jsonVersionKey].toInt() == 1)) {
        // We just ignore old version 1 or prior data
        return true;
    }

    QList<JsonHelper::KeyValidateInfo> keyInfoList = {
        { JsonHelper::jsonVersionKey,   QJsonValue::Double, true },
        { _jsonCirclesKey,              QJsonValue::Array,  true },
        { _jsonPolygonsKey,             QJsonValue::Array,  true },
    };
    if (!JsonHelper::validateKeys(json, keyInfoList, errorString)) {
        return false;
    }

    if (json[JsonHelper::jsonVersionKey].toInt() != _jsonCurrentVersion) {
        errorString = tr("GeoFence supports version %1").arg(_jsonCurrentVersion);
        return false;
    }

    QJsonArray jsonPolygonArray = json[_jsonPolygonsKey].toArray();
    for (const QJsonValue& jsonPolygonValue: jsonPolygonArray) {
        if (jsonPolygonValue.type() != QJsonValue::Object) {
            errorString = tr("GeoFence polygon not stored as object");
            return false;
        }

        QGCFencePolygon* fencePolygon = new QGCFencePolygon(false /* inclusion */, this /* parent */);
        if (!fencePolygon->loadFromJson(jsonPolygonValue.toObject(), true /* required */, errorString)) {
            return false;
        }
        _polygons.append(fencePolygon);
    }

    QJsonArray jsonCircleArray = json[_jsonCirclesKey].toArray();
    for (const QJsonValue& jsonCircleValue: jsonCircleArray) {
        if (jsonCircleValue.type() != QJsonValue::Object) {
            errorString = tr("GeoFence circle not stored as object");
            return false;
        }

        QGCFenceCircle* fenceCircle = new QGCFenceCircle(this /* parent */);
        if (!fenceCircle->loadFromJson(jsonCircleValue.toObject(), errorString)) {
            return false;
        }
        _circles.append(fenceCircle);
    }

    // GDP - Start
    QJsonArray jsonPolygonInfoArray = json[_jsonPolygonsInfoKey].toArray();
    for (const QJsonValue& jsonPolygonInfoValue: jsonPolygonInfoArray) {
        if (jsonPolygonInfoValue.type() != QJsonValue::Object) {
            errorString = tr("GeoFence polygonInfo not stored as object");
            return false;
        }

        QGCFencePolygon* fencePolygonInfo = new QGCFencePolygon(false /* inclusion */, this /* parent */);
        if (!fencePolygonInfo->loadFromJson(jsonPolygonInfoValue.toObject(), true /* required */, errorString)) {
            return false;
        }
        _polygonsInfo.append(fencePolygonInfo);
    }
    // GDP - Stop

    setDirty(false);
    _signalAll();

    return true;
}

void GeoFenceController::save(QJsonObject& json)
{
    json[JsonHelper::jsonVersionKey] = _jsonCurrentVersion;

    QJsonArray jsonPolygonArray;
    for (int i=0; i<_polygons.count(); i++) {
        QJsonObject jsonPolygon;
        QGCFencePolygon* fencePolygon = _polygons.value<QGCFencePolygon*>(i);
        fencePolygon->saveToJson(jsonPolygon);
        jsonPolygonArray.append(jsonPolygon);
    }
    json[_jsonPolygonsKey] = jsonPolygonArray;

    QJsonArray jsonCircleArray;
    for (int i=0; i<_circles.count(); i++) {
        QJsonObject jsonCircle;
        QGCFenceCircle* fenceCircle = _circles.value<QGCFenceCircle*>(i);
        fenceCircle->saveToJson(jsonCircle);
        jsonCircleArray.append(jsonCircle);
    }
    json[_jsonCirclesKey] = jsonCircleArray;

    // GDP - Start
    QJsonArray jsonPolygonInfoArray;
    for (int i=0; i<_polygonsInfo.count(); i++) {
        QJsonObject jsonPolygonInfo;
        QGCFencePolygon* fencePolygonInfo = _polygonsInfo.value<QGCFencePolygon*>(i);
        fencePolygonInfo->saveToJson(jsonPolygonInfo);
        jsonPolygonInfoArray.append(jsonPolygonInfo);
    }
    json[_jsonPolygonsInfoKey] = jsonPolygonInfoArray;
    // GDP - Stop
}

void GeoFenceController::removeAll(void)
{    
    setBreachReturnPoint(QGeoCoordinate());
    _polygons.clearAndDeleteContents();
    _circles.clearAndDeleteContents();
}

void GeoFenceController::removeAllFromVehicle(void)
{
    if (_masterController->offline()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::removeAllFromVehicle called while offline";
    } else if (syncInProgress()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::removeAllFromVehicle called while syncInProgress";
    } else {
        _geoFenceManager->removeAll();
    }
}

void GeoFenceController::loadFromVehicle(void)
{
    if (_masterController->offline()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::loadFromVehicle called while offline";
    } else if (syncInProgress()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::loadFromVehicle called while syncInProgress";
    } else {
        _itemsRequested = true;
        _geoFenceManager->loadFromVehicle();
    }
}

void GeoFenceController::sendToVehicle(void)
{
    if (_masterController->offline()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::sendToVehicle called while offline";
    } else if (syncInProgress()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::sendToVehicle called while syncInProgress";
    } else {
        qCDebug(GeoFenceControllerLog) << "GeoFenceController::sendToVehicle";
        _geoFenceManager->sendToVehicle(_breachReturnPoint, _polygons, _circles);
        setDirty(false);
    }
}

bool GeoFenceController::syncInProgress(void) const
{
    return _geoFenceManager->inProgress();
}

bool GeoFenceController::dirty(void) const
{
    return _dirty;
}


void GeoFenceController::setDirty(bool dirty)
{
    if (dirty != _dirty) {
        _dirty = dirty;
        if (!dirty) {
            for (int i=0; i<_polygons.count(); i++) {
                QGCFencePolygon* polygon = _polygons.value<QGCFencePolygon*>(i);
                polygon->setDirty(false);
            }
            for (int i=0; i<_circles.count(); i++) {
                QGCFenceCircle* circle = _circles.value<QGCFenceCircle*>(i);
                circle->setDirty(false);
            }
            // GDP - Start
            for (int i=0; i<_polygonsInfo.count(); i++) {
                QGCFencePolygon* polygonInfo = _polygonsInfo.value<QGCFencePolygon*>(i);
                polygonInfo->setDirty(false);
            }
            // GDP - Stop
        }
        emit dirtyChanged(dirty);
    }
}

void GeoFenceController::_polygonDirtyChanged(bool dirty)
{
    if (dirty) {
        setDirty(true);
    }
}

void GeoFenceController::_setDirty(void)
{
    setDirty(true);
}

void GeoFenceController::_setFenceFromManager(const QList<QGCFencePolygon>& polygons,
                                              const QList<QGCFenceCircle>&  circles)
{
    _polygons.clearAndDeleteContents();
    _circles.clearAndDeleteContents();

    for (int i=0; i<polygons.count(); i++) {
        _polygons.append(new QGCFencePolygon(polygons[i], this));
    }

    for (int i=0; i<circles.count(); i++) {
        _circles.append(new QGCFenceCircle(circles[i], this));
    }

    setDirty(false);
}

void GeoFenceController::_setReturnPointFromManager(QGeoCoordinate breachReturnPoint)
{
    _breachReturnPoint = breachReturnPoint;
    emit breachReturnPointChanged(_breachReturnPoint);
}

void GeoFenceController::_managerLoadComplete(void)
{
    // Fly view always reloads on _loadComplete
    // Plan view only reloads on _loadComplete if specifically requested
    if (_flyView || _itemsRequested) {
        _setReturnPointFromManager(_geoFenceManager->breachReturnPoint());
        _setFenceFromManager(_geoFenceManager->polygons(), _geoFenceManager->circles());
        setDirty(false);
        _signalAll();
        emit loadComplete();
    }
    _itemsRequested = false;
}

void GeoFenceController::_managerSendComplete(bool error)
{
    // Fly view always reloads on manager sendComplete
    if (!error && _flyView) {
        showPlanFromManagerVehicle();
    }
}

void GeoFenceController::_managerRemoveAllComplete(bool error)
{
    if (!error) {
        // Remove all from vehicle so we always update
        showPlanFromManagerVehicle();
    }
}

bool GeoFenceController::containsItems(void) const
{
    return _polygons.count() > 0 || _circles.count() /* GDP - Start */|| _polygonsInfo.count() /* GDP - Stop */ > 0;
}

void GeoFenceController::_updateContainsItems(void)
{
    emit containsItemsChanged(containsItems());
}

bool GeoFenceController::showPlanFromManagerVehicle(void)
{
    qCDebug(GeoFenceControllerLog) << "showPlanFromManagerVehicle _flyView" << _flyView;
    if (_masterController->offline()) {
        qCWarning(GeoFenceControllerLog) << "GeoFenceController::showPlanFromManagerVehicle called while offline";
        return true;    // stops further propagation of showPlanFromManagerVehicle due to error
    } else {
        _itemsRequested = true;
        if (!_managerVehicle->initialPlanRequestComplete()) {
            // The vehicle hasn't completed initial load, we can just wait for loadComplete to be signalled automatically
            qCDebug(GeoFenceControllerLog) << "showPlanFromManagerVehicle: !initialPlanRequestComplete, wait for signal";
            return true;
        } else if (syncInProgress()) {
            // If the sync is already in progress, _loadComplete will be called automatically when it is done. So no need to do anything.
            qCDebug(GeoFenceControllerLog) << "showPlanFromManagerVehicle: syncInProgress wait for signal";
            return true;
        } else {
            // Fake a _loadComplete with the current items
            qCDebug(GeoFenceControllerLog) << "showPlanFromManagerVehicle: sync complete simulate signal";
            _itemsRequested = true;
            _managerLoadComplete();
            return false;
        }
    }
}

void GeoFenceController::addInclusionPolygon(QGeoCoordinate topLeft, QGeoCoordinate bottomRight)
{
    QGeoCoordinate topRight(topLeft.latitude(), bottomRight.longitude());
    QGeoCoordinate bottomLeft(bottomRight.latitude(), topLeft.longitude());

    double halfWidthMeters = topLeft.distanceTo(topRight) / 2.0;
    double halfHeightMeters = topLeft.distanceTo(bottomLeft) / 2.0;

    QGeoCoordinate centerLeftEdge = topLeft.atDistanceAndAzimuth(halfHeightMeters, 180);
    QGeoCoordinate centerTopEdge = topLeft.atDistanceAndAzimuth(halfWidthMeters, 90);
    QGeoCoordinate center(centerLeftEdge.latitude(), centerTopEdge.longitude());

    // Initial polygon is inset to take 3/4s of viewport with max width/height of 3000 meters
    halfWidthMeters =   qMin(halfWidthMeters * 0.75, 1500.0);
    halfHeightMeters =  qMin(halfHeightMeters * 0.75, 1500.0);

    // Initial polygon has max width and height of 3000 meters
    topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 0);
    topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 0);
    bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 180);
    bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 180);

    QGCFencePolygon* polygon = new QGCFencePolygon(true /* inclusion */, this);
    polygon->appendVertex(topLeft);
    polygon->appendVertex(topRight);
    polygon->appendVertex(bottomRight);
    polygon->appendVertex(bottomLeft);
    _polygons.append(polygon);

    clearAllInteractive();
    polygon->setInteractive(true);
}

void GeoFenceController::addInclusionCircle(QGeoCoordinate topLeft, QGeoCoordinate bottomRight)
{
    QGeoCoordinate topRight(topLeft.latitude(), bottomRight.longitude());
    QGeoCoordinate bottomLeft(bottomRight.latitude(), topLeft.longitude());

    // Initial radius is inset to take 3/4s of viewport and max of 1500 meters
    double halfWidthMeters = topLeft.distanceTo(topRight) / 2.0;
    double halfHeightMeters = topLeft.distanceTo(bottomLeft) / 2.0;
    double radius = qMin(qMin(halfWidthMeters, halfHeightMeters) * 0.75, 1500.0);

    QGeoCoordinate centerLeftEdge = topLeft.atDistanceAndAzimuth(halfHeightMeters, 180);
    QGeoCoordinate centerTopEdge = topLeft.atDistanceAndAzimuth(halfWidthMeters, 90);
    QGeoCoordinate center(centerLeftEdge.latitude(), centerTopEdge.longitude());

    QGCFenceCircle* circle = new QGCFenceCircle(center, radius, true /* inclusion */, this);
    _circles.append(circle);

    clearAllInteractive();
    circle->setInteractive(true);
}

void GeoFenceController::deletePolygon(int index)
{
    if (index < 0 || index > _polygons.count() - 1) {
        return;
    }

    QGCFencePolygon* polygon = qobject_cast<QGCFencePolygon*>(_polygons.removeAt(index));
    polygon->deleteLater();
}

void GeoFenceController::deleteCircle(int index)
{
    if (index < 0 || index > _circles.count() - 1) {
        return;
    }

    QGCFenceCircle* circle = qobject_cast<QGCFenceCircle*>(_circles.removeAt(index));
    circle->deleteLater();
}

void GeoFenceController::clearAllInteractive(void)
{
    for (int i=0; i<_polygons.count(); i++) {
        _polygons.value<QGCFencePolygon*>(i)->setInteractive(false);
    }
    for (int i=0; i<_circles.count(); i++) {
        _circles.value<QGCFenceCircle*>(i)->setInteractive(false);
    }
}

//GDP - Start
void GeoFenceController::addInclusionPolygonInfo(QGeoCoordinate center, int height, int width)
{
    qDebug() << "addInclusionPolygonInfo";

    double halfWidthMeters = width / 2.0;
    double halfHeightMeters = height / 2.0;

    QGeoCoordinate topLeft =           center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate topRight =          center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 0);
    QGeoCoordinate bottomLeft =        center.atDistanceAndAzimuth(halfWidthMeters, -90).atDistanceAndAzimuth(halfHeightMeters, 180);
    QGeoCoordinate bottomRight =       center.atDistanceAndAzimuth(halfWidthMeters, 90).atDistanceAndAzimuth(halfHeightMeters, 180);

    QGCFencePolygon* polygonInfo = new QGCFencePolygon(true /* inclusion */, this);
    polygonInfo->appendVertex(topLeft);
    polygonInfo->appendVertex(topRight);
    polygonInfo->appendVertex(bottomRight);
    polygonInfo->appendVertex(bottomLeft);
    _polygonsInfo.append(polygonInfo);

    qDebug() << "Number of PolygonInfo: " << _polygonsInfo.count();

    clearAllInteractive();
    polygonInfo->setInteractive(true);
}

void GeoFenceController::deletePolygonInfo(int index)
{
    qDebug()<< "deletePolygonInfo";
    if (index < 0 || index > _polygonsInfo.count() - 1){
        return;
    }

    QGCFencePolygon* polygonInfo = qobject_cast<QGCFencePolygon*>(_polygonsInfo.removeAt(index));
    polygonInfo->deleteLater();
}

void GeoFenceController::rotatePolygonInfo(int index, int rotation)
{
    qDebug() << "rotatePolygonInfo";
}
// GDP - Stop

bool GeoFenceController::supported(void) const
{
    return (_managerVehicle->capabilityBits() & MAV_PROTOCOL_CAPABILITY_MISSION_FENCE) && (_managerVehicle->maxProtoVersion() >= 200);
}

// Hack for PX4
double GeoFenceController::paramCircularFence(void)
{
    if (_managerVehicle->isOfflineEditingVehicle() || !_managerVehicle->parameterManager()->parameterExists(FactSystem::defaultComponentId, _px4ParamCircularFence)) {
        return 0;
    }

    return _managerVehicle->parameterManager()->getParameter(FactSystem::defaultComponentId, _px4ParamCircularFence)->rawValue().toDouble();
}

void GeoFenceController::_parametersReady(void)
{
    if (_px4ParamCircularFenceFact) {
        _px4ParamCircularFenceFact->disconnect(this);
        _px4ParamCircularFenceFact = NULL;
    }

    if (_managerVehicle->isOfflineEditingVehicle() || !_managerVehicle->parameterManager()->parameterExists(FactSystem::defaultComponentId, _px4ParamCircularFence)) {
        emit paramCircularFenceChanged();
        return;
    }

    _px4ParamCircularFenceFact = _managerVehicle->parameterManager()->getParameter(FactSystem::defaultComponentId, _px4ParamCircularFence);
    connect(_px4ParamCircularFenceFact, &Fact::rawValueChanged, this, &GeoFenceController::paramCircularFenceChanged);
    emit paramCircularFenceChanged();
}
