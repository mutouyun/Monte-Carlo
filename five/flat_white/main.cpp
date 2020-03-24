#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "engine.h"

int main(int argc, char *argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine ui_engine;
    qmlRegisterSingletonType<Engine>("FlatWhite", 1, 0, "Engine", [](QQmlEngine*, QJSEngine*)->QObject* {
        return new Engine;
    });

    ui_engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (ui_engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
