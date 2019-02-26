/****************************************************************************
 *
 *   (c) 2009-2016 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef GeoFenceController_H
#define GeoFenceController_H

#include "PlanElementController.h"
#include "GeoFenceManager.h"
#include "QGCFencePolygon.h"
#include "QGCFenceCircle.h"
#include "Vehicle.h"
#include "MultiVehicleManager.h"
#include "QGCLoggingCategory.h"

Q_DECLARE_LOGGING_CATEGORY(GeoFenceControllerLog)

// GDP - Start
#define POLYGON_INFO_WIDTH  100
#define POLYGON_INFO_HEIGHT 100
// GDP - Stop

class GeoFenceManager;

class GeoFenceController : public PlanElementController
{
    Q_OBJECT
    
public:
    GeoFenceController(PlanMasterController* masterController, QObject* parent = NULL);
    ~GeoFenceController();

    Q_PROPERTY(QmlObjectListModel*  polygons            READ polygons                                       CONSTANT)
    Q_PROPERTY(QmlObjectListModel*  circles             READ circles                                        CONSTANT)
    // GDP - Start
    Q_PROPERTY(QmlObjectListModel*  polygonsInfo        READ polygonsInfo                                   CONSTANT)
    //GDP - Stop
    Q_PROPERTY(QGeoCoordinate       breachReturnPoint   READ breachReturnPoint  WRITE setBreachReturnPoint  NOTIFY breachReturnPointChanged)

    // Hack to expose PX4 circular fence controlled by GF_MAX_HOR_DIST
    Q_PROPERTY(double               paramCircularFence  READ paramCircularFence                             NOTIFY paramCircularFenceChanged)

    /// Add a new inclusion polygon to the fence
    ///     @param topLeft - Top left coordinate or map viewport
    ///     @param topLeft - Bottom right left coordinate or map viewport
    Q_INVOKABLE void addInclusionPolygon(QGeoCoordinate topLeft, QGeoCoordinate bottomRight);

    /// Add a new inclusion circle to the fence
    ///     @param topLeft - Top left coordinate or map viewport
    ///     @param topLeft - Bottom right left coordinate or map viewport
    Q_INVOKABLE void addInclusionCircle(QGeoCoordinate topLeft, QGeoCoordinate bottomRight);

    /// Deletes the specified polygon from the polygon list
    ///     @param index Index of poygon to delete
    Q_INVOKABLE void deletePolygon(int index);

    /// Deletes the specified circle from the circle list
    ///     @param index Index of circle to delete
    Q_INVOKABLE void deleteCircle(int index);

    /// Clears the interactive bit from all fence items
    Q_INVOKABLE void clearAllInteractive(void);

    // GDP - Start
    ///     @param topLeft - Top left coordinate or map viewport
    ///     @param topLeft - Bottom right left coordinate or map viewport
    Q_INVOKABLE void addInclusionPolygonInfo(QGeoCoordinate center, int height = POLYGON_INFO_HEIGHT, int width = POLYGON_INFO_WIDTH);

    ///     @param index Index of poygon to delete
    Q_INVOKABLE void deletePolygonInfo(int index);

    Q_INVOKABLE void rotatePolygonInfo(int index, int rotation);
    // GDP - Stop

    double paramCircularFence(void);

    // Overrides from PlanElementController
    bool supported                  (void) const final;
    void start                      (bool flyView) final;
    void save                       (QJsonObject& json) final;
    bool load                       (const QJsonObject& json, QString& errorString) final;
    void loadFromVehicle            (void) final;
    void sendToVehicle              (void) final;
    void removeAll                  (void) final;
    void removeAllFromVehicle       (void) final;
    bool syncInProgress             (void) const final;
    bool dirty                      (void) const final;
    void setDirty                   (bool dirty) final;
    bool containsItems              (void) const final;
    void managerVehicleChanged      (Vehicle* managerVehicle) final;
    bool showPlanFromManagerVehicle (void) final;

    QmlObjectListModel* polygons                (void) { return &_polygons; }
    QmlObjectListModel* circles                 (void) { return &_circles; }
    // GDP - Start
    QmlObjectListModel* polygonsInfo            (void) { return &_polygonsInfo; }
    //GDP - Stop
    QGeoCoordinate      breachReturnPoint       (void) const { return _breachReturnPoint; }

    void setBreachReturnPoint(const QGeoCoordinate& breachReturnPoint);

signals:
    void breachReturnPointChanged       (QGeoCoordinate breachReturnPoint);
    void editorQmlChanged               (QString editorQml);
    void loadComplete                   (void);
    void paramCircularFenceChanged      (void);

private slots:
    void _polygonDirtyChanged       (bool dirty);
    void _setDirty                  (void);
    void _setFenceFromManager       (const QList<QGCFencePolygon>& polygons, const QList<QGCFenceCircle>&  circles);
    void _setReturnPointFromManager (QGeoCoordinate breachReturnPoint);
    void _managerLoadComplete       (void);
    void _updateContainsItems       (void);
    void _managerSendComplete       (bool error);
    void _managerRemoveAllComplete  (bool error);
    void _parametersReady           (void);

private:
    void _init(void);
    void _signalAll(void);

    GeoFenceManager*    _geoFenceManager;
    bool                _dirty;
    QmlObjectListModel  _polygons;
    QmlObjectListModel  _circles;
    // GDP - Start
    QmlObjectListModel  _polygonsInfo;
    //GDP - Stop
    QGeoCoordinate      _breachReturnPoint;
    bool                _itemsRequested;
    Fact*               _px4ParamCircularFenceFact;

    static const char* _px4ParamCircularFence;

    static const int _jsonCurrentVersion = 2;

    static const char* _jsonFileTypeValue;
    static const char* _jsonBreachReturnKey;
    static const char* _jsonPolygonsKey;
    static const char* _jsonCirclesKey;
    // GDP - Start
    static const char*  _jsonPolygonsInfoKey;
    //GDP - Stop
};

#endif
