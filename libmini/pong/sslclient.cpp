// (c) by Stefan Roettger, licensed under GPL 3.0

#include <QSettings>

#include "sslclient.h"

// ssl transmission database client ctor
SSLTransmissionDatabaseClient::SSLTransmissionDatabaseClient(QString hostName, quint16 port,
                                                             QString uid, bool verify, bool compress,
                                                             QObject *parent)
   : QObject(parent),
     hostName_(hostName), port_(port),
     uid_(uid), verify_(verify), compress_(compress),
     autoselect_(true), autoselecting_(false),
     receiver_(NULL),
     client_(NULL)
{
   if (hostName_ != "" && uid_ != "")
      autoselect_ = false;

   receiver_ = new SSLTransmissionResponseReceiver(parent);
   client_ = new SSLTransmissionClient(receiver_, parent);

   // signal ssl transmission pong
   connect(receiver_, SIGNAL(onPong(QString, quint16, bool)),
           this, SLOT(onPong(QString, quint16, bool)));

   // signal ssl transmission success
   connect(receiver_, SIGNAL(onSuccess(QString, quint16, QString, QString)),
           this, SLOT(onSuccess(QString, quint16, QString, QString)));

   // signal ssl transmission failure
   connect(receiver_, SIGNAL(onFailure(QString, quint16, QString, QString)),
           this, SLOT(onFailure(QString, quint16, QString, QString)));

   // signal ssl transmission response
   connect(receiver_, SIGNAL(onResponse(SSLTransmission)),
           this, SLOT(onResponse(SSLTransmission)));

   // signal ssl transmission result
   connect(receiver_, SIGNAL(onResult(SSLTransmission)),
           this, SLOT(onResult(SSLTransmission)));

   // start auto-selection
   autoselectUID();
}

// ssl transmission database client dtor
SSLTransmissionDatabaseClient::~SSLTransmissionDatabaseClient()
{
   if (client_)
      delete client_;

   if (receiver_)
      delete receiver_;
}

// get host name
QString SSLTransmissionDatabaseClient::getHostName()
{
   QString hostName = hostName_;

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (autoselect_)
      if (settings.contains("hostName"))
         hostName = settings.value("hostName").toString();

   return(hostName);
}

// get port
quint16 SSLTransmissionDatabaseClient::getPort()
{
   quint16 port = port_;

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (autoselect_)
      if (settings.contains("port"))
         port = settings.value("port").toInt();

   return(port);
}

// get user name
QString SSLTransmissionDatabaseClient::getUID()
{
   if (!autoselectUID())
      return("");

   return(uid_);
}

// auto-select user name
bool SSLTransmissionDatabaseClient::autoselectUID()
{
   if (!autoselect_)
      return(true);

   if (autoselecting_)
      return(false);

   QString hostName = hostName_;
   quint16 port = port_;
   QString uid = uid_;

   QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

   if (settings.contains("hostName") &&
       settings.contains("port") &&
       settings.contains("uid"))
   {
      hostName = settings.value("hostName").toString();
      port = settings.value("port").toInt();
      uid = settings.value("uid").toString();
   }

   if ((hostName_ == "") ||
       (hostName_ != "" && hostName_ == hostName && port_ == port))
   {
      hostName_ = hostName;
      port_ = port;
      uid_ = uid;
   }

   if (hostName_ == "" || uid_ == "")
   {
      if (hostName_ == "")
         hostName_ = "localhost";

      settings.setValue("hostName", hostName_);
      settings.setValue("port", port_);
      settings.setValue("uid", "");

      SSLTransmission t(QByteArray("create_uid"), "", "",
                        QDateTime::currentDateTimeUtc(),
                        false, SSLTransmission::cc_command);

      client_->transmitNonBlocking(hostName_, port_, t, verify_);
      autoselecting_ = true;

      return(false);
   }

   return(true);
}

// get receiver
SSLTransmissionResponseReceiver *SSLTransmissionDatabaseClient::getReceiver()
{
   return(receiver_);
}

// start transmission
bool SSLTransmissionDatabaseClient::transmit(SSLTransmission t)
{
   QString uid = getUID();

   if (uid != "")
   {
      t.setUID(uid);
      return(client_->transmit(hostName_, port_, t, verify_));
   }

   return(false);
}

// start file transmission
bool SSLTransmissionDatabaseClient::transmit(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
      return(client_->transmit(hostName_, port_,
                               fileName, uid,
                               verify_, compress_));

   return(false);
}

// specify transmission host name
void SSLTransmissionDatabaseClient::transmitHostName(QString hostName, quint16 port)
{
   if (!autoselect_)
      return;

   hostName_ = hostName;
   port_ = port;
   uid_ = "";

   if (!autoselectUID())
      emit error("failed to contact host");
}

// start non-blocking ping
void SSLTransmissionDatabaseClient::pingNonBlocking()
{
   client_->pingNonBlocking(hostName_, port_, verify_);
}

// start non-blocking transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(SSLTransmission t)
{
   QString uid = getUID();

   if (uid != "")
   {
      t.setUID(uid);
      client_->transmitNonBlocking(hostName_, port_, t, verify_);
   }
   else
      emit error("failed to register with host");
}

// start non-blocking file transmission
void SSLTransmissionDatabaseClient::transmitNonBlocking(QString fileName)
{
   QString uid = getUID();

   if (uid != "")
   {
      QFileInfo fileInfo(fileName);

      if (fileInfo.isReadable())
         client_->transmitNonBlocking(hostName_, port_,
                                      fileName, uid,
                                      verify_, compress_);
      else
         emit error("unable to read file");
   }
   else
      emit error("failed to register with host");
}

// ssl transmission pong
void SSLTransmissionDatabaseClient::onPong(QString hostName, quint16 port, bool ack)
{
   emit pong(hostName, port, ack);
}

// ssl transmission success
void SSLTransmissionDatabaseClient::onSuccess(QString hostName, quint16 port, QString tid, QString uid)
{
   emit success(hostName, port, tid, uid);
}

// ssl transmission failure
void SSLTransmissionDatabaseClient::onFailure(QString hostName, quint16 port, QString tid, QString uid)
{
   emit failure(hostName, port, tid, uid);
}

// ssl transmission response
void SSLTransmissionDatabaseClient::onResponse(SSLTransmission t)
{
   emit response(t);
}

// ssl transmission result
void SSLTransmissionDatabaseClient::onResult(SSLTransmission t)
{
   if (t.getTID() == "create_uid")
   {
      uid_ = t.getData();

      QSettings settings("www.open-terrain.org", "SSLTransmissionDatabaseClient");

      settings.setValue("uid", uid_);

      autoselecting_ = false;
   }
   else
      emit result(t);
}
