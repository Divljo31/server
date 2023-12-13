ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QAbstractSocket>
#include <QJsonObject>
#include <QJsonParseError>

class ServerWorker : public QObject
{
    Q_OBJECT

public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString userName() const;
    void setUserName(const QString &userName);
    void sendJson(const QJsonObject &jsonData);
    QTcpSocket *serverSocket() const;
    void setServerSocket(QTcpSocket *newServerSocket);

signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);
public slots:
    void disconnectFromClient();
private slots:
    void receiveJson();
private:
    QTcpSocket *m_serverSocket;
    QString m_userName;
};

#endif // SERVERWORKER_H
