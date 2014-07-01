// (c) by Stefan Roettger, licensed under GPL 3.0

#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include "ssldatabase.h"

// ssl transmission database client class
class SSLTransmissionDatabaseClient: public QObject
{
   Q_OBJECT

public:

   SSLTransmissionDatabaseClient(QString hostName, quint16 port=10000,
                                 QString uid="", bool verify=true, bool compress=false,
                                 int maxThreads=10,
                                 QObject *parent = NULL);

   virtual ~SSLTransmissionDatabaseClient();

   // get host name
   QString getHostName();

   // get port
   quint16 getPort();

   // get user name
   QString getUID();

   // get receiver
   SSLTransmissionResponseReceiver *getReceiver();

   // start transmission
   bool transmit(SSLTransmission t);

   // start file transmission
   bool transmit(QString fileName);

public slots:

   // specify transmission host name
   void transmitHostName(QString hostName, quint16 port);

   // start non-blocking ping
   void pingNonBlocking();

   // start non-blocking transmission
   void transmitNonBlocking(SSLTransmission t);

   // start non-blocking file transmission
   void transmitNonBlocking(QString fileName);

protected:

   QString hostName_;
   quint16 port_;
   QString uid_;
   bool verify_;
   bool compress_;

   bool autoselect_;
   bool autoselecting_;

   SSLTransmissionResponseReceiver *receiver_;
   SSLTransmissionClient *client_;

   // auto-select user name
   bool autoselectUID();

protected slots:

   // ssl transmission pong
   void onPong(QString hostName, quint16 port, bool ack);

   // ssl transmission success
   void onSuccess(QString hostName, quint16 port, QString tid, QString uid);

   // ssl transmission failure
   void onFailure(QString hostName, quint16 port, QString tid, QString uid);

   // ssl transmission response
   void onResponse(SSLTransmission t);

   // ssl transmission result
   void onResult(SSLTransmission t);

signals:

   void pong(QString hostName, quint16 port, bool ack);
   void success(QString hostName, quint16 port, QString tid, QString uid);
   void failure(QString hostName, quint16 port, QString tid, QString uid);
   void response(SSLTransmission t);
   void result(SSLTransmission t);
   void error(QString e);
};

#endif
