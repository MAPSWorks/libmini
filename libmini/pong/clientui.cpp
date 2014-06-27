// (c) by Stefan Roettger, licensed under GPL 3.0

#include "ssltransmission.h"

#include "clientui.h"

ClientUI::ClientUI(QString hostName,
                   QWidget *parent)
   : QWidget(parent), hostName_(hostName)
{
   // set main inherited style sheet
   QString css("QGroupBox { background-color: #eeeeee; border: 2px solid #999999; border-radius: 5px; margin: 3px; padding-top: 16px; }"
               "QGroupBox::title { subcontrol-origin: padding; subcontrol-position: top left; padding-left: 8px; padding-top: 3px; }");
   setStyleSheet(css);

   QVBoxLayout *layout = new QVBoxLayout;
   setLayout(layout);

   layout->addWidget(new QLabel("Ping Client"));

   QGroupBox *dropBox = new QGroupBox("To transmit files");
   QVBoxLayout *dropBoxLayout = new QVBoxLayout;
   layout->addWidget(dropBox);
   dropBox->setLayout(dropBoxLayout);
   dropBoxLayout->addWidget(new QLabel("drag and drop here"));

   QGroupBox *lineEditGroup_hostName = createEdit("Transmit to host", hostName_, &lineEdit_hostName);
   connect(lineEdit_hostName, SIGNAL(textChanged(QString)), this, SLOT(hostNameChanged(QString)));

   layout->addWidget(lineEditGroup_hostName);

   QPushButton *quitButton = new QPushButton("Quit");
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));
   layout->addWidget(quitButton);

   // accept drag and drop
   setAcceptDrops(true);
}

ClientUI::~ClientUI()
{}

QGroupBox *ClientUI::createEdit(QString name, QString value,
                                QLineEdit **lineEdit)
{
   QGroupBox *lineEditGroup = new QGroupBox(name);
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value);
   lineEditLayout->addWidget(*lineEdit);
   return(lineEditGroup);
}

void ClientUI::hostNameChanged(QString hostName)
{
   emit host(hostName);
}

QString ClientUI::normalizeFile(QString file)
{
   file.remove("file://");
   file.replace('\\', '/');
   if (file.contains(QRegExp("^/[A-Z]:"))) file.remove(0,1);

   return(file);
}

void ClientUI::dragEnterEvent(QDragEnterEvent *event)
{
   event->acceptProposedAction();
}

void ClientUI::dragMoveEvent(QDragMoveEvent *event)
{
   event->acceptProposedAction();
}

void ClientUI::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls())
   {
      event->acceptProposedAction();

      QList<QUrl> urlList = mimeData->urls();

      for (int i=0; i<urlList.size(); i++)
      {
         QUrl qurl = urlList.at(i);
         QString fileName = normalizeFile(qurl.toString());

         emit transmit(fileName);
      }
   }
}

void ClientUI::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}
