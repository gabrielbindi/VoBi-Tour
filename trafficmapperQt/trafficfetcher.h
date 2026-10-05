#pragma once
#include <QObject>
#include <QNetworkAccessManager>

class TrafficFetcher : public QObject
{
    Q_OBJECT
public:
    explicit TrafficFetcher(QObject *parent = nullptr);

    void fetchSegment(double lat, double lon);

private slots:
    void onReplyFinished();

private:
    QNetworkAccessManager *m_manager;
    QString m_apiKey = "DEIN_KEY_HIER"; // vorerst hartcodiert, dazu gleich mehr
};