#include "ServerWorker.h"

ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred), this, &ServerWorker::error);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

QString ServerWorker::userName() const
{
    return m_userName;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userName = userName;
}

void ServerWorker::receiveJson()
{

    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);

    socketStream.setVersion(QDataStream::Qt_5_7);

    for (;;) {

        socketStream.startTransaction();

        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {

            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    emit jsonReceived(jsonDoc.object());
                else
                    emit logMessage("Invalid message: " + QString::fromUtf8(jsonData));
            } else {
                emit logMessage("Invalid message: " + QString::fromUtf8(jsonData));
            }
        } else {
            break;
        }
    }
}

QTcpSocket *ServerWorker::serverSocket() const
{
    return m_serverSocket;
}

void ServerWorker::setServerSocket(QTcpSocket *newServerSocket)
{
    m_serverSocket = newServerSocket;
}

void ServerWorker::sendJson(const QJsonObject &json)
{

    const QByteArray jsonData = QJsonDocument(json).toJson(QJsonDocument::Compact);

    emit logMessage("Sending to " + userName() + " - " + QString::fromUtf8(jsonData));

    QDataStream socketStream(m_serverSocket);
    socketStream << jsonData;
}