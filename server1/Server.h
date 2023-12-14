#ifndef SERVER_H
#define SERVER_H

#include "Client.h"
#include <QObject>

class Server : public QTcpServer
{
    Q_OBJECT

public:

    explicit Server(QObject *parent = nullptr);

    QList<QTcpSocket *> getClients();

    QTcpServer *m_server;
signals:

    void smbConnected();
    void smbDisconnected();
    void newMessage(QString msg);


public slots:

    virtual void newConnection();
    void readClient();
    void gotDisconnection();
    qint64 sendToClient(QTcpSocket *socket, const QString &str);

    QString connectClicked();
    void smbConnectedToServer();
    void smbDisconnectedFromServer();
    void gotNewMessage(QString msg);

private:

    quint16 m_nNextBlockSize;
    QList<QTcpSocket*> m_clients;

};


#endif // SERVER_H
