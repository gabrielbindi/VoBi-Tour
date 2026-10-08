#include "trafficmodel.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

TrafficModel::TrafficModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_manager(new QNetworkAccessManager(this))
{
    m_segments.resize(monitoredPoints.size());
    for (int i = 0; i < monitoredPoints.size(); ++i)
        m_segments[i].name = monitoredPoints[i].name;
}

int TrafficModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_segments.size();
}

QVariant TrafficModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_segments.size())
        return {};

    const auto &seg = m_segments[index.row()];
    switch (role) {
    case NameRole:       return seg.name;
    case PathRole:        return seg.path;
    case SpeedRatioRole:  return seg.speedRatio;
    }
    return {};
}

QHash<int, QByteArray> TrafficModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {SpeedRatioRole, "speedRatio"}
    };
}

void TrafficModel::refresh()
{
    for (int i = 0; i < monitoredPoints.size(); ++i)
        fetchPoint(i);
}

void TrafficModel::fetchPoint(int idx)
{
    const auto &point = monitoredPoints[idx];
    QUrl url("https://api.tomtom.com/traffic/services/4/flowSegmentData/absolute/10/json");
    QUrlQuery query;
    query.addQueryItem("point", QString("%1,%2").arg(point.coordinate.latitude()).arg(point.coordinate.longitude()));
    query.addQueryItem("key", m_apiKey);
    url.setQuery(query);

    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, [this, reply, idx]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            qWarning() << "Network error for" << monitoredPoints[idx].name << ":" << reply->errorString();
            return;
        }

        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject flowData = doc.object()["flowSegmentData"].toObject();

        double currentSpeed  = flowData["currentSpeed"].toDouble();
        double freeFlowSpeed = flowData["freeFlowSpeed"].toDouble();

        QVariantList path;
        const QJsonArray coordArray = flowData["coordinates"].toObject()["coordinate"].toArray();
        for (const QJsonValue &v : coordArray) {
            QJsonObject p = v.toObject();
            path.append(QVariant::fromValue(
                QGeoCoordinate(p["latitude"].toDouble(), p["longitude"].toDouble())));
        }

        m_segments[idx].path = path;
        m_segments[idx].speedRatio = freeFlowSpeed > 0 ? currentSpeed / freeFlowSpeed : 1.0;

        QModelIndex changedIndex = index(idx);
        emit dataChanged(changedIndex, changedIndex);
    });
}