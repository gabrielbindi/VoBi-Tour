#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "trafficmodel.h"
#include <QElapsedTimer>
#include <QDebug>
#include <Qtimer>

void BenchmarkFetchPoint() {

    QElapsedTimer timer;
    timer.start();

    void fetchPoint();

    qDebug() << "Dauer:"
             << timer.nsecsElapsed() / 1e6
             << "ms";
}

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("trafficmapperQt", "Main");

    BenchmarkFetchPoint();

    return QGuiApplication::exec();
}
