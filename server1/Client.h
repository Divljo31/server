#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QTimer>
#include <QDataStream>


class Client : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool currentStatus READ getStatus NOTIFY statusChanged)


public:
    explicit Client(QObject *parent, const QString hostAddress, int portNumber);
    bool getStatus();


    QTcpSocket *m_socket;

public slots:
    void closeConnection();
    void connect2host();

    void setStatus(bool newStatus);
    void receivedSomething(QString msg);
    void gotError(QAbstractSocket::SocketError err);
    void sendClicked(QString msg);
    void connectClicked();
    void disconnectClicked();

private slots:
    void readyRead();
    void connected();
    void connectionTimeout();

signals:

    void statusChanged(bool status);
    void someError(QString err);
    void someMessage(QString msg);

    void hasReadSome(QString msg);

private:

    QString m_host;
    int m_port;
    bool m_status;
    quint16 m_NextBlockSize;
    QTimer *m_timeoutTimer;

};

#endif // CLIENT_H
