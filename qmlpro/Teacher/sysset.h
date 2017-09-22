#ifndef SYSSET_H
#define SYSSET_H

#include <QObject>

class Sysset : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString teacherip READ teacherip WRITE setTeacherip NOTIFY teacheripChanged)
    Q_PROPERTY(QString seatno READ seatno WRITE setSeatno NOTIFY seatnoChanged)
    Q_PROPERTY(int isdhcp READ isdhcp WRITE setIsdhcp NOTIFY isdhcpChanged)
    Q_PROPERTY(int isdhcp2 READ isdhcp2 WRITE setIsdhcp2 NOTIFY isdhcp2Changed)
    Q_PROPERTY(QString netip READ netip WRITE setNetip NOTIFY netipChanged)
    Q_PROPERTY(QString mask READ mask WRITE setMask NOTIFY maskChanged)
    Q_PROPERTY(QString gateway READ gateway WRITE setGateway NOTIFY gatewayChanged)
    //dns
    Q_PROPERTY(QString dns1 READ dns1 WRITE setDns1 NOTIFY dns1Changed)
    Q_PROPERTY(QString dns2 READ dns2 WRITE setDns2 NOTIFY dns2Changed)

public:
    Sysset();
    ~Sysset();
public:
    QString teacherip();
    void setTeacherip(QString str);
    QString seatno();
    void setSeatno(QString str);
    int isdhcp();
    void setIsdhcp(int value);
    int isdhcp2();
    void setIsdhcp2(int value);
    QString netip();
    void setNetip(QString str);
    QString mask();
    void setMask(QString str);
    QString gateway();
    void setGateway(QString str);
    QString  dns1();
    void setDns1(QString str);
    QString dns2();
    void setDns2(QString str);
private:
    QString m_teacherip;
    QString m_seatno;
    int m_isdhcp;
    int m_isdhcp2;
    QString m_netip;
    QString m_mask;
    QString m_gateway;
    QString m_dns1;
    QString m_dns2;
    int  m_netModify;
    int  m_dnsModify;
signals:
    void teacheripChanged();
    void seatnoChanged();
    void isdhcpChanged();
    void isdhcp2Changed();
    void netipChanged();
    void maskChanged();
    void gatewayChanged();
    void dns1Changed();
    void dns2Changed();
    void saveResult(int result);
public slots:
    void slot_save_acq();
};

#endif // SYSSET_H
