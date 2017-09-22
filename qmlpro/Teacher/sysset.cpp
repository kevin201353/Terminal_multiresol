#include "sysset.h"
#include <QDebug>
#include "ryanConfig.h"
#include "netconfig.h"

#define  SH_CONFIG_FILE_PATH   "/usr/local/shencloud/Terminal.con"
Sysset::Sysset()
{
    m_teacherip = "";
    m_seatno = "";
    m_isdhcp = 1;
    m_isdhcp2 = 1;
    m_netip = "";
    m_mask = "";
    m_gateway = "";
    m_dns1 = "";
    m_dns2 = "";
    m_netModify = 0;
    m_dnsModify = 0;
}

Sysset::~Sysset()
{

}

QString Sysset::teacherip()
{
    return m_teacherip;
}
void Sysset::setTeacherip(QString str)
{
    if (m_teacherip != str)
    {
        m_teacherip = str;
        emit teacheripChanged();
    }
}
QString Sysset::seatno()
{
    return m_seatno;
}
void Sysset::setSeatno(QString str)
{
    if (m_seatno != str)
    {
        m_seatno = str;
        emit seatnoChanged();
    }
}
int Sysset::isdhcp()
{
    return m_isdhcp;
}
void Sysset::setIsdhcp(int value)
{
    if (m_isdhcp != value)
    {
        m_netModify |= 1;
        m_isdhcp = value;
        emit isdhcpChanged();
    }else
    {
        m_netModify &= ~1;
    }
}

int Sysset::isdhcp2()
{
    return m_isdhcp2;
}
void Sysset::setIsdhcp2(int value)
{
    if (m_isdhcp2 != value)
    {
        m_isdhcp2 = value;
        emit isdhcp2Changed();
    }
}

QString Sysset::netip()
{
    return m_netip;
}

void Sysset::setNetip(QString str)
{
    if (m_netip != str)
    {
        m_netModify |= 2;
        m_netip = str;
        emit netipChanged();
    }else
    {
        m_netModify &= ~(1<<1);
    }
}
QString Sysset::mask()
{
    return m_mask;
}
void Sysset::setMask(QString str)
{
    if (m_mask != str)
    {
        m_netModify |= 3;
        m_mask = str;
        emit maskChanged();
    }else
    {
        m_netModify &= ~(1<<2);
    }
}
QString Sysset::gateway()
{
    return m_gateway;
}
void Sysset::setGateway(QString str)
{
    if (m_gateway != str)
    {
        m_netModify |= 4;
        m_gateway = str;
        emit gatewayChanged();
    }else
    {
        m_netModify &= ~(1<<3) ;
    }
}
QString  Sysset::dns1()
{
    return m_dns1;
}
void Sysset::setDns1(QString str)
{
    if (m_dns1 != str)
    {
        m_dns1 = str;
        emit dns1Changed();
    }
}
QString Sysset::dns2()
{
    return m_dns2;
}
void Sysset::setDns2(QString str)
{
    if (m_dns2 != str)
    {
        m_dns2 = str;
        emit dns2Changed();
    }
}

void Sysset::slot_save_acq()
{
    if (m_netModify == 0)
    {
        emit saveResult(0);
        return;
    }

    m_netModify = 0;
    if (m_netip == "" ||
        m_mask == ""  ||
        m_gateway == "")
    {
        qDebug() << "slot_save_acq  ip is null or mask is null or gateway is null or dns is null";
        emit saveResult(0);
        return;
    }
    qDebug() << "teacher ip : " << m_teacherip;
    qDebug() << "seat no : " << m_seatno;
    qDebug() << "m_isdhcp : " << m_isdhcp;
    qDebug() << "m_isdhcp2 : " << m_isdhcp2;
    qDebug() << "m_netip : " << m_netip;
    qDebug() << "m_mask : " << m_mask;
    qDebug() << "m_gateway : " << m_gateway;
    qDebug() << "m_dns1 : " << m_dns1;
    qDebug() << "m_dns2 : " << m_dns2;
    WriteConfigString(SH_CONFIG_FILE_PATH, "SERVICE", "ServiceIP", m_teacherip.toStdString().c_str());
    WriteConfigString(SH_CONFIG_FILE_PATH, "ROOM", "ServiceIP", m_teacherip.toStdString().c_str());
    WriteConfigString(SH_CONFIG_FILE_PATH, "ROOM", "SeatName", m_seatno.toStdString().c_str());

    NetConfig *pNetconfig = new NetConfig;
    memset(pNetconfig->m_pNetConfig, 0, sizeof(struct  NetConfigS));
    pNetconfig->m_pNetConfig->s_bIP = m_isdhcp;
    pNetconfig->m_pNetConfig->s_bDNS = m_isdhcp2;
    strcpy(pNetconfig->m_pNetConfig->s_strIP, m_netip.toStdString().c_str());
    strcpy(pNetconfig->m_pNetConfig->s_strIPMask, m_mask.toStdString().c_str());
    strcpy(pNetconfig->m_pNetConfig->s_strGateWay, m_gateway.toStdString().c_str());
    strcpy(pNetconfig->m_pNetConfig->s_strDNS[0], m_dns1.toStdString().c_str());
    strcpy(pNetconfig->m_pNetConfig->s_strDNS[1], m_dns2.toStdString().c_str());
    pNetconfig->WriteFile();
    if (NULL != pNetconfig)
    {
        delete pNetconfig;
        pNetconfig = NULL;
    }
//    system("sudo ifdown eth0");
//    system("sudo ifup eth0");
    emit saveResult(1);
}
