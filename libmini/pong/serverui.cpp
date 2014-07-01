// (c) by Stefan Roettger, licensed under GPL 3.0

#include "serverui.h"

ServerUI::ServerUI(SSLTransmissionDatabaseServer *server,
                   QWidget *parent)
   : QWidget(parent)
{
   // set main inherited style sheet
   QString css("QGroupBox { background-color: #eeeeee; border: 2px solid #999999; border-radius: 5px; margin: 3px; padding-top: 16px; }"
               "QGroupBox::title { subcontrol-origin: padding; subcontrol-position: top left; padding-left: 8px; padding-top: 3px; }");
   setStyleSheet(css);

   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);

   layout->addWidget(new QLabel("Pong Server"));

   QGroupBox *infoBox = new QGroupBox("Server status");
   QVBoxLayout *infoBoxLayout = new QVBoxLayout;
   layout->addWidget(infoBox);
   infoBox->setLayout(infoBoxLayout);

   counter_ = 0;
   counterLabel_ = new QLabel("Transmissions: none");
   infoBoxLayout->addWidget(counterLabel_);

   lastLabel_ = new QLabel;
   infoBoxLayout->addWidget(lastLabel_);

   nameLabel_ = new QLabel;
   infoBoxLayout->addWidget(nameLabel_);

   userLabel_ = new QLabel;
   infoBoxLayout->addWidget(userLabel_);

   timeLabel_ = new QLabel;
   infoBoxLayout->addWidget(timeLabel_);

   errorLabel_ = new QLabel;
   infoBoxLayout->addWidget(errorLabel_);

   QPushButton *quitButton = new QPushButton("Quit");
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   layout->addWidget(quitButton);

   // connect gui with the connection factory transmitted signal
   QObject::connect(server->getFactory(), SIGNAL(transmitted(SSLTransmission)),
                    this, SLOT(transmitted(SSLTransmission)));

   // connect gui with the connection factory report signal
   QObject::connect(server->getFactory(), SIGNAL(report(QString)),
                    this, SLOT(report(QString)));
}

ServerUI::~ServerUI()
{}

void ServerUI::transmitted(SSLTransmission t)
{
   counter_++;
   counterLabel_->setText("Transmissions: "+QString::number(counter_));

   QString transmission;

   if (t.getSize() < 1024)
      transmission = QString::number(t.getSize())+" bytes";
   else if (t.getSize() < 1024*1024)
      transmission = QString::number((double)t.getSize()/1024, 'g', 3)+" kbytes";
   else
      transmission = QString::number((double)t.getSize()/(1024*1024), 'g', 3)+" mbytes";

   lastLabel_->setText("Last request @ "+
                       QDateTime::currentDateTimeUtc().toString()+": "+transmission);

   nameLabel_->setText("File name: \""+t.getShortTID()+"\"");
   userLabel_->setText("User name: \""+t.getShortUID()+"\"");
   timeLabel_->setText("Time stamp: "+t.getTime().toString());

   errorLabel_->setText("ok");
}

void ServerUI::report(QString error)
{
   lastLabel_->setText("Last request @ "+
                       QDateTime::currentDateTimeUtc().toString());

   nameLabel_->setText("");
   userLabel_->setText("");
   timeLabel_->setText("");

   errorLabel_->setText(error);
}
