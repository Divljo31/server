#include "Server.h"


Server::Server(QObject *parent) : m_nNextBlockSize(0)
{

    m_server = new QTcpServer();

    connect(this, &Server::newMessage, this, &Server::gotNewMessage);
    connect(this, &QTcpServer::newConnection, this, &Server::smbConnectedToServer);
    connect(this, &Server::smbDisconnected, this, &Server::smbDisconnectedFromServer);
}

QList<QTcpSocket *> Server::getClients()
{

    return m_clients;
}

void Server::newConnection()
{

    QTcpSocket *clientSocket = m_server->nextPendingConnection();

    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::readClient);
    connect(clientSocket, &QTcpSocket::disconnected, this, &Server::gotDisconnection);

    m_clients << clientSocket;

    sendToClient(clientSocket, "Reply: connection established");
}


void Server::readClient()
{

    QTcpSocket *clientSocket = (QTcpSocket*)sender();
    QDataStream in(clientSocket);

    //in.setVersion(QDataStream::Qt_6_6);

    for (;;)
    {
        if (!m_nNextBlockSize)
        {
            if (clientSocket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_nNextBlockSize;
        }

        if (clientSocket->bytesAvailable() < m_nNextBlockSize) { break; }
        QString str;
        in >> str;

        emit gotNewMessage(str);

        m_nNextBlockSize = 0;

        if (sendToClient(clientSocket, QString("Reply: received [%1]").arg(str)) == -1)
        {
            qDebug() << "Some error occured";
        }
    }
}


void Server::gotDisconnection()
{

    m_clients.removeAt(m_clients.indexOf((QTcpSocket*)sender()));
    emit smbDisconnected();
}

qint64 Server::sendToClient(QTcpSocket *socket, const QString &str)
{

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    //out << quint16(0) << QTime::currentTime() << str;
    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    return socket->write(arrBlock);
}


QString Server::connectClicked()
{

    if (!this->m_server->listen(QHostAddress::Any, 6547)) // set this to written port in ui
    {
        return "Error! The port is taken by some other service";
    }
    else
    {
        connect(m_server, &QTcpServer::newConnection, this, &Server::newConnection);
        return "Server started, port is openned";
    }
}

void Server::smbConnectedToServer()
{

    emit smbConnected();
}

void Server::smbDisconnectedFromServer()
{
    emit smbDisconnected();

}

void Server::gotNewMessage(QString msg)
{
    emit newMessage(msg);

}






