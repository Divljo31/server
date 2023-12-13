#include "Client.h"


Client::Client(QObject *parent, const QString hostAddress, int portNumber) : m_NextBlockSize(0){

    m_status = false;
    m_socket = new QTcpSocket();

    m_host = hostAddress;
    m_port = portNumber;

    m_timeoutTimer = new QTimer();
    m_timeoutTimer->setSingleShot(true);

    connect(this, &Client::hasReadSome, this, &Client::receivedSomething);
    connect(this, &Client::statusChanged, this, &Client::setStatus);

    connect(m_socket, &QTcpSocket::errorOccurred, this, &Client::gotError);

    connect(m_timeoutTimer, &QTimer::timeout, this, &Client::connectionTimeout);

    connect(m_socket, &QTcpSocket::disconnected, this, &Client::closeConnection);
}


bool Client::getStatus()
{

    return m_status;

}

void Client::closeConnection()
{

    m_timeoutTimer->stop();

    //qDebug() << tcpSocket->state();
    disconnect(m_socket, &QTcpSocket::connected, 0, 0);
    disconnect(m_socket, &QTcpSocket::readyRead, 0, 0);

    bool shouldEmit = false;
    switch (m_socket->state())
    {
    case 0:
        m_socket->disconnectFromHost();
        shouldEmit = true;
        break;
    case 2:
        m_socket->abort();
        shouldEmit = true;
        break;
    default:
        m_socket->abort();
    }

    if (shouldEmit)
    {
        m_status = false;
        emit statusChanged(m_status);
    }
}

void Client::connect2host()
{

    m_timeoutTimer->start(3000);

    m_socket->connectToHost(m_host, m_port);
    connect(m_socket, &QTcpSocket::connected, this, &Client::connected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);
}

void Client::setStatus(bool newStatus)
{
    if (newStatus)
    { emit statusChanged("CONNECTED"); }
    else
    { emit statusChanged("DISCONNECTED"); }
}

void Client::receivedSomething(QString msg)
{

    emit someMessage(msg);

}

void Client::gotError(QAbstractSocket::SocketError err)
{

    //qDebug() << "got error";
    QString strError = "unknown";
    switch (err)
    {
    case 0:
        strError = "Connection was refused";
        break;
    case 1:
        strError = "Remote host closed the connection";
        break;
    case 2:
        strError = "Host address was not found";
        break;
    case 5:
        strError = "Connection timed out";
        break;
    default:
        strError = "Unknown error";
    }

    emit someError(strError);
}

void Client::sendClicked(QString msg)
{

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    //out.setVersion(QDataStream::Qt_5_10);
    out << quint16(0) << msg;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    this->m_socket->write(arrBlock);
}

void Client::connectClicked()
{


    this->connect2host();

}

void Client::disconnectClicked()
{

    this->closeConnection();

}

void Client::readyRead()
{

    QDataStream in(m_socket);
    //in.setVersion(QDataStream::Qt_5_10);
    for (;;)
    {
        if (!m_NextBlockSize)
        {
            if (m_socket->bytesAvailable() < sizeof(quint16)) { break; }
            in >> m_NextBlockSize;
        }

        if (m_socket->bytesAvailable() < m_NextBlockSize) { break; }

        QString str; in >> str;

        if (str == "0")
        {
            str = "Connection closed";
            closeConnection();
        }

        emit hasReadSome(str);
        m_NextBlockSize = 0;
    }
}

void Client::connected()
{

    m_status = true;
    emit statusChanged(m_status);
}

void Client::connectionTimeout()
{

    //qDebug() << m_socket->state();
    if(m_socket->state() == QAbstractSocket::ConnectingState)
    {
        m_socket->abort();
        emit someError("timeout");
    }
}
