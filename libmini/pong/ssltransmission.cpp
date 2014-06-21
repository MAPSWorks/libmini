// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

// ssl transmission server connection factory ctor
SSLTransmissionServerConnectionFactory::SSLTransmissionServerConnectionFactory(QObject *parent)
   : SSLServerConnectionFactory(parent)
{
   qRegisterMetaType<SSLTransmission>("SSLTransmission");
}

// ssl transmission server connection factory dtor
SSLTransmissionServerConnectionFactory::~SSLTransmissionServerConnectionFactory()
{}

// create a new transmission server connection
SSLServerConnection *SSLTransmissionServerConnectionFactory::create(int socketDescriptor,
                                                                    QString certPath, QString keyPath,
                                                                    QObject *parent)
{
   SSLTransmissionServerConnection *connection =
      new SSLTransmissionServerConnection(socketDescriptor, certPath, keyPath, this, parent);

   // signal transmission
   connect(connection, SIGNAL(transmit(SSLTransmission)),
           this, SLOT(receive(SSLTransmission)), Qt::QueuedConnection);

   return(connection);
}

// receiver of transmitted data blocks
void SSLTransmissionServerConnectionFactory::receive(SSLTransmission t)
{
   emit transmitted(t);
}

// ssl transmission server connection ctor
SSLTransmissionServerConnection::SSLTransmissionServerConnection(int socketDescriptor,
                                                                 QString certPath, QString keyPath,
                                                                 SSLServerConnectionFactory *factory,
                                                                 QObject *parent)
   : SSLServerConnection(socketDescriptor, certPath, keyPath, factory, parent)
{}

// ssl transmission server connection dtor
SSLTransmissionServerConnection::~SSLTransmissionServerConnection()
{}

// start reading from an established connection
bool SSLTransmissionServerConnection::startReading(QSslSocket *socket)
{
   // read from the ssl socket
   if (!t_.read(socket)) return(false);

   // signal transmission of data block
   if (!t_.empty())
      if (t_.getCommand() == SSLTransmission::command_transmit) emit transmit(t_);
      else emit command(t_);

   return(true);
}

// ssl transmission client ctor
SSLTransmissionClient::SSLTransmissionClient(QObject *parent)
   : SSLClient(parent)
{}

// ssl transmission client dtor
SSLTransmissionClient::~SSLTransmissionClient()
{}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify)
{
   t_ = t;

   return(SSLClient::transmit(hostName, port, verify));
}

// start transmission
bool SSLTransmissionClient::transmit(QString hostName, quint16 port, QString fileName, QString uid, bool verify, bool compress, int command)
{
   QFile file(fileName);

   if (!file.open(QIODevice::ReadOnly))
      return(false);

   SSLTransmission t(file, uid, command);

   if (compress)
      t.compress();

   return(transmit(hostName, port, t, verify));
}

// start writing through an established connection
void SSLTransmissionClient::startWriting(QSslSocket *socket)
{
   // write to the ssl socket
   t_.write(socket);
}

// start non-blocking transmission
void SSLTransmissionClient::transmitNonBlocking(QString hostName, quint16 port, QString fileName, QString uid, bool verify, bool compress, int command)
{
   SSLTransmissionThread::transmit(hostName, port, fileName, uid, verify, compress, command);
}

// ssl transmission thread ctor
SSLTransmissionThread::SSLTransmissionThread(QString hostName, quint16 port, QString fileName, QString uid, bool verify, bool compress, int command)
   : QThread(), hostName_(hostName), port_(port), fileName_(fileName), uid_(uid), verify_(verify), compress_(compress), command_(command)
{
   // self-termination after thread has finished
   connect(this, SIGNAL(finished()),
           this, SLOT(deleteLater()));
}

// ssl transmission thread dtor
SSLTransmissionThread::~SSLTransmissionThread()
{}

// thread run method
void SSLTransmissionThread::run()
{
   SSLTransmissionClient client;

   client.transmit(hostName_, port_, fileName_, uid_, verify_, compress_, command_);
}

// non-blocking transmission
void SSLTransmissionThread::transmit(QString hostName, quint16 port, QString fileName, QString uid, bool verify, bool compress, int command)
{
   SSLTransmissionThread *thread;

   thread = new SSLTransmissionThread(hostName, port, fileName, uid, verify, compress, command);
   thread->start();
}
