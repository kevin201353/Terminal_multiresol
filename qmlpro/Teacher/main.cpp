#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QColor>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSize>
#include <QScreen>
#include "sysset.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    //QQmlApplicationEngine engine;
    //engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    //注册组件到QML
    qmlRegisterType<Sysset>("Sysset", 2, 0, "Sysset");

    QQuickView viwer;
    //无边框，背景透明
    viwer.setFlags(Qt::FramelessWindowHint);
    viwer.setColor(QColor(Qt::transparent));
    //加载qml，qml添加到资源文件中可避免qml暴露
    viwer.setSource(QUrl("qrc:/qml/main.qml"));
    //viwer.setSource(QUrl("qrc:/qml/netset.qml"));
    QScreen *screen = app.primaryScreen();
    QSize size = screen->size();
    viwer.setPosition(size.width()/2 - viwer.width()/2, size.height()/2 - viwer.height()/2);
    viwer.show();
    //将viewer设置为main.qml属性
    viwer.rootContext()->setContextProperty("mainwindow",&viwer);
    return app.exec();
}
