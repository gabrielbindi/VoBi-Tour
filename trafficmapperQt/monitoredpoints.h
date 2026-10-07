#pragma once
#include <QList>
#include <QGeoCoordinate>

struct MonitoredPoint {
    QString name;
    QGeoCoordinate coordinate;
};

inline const QList<MonitoredPoint> monitoredPoints = {
    {"Neutorgasse",   QGeoCoordinate(47.0707, 15.4395)},
    {"Herrengasse",   QGeoCoordinate(47.0750, 15.4420)},
    {"Jakominiplatz", QGeoCoordinate(47.0650, 15.4395)},
    };