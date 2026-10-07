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
};
