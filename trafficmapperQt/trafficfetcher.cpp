#include "trafficfetcher.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QUrlQuery>
#include <QDebug>

TrafficFetcher::TrafficFetcher(QObject *parent)
    : QObject(parent)
    , m_manager(new QNetworkAccessManager(this))
{
}

void TrafficFetcher::fetchSegment(double lat, double lon)
{
    QUrl url("https://api.tomtom.com/traffic/services/4/flowSegmentData/absolute/10/json");
    QUrlQuery query;
    query.addQueryItem("point", QString("%1,%2").arg(lat).arg(lon));
    query.addQueryItem("key", "0fqvro4MWKyL9VdNP8k1Gxj9uzp29Obb");
    url.setQuery(query);

    QNetworkReply *reply = m_manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &TrafficFetcher::onReplyFinished);
}

void TrafficFetcher::onReplyFinished()
{

    auto *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater(); // wichtig: Reply-Objekt muss manuell aufgeräumt werden

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Network error:" << reply->errorString();
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject flowData = doc.object()["flowSegmentData"].toObject();

    m_currentSpeed = flowData["currentSpeed"].toDouble();
    m_freeFlowSpeed  = flowData["freeFlowSpeed"].toDouble();

    m_coordinates.clear();

    const QJsonArray coordArray = flowData["coordinates"].toObject()["coordinate"].toArray();
    for (const QJsonValue &v : coordArray) {
        QJsonObject point = v.toObject();
        m_coordinates.append(QVariant::fromValue(
            QGeoCoordinate(point["latitude"].toDouble(), point["longitude"].toDouble())
            ));
    }

    emit dataChanged();
}