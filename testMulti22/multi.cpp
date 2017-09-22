#include "multi.h"
#include <QtNetwork>

Multi::Multi()
{

}

void Multi::uinit()
{
    revWBudp->disconnected();
    delete revWBudp;
    revWBudp = nullptr;
}

void Multi::init()
{
    revWBudp = new QUdpSocket(this);
    groupAddress = QHostAddress("224.5.6.88");
    //绑定本地端口
    revWBudp->bind(QHostAddress::AnyIPv4, 11008, QUdpSocket::ShareAddress);
    //加入组播组
   bool result = revWBudp->joinMulticastGroup(groupAddress);
   if(result)
    {
        //设置缓冲区
        //revWBudp->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption,1024);
        revWBudp->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
        //连接接收信号槽
        connect(revWBudp,SIGNAL(readyRead()),this,SLOT(dataReceived()));
        //发送数据 目的地址：serverIP  目的IP:serverPort
           /*QByteArray line;
           line.resize(2);
           line[0] = 10;
           line[1] = 11;
           revWBudp->writeDatagram(line.data(),QHostAddress("224.5.6.88"),11008);
           */
    }
}

void Multi::dataReceived()
{
    while(revWBudp->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(revWBudp->pendingDatagramSize());
        revWBudp->readDatagram(datagram.data(), datagram.size());
        qDebug() << datagram.data();
    }
}
