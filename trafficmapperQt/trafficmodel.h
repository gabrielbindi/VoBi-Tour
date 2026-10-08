#pragma once
#include <QAbstractListModel>
#include <QNetworkAccessManager>
#include <QQmlEngine>
#include "monitoredpoints.h"

class TrafficModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        SpeedRatioRole
    };

    explicit TrafficModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();

private:
    struct SegmentData {
        QString name;
        QVariantList path;
        double speedRatio = 1.0;
    };

    void fetchPoint(int idx);

    QNetworkAccessManager *m_manager;
    QString m_apiKey = "NXORXmTc9Ldb7ecq8bxcmw8iIwgI9iw0";
    QVector<SegmentData> m_segments;
};
