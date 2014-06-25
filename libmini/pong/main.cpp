// (c) by Stefan Roettger, licensed under GPL 3.0

#define VERSION "v0.7 as of 25.June.2014"

#define LICENSE "licensed under GPL 3.0"
#define COPYRIGHT "(c) by Stefan Roettger 2014"
#define DISCLAIMER "no warranty, no responsibility, no nothing is granted for anything!"

#ifdef HAVE_QT5
#include <QtWidgets/QApplication>
#else
#include <QtGui/QApplication>
#endif

#include "ssldatabase.h"

#include "serverui.h"
#include "clientui.h"

QString get_str(QString o)
{
   return(o.mid(o.indexOf("=")+1));
}

double get_opt(QString o)
{
   return(get_str(o).toDouble());
}

void usage(const char *prog)
{
   QString name(prog);
   std::string app(name.mid(name.lastIndexOf("/")+1).toStdString());
   std::cout << app << " " << VERSION << std::endl;
   std::cout << " " << LICENSE << std::endl;
   std::cout << " " << COPYRIGHT << std::endl;
   std::cout << " " << DISCLAIMER << std::endl;
   std::cout << "usage:" << std::endl;
   std::cout << " " << app << " {options} [ip-address [file]]" << std::endl;
   std::cout << "where options are:" << std::endl;
   std::cout << " --server: start pong server" << std::endl;
   std::cout << " --client: use drop box gui for transmissions" << std::endl;
   std::cout << " --transmit: transmit file" << std::endl;
   std::cout << " --dump: dump database contents" << std::endl;
   std::cout << " --port=n: use tcp port n" << std::endl;
   std::cout << " --user=\"name\": specify user name" << std::endl;
   std::cout << " --user-reset: reset auto-selected user name" << std::endl;
   std::cout << " --verify-peer: verify integrity of peer" << std::endl;
   std::cout << " --self-certified: allow self-certified certificates" << std::endl;
   std::cout << " --compress: compress files" << std::endl;
   std::cout << " --help: this help text" << std::endl;
   std::cout << "example server usage:" << std::endl;
   std::cout << " ./pong --server" << std::endl;
   std::cout << "example client usage:" << std::endl;
   std::cout << " ./pong --client 127.0.0.1" << std::endl;
   std::cout << "example transmission usage:" << std::endl;
   std::cout << " ./pong --transmit pong.server.org --compress file.txt" << std::endl;
   exit(1);
}

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);

   setlocale(LC_NUMERIC, "C");

   // get argument list
   QStringList args = QCoreApplication::arguments();

   // scan for arguments and options
   QStringList arg,opt;
   for (int i=1; i<args.size(); i++)
      if (args[i].startsWith("--")) opt.push_back(args[i].mid(2));
      else if (args[i].startsWith("-")) opt.push_back(args[i].mid(1));
      else arg.push_back(args[i]);

   bool server=true;
   bool client=false;
   bool transmit=false;
   bool dump=false;
   int port=10000;
   QString user="";
   bool reset=false;
   bool verify=false;
   bool compress=false;

#ifdef HAVE_SERVER
   server=true;
   client=false;
#endif

#ifdef HAVE_CLIENT
   server=false;
   client=true;
#endif

   // scan option list
   for (int i=0; i<opt.size(); i++)
      if (opt[i]=="server") {server=true; client=transmit=dump=false;}
      else if (opt[i]=="client") {client=true; server=transmit=dump=false;}
      else if (opt[i]=="transmit") {transmit=true; server=client=dump=false;}
      else if (opt[i]=="dump") {dump=true; server=client=transmit=false;}
      else if (opt[i].startsWith("port=")) port=(int)(get_opt(opt[i])+0.5);
      else if (opt[i].startsWith("user=")) user=get_str(opt[i]);
      else if (opt[i]=="user-reset") reset=true;
      else if (opt[i]=="verify-peer") verify=true;
      else if (opt[i]=="self-certified") verify=false;
      else if (opt[i]=="compress") compress=true;
      else if (opt[i]=="help") usage(argv[0]);
      else usage(argv[0]);

   // server mode
   if (server && arg.size()==0)
   {
      try
      {
         ServerUI main;
         SSLTransmissionDatabaseServer server(port, "cert.pem", "key.pem", "/usr/share/pingpong");

         // connect server gui with the connection factory transmitted signal
         QObject::connect(server.getFactory(), SIGNAL(transmitted(SSLTransmission)),
                          &main, SLOT(transmitted(SSLTransmission)));

         // connect server gui with the connection factory report signal
         QObject::connect(server.getFactory(), SIGNAL(report(QString)),
                          &main, SLOT(report(QString)));

         // start server on specified port (default 10000)
         server.start();

         main.show();

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // client mode
   else if (client && arg.size()<=1)
   {
      try
      {
         QString hostName = "localhost";
         if (arg.size()>0) hostName = arg[0];

         ClientUI main;
         SSLTransmissionDatabaseClient client(hostName, port, user, verify, compress);

         // auto-select user name
         if (user == "")
            if (!client.autoselectUID(reset))
               return(1);

         // connect client gui with client
         QObject::connect(&main, SIGNAL(transmit(QString)),
                          &client, SLOT(transmitNonBlocking(QString)));

         main.show();

         return(app.exec());
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // transmit mode
   else if (transmit && arg.size()==2)
   {
      try
      {
         SSLTransmissionDatabaseClient client(arg[0], port, user, verify, compress);

         // auto-select user name
         if (user == "")
            if (!client.autoselectUID(reset))
               return(1);

         // transmit file
         if (!client.transmit(arg[1]))
            return(1);
      }
      catch (SSLError &e)
      {
         std::cout << e.what() << std::endl;
         return(1);
      }
      catch (...)
      {
         return(1);
      }
   }
   // dump mode
   else if (dump && arg.size()==0)
   {
      SSLTransmissionDatabase::dump();
   }
   // print usage
   else usage(argv[0]);

   return(0);
}