#ifndef MYHTTP_H
#define MYHTTP_H

#include <QString>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include "log.h"
#include <QTextCodec>
#include <QObject>
#include <QEventLoop>
#include <QTimer>
#define TIMEOUT 15000
class myHttp:public QObject
{
    Q_OBJECT
public:
    myHttp();
    ~myHttp();
    void SetUrlIP(QString IP)
    {
        m_strIP.clear();
        m_strIP = "http://";
        m_strIP += IP;
    }
    void Post(QString url, const char *append);
    void GetData(char *Buf);
    void Get(QString url);
private:
    QNetworkAccessManager   *m_pNetManager;
    QString m_strIP;
private:
    QString m_JsonMessage;
    QEventLoop *m_peventLoop;
    QNetworkReply *m_preply;
    pthread_mutex_t  m_Locak;
    QTimer      m_Qtimer;
};

#endif // MYHTTP_H
