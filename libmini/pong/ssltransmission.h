// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLTRANSMISSION_H
#define SSLTRANSMISSION_H

#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <QDateTime>

#include "sslsocket.h"

// ssl transmission header structure
struct SSLTransmissionHeader
{
   qint64 size;
   qint8 compressed;
   qint64 time;
};

// ssl transmission class
class SSLTransmission
{
public:

   SSLTransmission(const QDateTime time = QDateTime::currentDateTimeUtc())
      : data_(), transmitState_(0)
   {
      header_.size = 0;
      header_.compressed = false;
      header_.time = time.toUTC().toMSecsSinceEpoch();
   }

   SSLTransmission(const QByteArray &data, const QDateTime time = QDateTime::currentDateTimeUtc())
      : data_(data), transmitState_(0)
   {
      header_.size = data_.size();
      header_.compressed = false;
      header_.time = time.toUTC().toMSecsSinceEpoch();
   }

   SSLTransmission(QFile &file)
      : data_(file.readAll()), transmitState_(0)
   {
      QFileInfo fileInfo(file);

      header_.size = data_.size();
      header_.compressed = false;
      header_.time = fileInfo.lastModified().toUTC().toMSecsSinceEpoch();
   }

   ~SSLTransmission()
   {}

   QByteArray getData()
   {
      uncompress();
      return(data_);
   }

   qint64 getSize()
   {
      return(header_.size);
   }

   QDateTime getTime()
   {
      QDateTime t;

      t.setMSecsSinceEpoch(header_.time);

      return(t.toUTC());
   }

   void append(const QByteArray &data)
   {
      if (!header_.compressed)
      {
         data_.append(data);
         header_.size = data_.size();
      }
   }

   void compress()
   {
      if (!header_.compressed)
      {
         data_ = qCompress(data_, 3); // favor speed over compression ratio
         header_.size = data_.size();
         header_.compressed = true;
      }
   }

   void uncompress()
   {
      if (header_.compressed)
      {
         data_ = qUncompress(data_);
         header_.size = data_.size();
         header_.compressed = false;
      }
   }

   void write(QSslSocket *socket)
   {
      if (transmitState_ == 0)
      {
         QByteArray block;
         QDataStream out(&block, QIODevice::WriteOnly);

         // assemble header block
         out.setVersion(QDataStream::Qt_4_0);
         out << header_.size;
         out << header_.compressed;
         out << header_.time;

         // write header block to the ssl socket
         socket->write(block);

         // write data block to the ssl socket
         socket->write(data_);

         // clear data block
         data_.clear();

         transmitState_++;
      }
   }

   bool read(QSslSocket *socket)
   {
      QDataStream in(socket);
      in.setVersion(QDataStream::Qt_4_0);

      if (transmitState_ == 0)
      {
         // check if entire header block has arrived
         if (socket->bytesAvailable() < (int)sizeof(header_)) return(false);

         // read data block size etc.
         in >> header_.size;
         in >> header_.compressed;
         in >> header_.time;

         transmitState_++;
      }
      else if (transmitState_ == 1)
      {
         // read data block from the ssl socket
         data_.append(socket->read(header_.size-data_.size()));

         // check if entire data block has arrived
         if (data_.size() < header_.size) return(false);

         transmitState_++;
      }

      return(true);
   }

protected:

   struct SSLTransmissionHeader header_;
   QByteArray data_;

   int transmitState_;
};

// ssl transmission server connection factory class
class SSLTransmissionServerConnectionFactory: public SSLServerConnectionFactory
{
   Q_OBJECT

public:

   SSLTransmissionServerConnectionFactory(QObject *parent = NULL);
   virtual ~SSLTransmissionServerConnectionFactory();

   // create a new transmission server connection
   virtual SSLServerConnection *create(int socketDescriptor,
                                       QString certPath, QString keyPath,
                                       QObject *parent);

protected:

   // consumer of transmitted data blocks
   virtual void consume(SSLTransmission &t);

public slots:

   // receiver of transmitted data blocks
   void receive(SSLTransmission);

signals:

   // signal transmission
   void transmitted(SSLTransmission);
};

// ssl transmission server connection class
class SSLTransmissionServerConnection: public SSLServerConnection
{
   Q_OBJECT

public:

   SSLTransmissionServerConnection(int socketDescriptor,
                                   QString certPath, QString keyPath,
                                   SSLServerConnectionFactory *factory,
                                   QObject *parent = NULL);

   virtual ~SSLTransmissionServerConnection();

protected:

   // start reading from an established connection
   virtual bool startReading(QSslSocket *socket);

   SSLTransmission t_;

signals:

   // signal transmission of data block
   void transmit(SSLTransmission);
};

// ssl transmission client class
class SSLTransmissionClient: public SSLClient
{
   Q_OBJECT

public:

   SSLTransmissionClient(QObject *parent = NULL);
   virtual ~SSLTransmissionClient();

   // start transmission
   bool transmit(QString hostName, quint16 port, const SSLTransmission &t, bool verify=true);
   bool transmit(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);

protected:

   // start writing through an established connection
   virtual void startWriting(QSslSocket *socket);

   SSLTransmission t_;

public slots:

   // start non-blocking transmission
   void transmitNonBlocking(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);
};

// ssl transmission thread class
class SSLTransmissionThread: public QThread
{
   Q_OBJECT

public:

   SSLTransmissionThread(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);
   virtual ~SSLTransmissionThread();

   // non-blocking transmission
   static void transmit(QString hostName, quint16 port, QString fileName, bool verify=true, bool compress=false);

protected:

   virtual void run();

   QString hostName_;
   quint16 port_;
   QString fileName_;
   bool verify_;
   bool compress_;
};

#endif
