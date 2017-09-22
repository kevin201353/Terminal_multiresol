#ifndef MULTI_H
#define MULTI_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class Multi : public QObject
{
    Q_OBJECT
public:
    Multi();
    void uinit();
public:
    void init();
    QUdpSocket *revWBudp;
};

#endif // MULTI_H
