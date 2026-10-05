#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QVariantList>
#include <QGeoCoordinate>
#include <QQmlEngine>

class TrafficFetcher : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY (double currentSpeed READ currentSpeed NOTIFY dataChanged)
    Q_PROPERTY (double freeFlowSpeed READ freeFlowSpeed NOTIFY dataChanged)
    Q_PROPERTY (QVariantList coordinates READ coordinates NOTIFY dataChanged)

public:
    explicit TrafficFetcher(QObject *parent = nullptr);

    Q_INVOKABLE void fetchSegment(double lat, double lon);

    double currentSpeed() const { return m_currentSpeed; }
    double freeFlowSpeed() const { return m_freeFlowSpeed; }
    QVariantList coordinates() const { return m_coordinates; }

signals:
    void dataChanged();

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *m_manager;
    QString m_apiKey = "0fqvro4MWKyL9VdNP8k1Gxj9uzp29Obb"; // vorerst hartcodiert, dazu gleich mehr

    double m_currentSpeed = 0;
    double m_freeFlowSpeed = 0;
    QVariantList m_coordinates;
};