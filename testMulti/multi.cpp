#include "multi.h"
#include <QtNetwork>

Multi::Multi()
{

}

void Multi::uinit()
{
}

void Multi::init()
{
    revWBudp = new QUdpSocket(this);
    QByteArray line;
    line.resize(2);
    line[0] = 10;
    line[1] = 11;
    revWBudp->writeDatagram(line.data(),QHostAddress("224.5.6.88"),11008);
}

