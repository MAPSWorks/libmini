// (c) by Stefan Roettger, licensed under GPL 2+

#include <QtGui>

#include <grid/grid.h>

#include "viewerwindow.h"

#include "mainconst.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
   : QMainWindow(parent)
{
   initSettings();

   createActions();
   createMenus();
   createWidgets();

   setCentralWidget(mainGroup);
   setWindowTitle(tr(VIEWER_NAME));
}

MainWindow::~MainWindow()
{
   saveSettings();

   clear(true);

   delete viewerWindow;
}

void MainWindow::initSettings()
{
   // define default settings:

   ministring home_path = Object::normalize_path(QDir::homePath().toStdString().c_str());
   ministring temp_path = Object::normalize_path(QDir::tempPath().toStdString().c_str());

#ifdef __APPLE__
   home_path += "Desktop";
#endif

   repoPath = home_path;
   exportPath = home_path;
   tmpPath = temp_path;

   grid_level = 0;
   grid_levels = 1;
   grid_step = 2;

   shadePower = 2.0;
   shadeAmbient = 0.1;
   jpegQuality = 90;

   contourSpacing = 100.0;
   contourThickness = 2.0;

   // override with persistent settings:

   QSettings settings("www.open-terrain.org", "qtviewer");

   if (settings.contains("repoPath"))
      repoPath = settings.value("repoPath").toString().toStdString().c_str();

   if (settings.contains("exportPath"))
      exportPath = settings.value("exportPath").toString().toStdString().c_str();

   if (settings.contains("tmpPath"))
      tmpPath = settings.value("tmpPath").toString().toStdString().c_str();

   if (settings.contains("gridLevel"))
      grid_level = settings.value("gridLevel").toDouble();

   if (settings.contains("gridLevels"))
      grid_levels = settings.value("gridLevels").toDouble();

   if (settings.contains("gridStep"))
      grid_step = settings.value("gridStep").toDouble();

   if (settings.contains("shadePower"))
      shadePower = settings.value("shadePower").toDouble();

   if (settings.contains("shadeAmbient"))
      shadeAmbient = settings.value("shadeAmbient").toDouble();

   if (settings.contains("jpegQuality"))
      jpegQuality = settings.value("jpegQuality").toDouble();

   if (settings.contains("contourSpacing"))
      contourSpacing = settings.value("contourSpacing").toDouble();

   if (settings.contains("contourThickness"))
      contourThickness = settings.value("contourThickness").toDouble();
}

void MainWindow::saveSettings()
{
   QSettings settings("www.open-terrain.org", "qtviewer");

   settings.setValue("repoPath", QString(repoPath.c_str()));
   settings.setValue("exportPath", QString(exportPath.c_str()));
   settings.setValue("tmpPath", QString(tmpPath.c_str()));

   settings.setValue("gridLevel", grid_level);
   settings.setValue("gridLevels", grid_levels);
   settings.setValue("gridStep", grid_step);

   settings.setValue("shadePower", shadePower);
   settings.setValue("shadeAmbient", shadeAmbient);
   settings.setValue("jpegQuality", jpegQuality);

   settings.setValue("contourSpacing", contourSpacing);
   settings.setValue("contourThickness", contourThickness);
}

void MainWindow::createActions()
{
   aboutAction = new QAction(tr("&About"), this);
   aboutAction->setIcon(QIcon(":/images/about.png"));
   aboutAction->setShortcut(tr("Ctrl+A"));
   aboutAction->setStatusTip(tr("About this program"));
   connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

   openAction = new QAction(tr("O&pen"), this);
   openAction->setIcon(QIcon(":/images/open.png"));
   openAction->setShortcuts(QKeySequence::Open);
   openAction->setStatusTip(tr("Open location"));
   connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

   clearAction = new QAction(tr("C&lear"), this);
   clearAction->setIcon(QIcon(":/images/close.png"));
   clearAction->setShortcuts(QKeySequence::Close);
   clearAction->setStatusTip(tr("Clear locations"));
   connect(clearAction, SIGNAL(triggered()), this, SLOT(clear()));

   quitAction = new QAction(tr("Q&uit"), this);
   quitAction->setShortcuts(QKeySequence::Quit);
   quitAction->setStatusTip(tr("Quit the application"));
   connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}

void MainWindow::createMenus()
{
   fileMenu = menuBar()->addMenu(tr("&File"));
   fileMenu->addAction(openAction);
   fileMenu->addAction(clearAction);
   fileMenu->addAction(quitAction);

   menuBar()->addSeparator();

   helpMenu = menuBar()->addMenu(tr("&Help"));
   helpMenu->addAction(aboutAction);
}

void MainWindow::createWidgets()
{
   mainGroup = new QGroupBox;
   mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
   mainSplitter = new QSplitter;

   viewerWindow = new ViewerWindow;
   tabWidget = new QTabWidget;

   viewerGroup = new QGroupBox;
   viewerLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   prefGroup = new QGroupBox;
   prefLayout = new QVBoxLayout;

   buttonBox = new QDialogButtonBox;

   viewerTable = new MyQTableWidget(viewerWindow);

   tableBox = new QBoxLayout(QBoxLayout::LeftToRight);
   tableFilter = new QButtonGroup(viewerWindow);

   sliderBox = new QGroupBox;
   sliderLayout = new QBoxLayout(QBoxLayout::TopToBottom);

   // drag and drop:

   connect(viewerWindow, SIGNAL(changed(ministring)),
           this, SLOT(updateTable(ministring)));

   // layer table:

   QStringList labels;
   labels << tr("Object") << tr("Name");

   viewerTable->setColumnCount(2);
   viewerTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
   viewerTable->setSelectionMode(QAbstractItemView::SingleSelection);
   viewerTable->setHorizontalHeaderLabels(labels);
   viewerTable->horizontalHeader()->setStretchLastSection(true);
   viewerTable->setAcceptDrops(true);

   connect(viewerTable, SIGNAL(cellClicked(int, int)), this, SLOT(click(int, int)));
   connect(viewerTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(doubleclick(int, int)));

   // context menu:

   viewerTable->setContextMenuPolicy(Qt::CustomContextMenu);
   connect(viewerTable, SIGNAL(customContextMenuRequested(const QPoint&)),
           viewerTable, SLOT(showContextMenu(const QPoint&)));

   connect(viewerTable, SIGNAL(activate(ministring,int)),
           this, SLOT(runAction(ministring,int)));

   // layer table filter:

   QRadioButton *filter1 = new QRadioButton(tr("DEM"));
   QRadioButton *filter2 = new QRadioButton(tr("IMG"));
   QRadioButton *filter3 = new QRadioButton(tr("ALL"));
   filter3->setChecked(true);

   tableFilter->addButton(filter1);
   tableFilter->addButton(filter2);
   tableFilter->addButton(filter3);

   tableBox->addWidget(filter1);
   tableBox->addWidget(filter2);
   tableBox->addWidget(filter3);

   connect(filter1, SIGNAL(toggled(bool)), this, SLOT(filterDEMToggled(bool)));
   connect(filter2, SIGNAL(toggled(bool)), this, SLOT(filterIMGToggled(bool)));
   connect(filter3, SIGNAL(toggled(bool)), this, SLOT(filterALLToggled(bool)));

   // worker activity:

   workerActivity = new QLabel(this);
   workerActivity->setText("");

   // fog check:

   fogCheck = new QCheckBox(tr("Fog Density"));
   fogCheck->setChecked(true);

   connect(fogCheck, SIGNAL(stateChanged(int)), this, SLOT(checkFog(int)));

   fogDensitySlider = createSlider(0, 100, 0);

   connect(fogDensitySlider, SIGNAL(valueChanged(int)), this, SLOT(setFogDensity(int)));

   fogGroup = new QGroupBox;
   fogLayout = new QHBoxLayout;

   fogLayout->addWidget(fogCheck);
   fogLayout->addWidget(fogDensitySlider);
   fogGroup->setLayout(fogLayout);

   // contour check:

   contourCheck = new QCheckBox(tr("Contours"));
   contourCheck->setChecked(false);

   connect(contourCheck, SIGNAL(stateChanged(int)), this, SLOT(checkContours(int)));

   contourGroup = new QGroupBox;
   contourLayout = new QHBoxLayout;

   contourLayout->addWidget(contourCheck);
   contourGroup->setLayout(contourLayout);

   // sea level check:

   seaLevelCheck = new QCheckBox(tr("Ocean Sea Level"));
   seaLevelCheck->setChecked(false);

   connect(seaLevelCheck, SIGNAL(stateChanged(int)), this, SLOT(checkSeaLevel(int)));

   seaLevelSlider = createSlider(-100, 100, 0);

   connect(seaLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setSeaLevel(int)));

   seaGroup = new QGroupBox;
   seaLayout = new QHBoxLayout;

   seaLayout->addWidget(seaLevelCheck);
   seaLayout->addWidget(seaLevelSlider);
   seaGroup->setLayout(seaLayout);

   // light check:

   lightCheck = new QCheckBox(tr("UTC Night"));
   lightCheck->setChecked(false);

   connect(lightCheck, SIGNAL(stateChanged(int)), this, SLOT(checkLight(int)));

   lightSlider = createSlider(-12, 12, 0);

   connect(lightSlider, SIGNAL(valueChanged(int)), this, SLOT(setLight(int)));

   lightGroup = new QGroupBox;
   lightLayout = new QHBoxLayout;

   lightLayout->addWidget(lightCheck);
   lightLayout->addWidget(lightSlider);
   lightGroup->setLayout(lightLayout);

   // exaggeration check:

   exaggerCheck = new QCheckBox(tr("Exaggeration"));
   exaggerCheck->setChecked(false);

   connect(exaggerCheck, SIGNAL(stateChanged(int)), this, SLOT(checkExagger(int)));

   exaggerSlider = createSlider(0, 100, 100);

   connect(exaggerSlider, SIGNAL(valueChanged(int)), this, SLOT(setExagger(int)));

   exaggerGroup = new QGroupBox;
   exaggerLayout = new QHBoxLayout;

   exaggerLayout->addWidget(exaggerCheck);
   exaggerLayout->addWidget(exaggerSlider);
   exaggerGroup->setLayout(exaggerLayout);

   // stereo check:

   stereoCheck = new QCheckBox(tr("Stereo"));
   stereoCheck->setChecked(false);

   connect(stereoCheck, SIGNAL(stateChanged(int)), this, SLOT(toggleStereo(int)));

   stereoGroup = new QGroupBox;
   stereoLayout = new QHBoxLayout;

   stereoLayout->addWidget(stereoCheck);
   stereoGroup->setLayout(stereoLayout);

   // wire frame check:

   wireFrameCheck = new QCheckBox(tr("Wire Frame"));
   wireFrameCheck->setChecked(false);

   connect(wireFrameCheck, SIGNAL(stateChanged(int)), this, SLOT(toggleWireFrame(int)));

   QGroupBox *wireGroup = new QGroupBox;
   QHBoxLayout *wireLayout = new QHBoxLayout;

   wireLayout->addWidget(wireFrameCheck);
   wireGroup->setLayout(wireLayout);

   // slider group:

   QHBoxLayout *renderLayout = new QHBoxLayout;
   renderLayout->addWidget(stereoGroup);
   renderLayout->addWidget(wireGroup);

   sliderLayout->addWidget(fogGroup);
   sliderLayout->addWidget(contourGroup);
   sliderLayout->addWidget(seaGroup);
   sliderLayout->addWidget(lightGroup);
   sliderLayout->addWidget(exaggerGroup);
   sliderLayout->addLayout(renderLayout);
   sliderBox->setLayout(sliderLayout);

   // viewer group:

   viewerLayout->addLayout(tableBox);
   viewerLayout->addWidget(viewerTable);
   viewerLayout->addWidget(workerActivity);
   viewerLayout->addWidget(sliderBox);

   viewerGroup->setLayout(viewerLayout);
   viewerGroup->setFlat(true);

   // pref group:

   QGroupBox *lineEditGroup_repoPath = createPathEdit(TR("Repository Path"), repoPath,
                                                      &lineEdit_repoPath, &browseButton_repoPath);

   connect(lineEdit_repoPath,SIGNAL(textChanged(QString)),this,SLOT(repoPathChanged(QString)));
   connect(browseButton_repoPath, SIGNAL(pressed()), this, SLOT(browseRepoPath()));

   QGroupBox *lineEditGroup_exportPath = createPathEdit(TR("Export Path"), exportPath,
                                                        &lineEdit_exportPath, &browseButton_exportPath);

   connect(lineEdit_exportPath,SIGNAL(textChanged(QString)),this,SLOT(exportPathChanged(QString)));
   connect(browseButton_exportPath, SIGNAL(pressed()), this, SLOT(browseExportPath()));

   QGroupBox *lineEditGroup_tmpPath = createPathEdit(TR("Temporary Path"), tmpPath,
                                                     &lineEdit_tmpPath, &browseButton_tmpPath);

   connect(lineEdit_tmpPath,SIGNAL(textChanged(QString)),this,SLOT(tmpPathChanged(QString)));
   connect(browseButton_tmpPath, SIGNAL(pressed()), this, SLOT(browseTmpPath()));

   QGroupBox *lineEditGroup_gridLevel = createEdit(TR("Grid Level"), grid_level, &lineEdit_gridLevel);
   connect(lineEdit_gridLevel,SIGNAL(textChanged(QString)),this,SLOT(gridLevelChanged(QString)));

   QGroupBox *lineEditGroup_gridLevels = createEdit(TR("Grid Levels"), grid_levels, &lineEdit_gridLevels);
   connect(lineEdit_gridLevels,SIGNAL(textChanged(QString)),this,SLOT(gridLevelsChanged(QString)));

   QGroupBox *lineEditGroup_gridStep = createEdit(TR("Grid Step"), grid_step, &lineEdit_gridStep);
   connect(lineEdit_gridStep,SIGNAL(textChanged(QString)),this,SLOT(gridStepChanged(QString)));

   QGroupBox *lineEditGroup_shadePower = createEdit(TR("Shading Power"), shadePower, &lineEdit_shadePower);
   connect(lineEdit_shadePower,SIGNAL(textChanged(QString)),this,SLOT(shadePowerChanged(QString)));

   QGroupBox *lineEditGroup_shadeAmbient = createEdit(TR("Shading Ambience"), shadeAmbient, &lineEdit_shadeAmbient);
   connect(lineEdit_shadeAmbient,SIGNAL(textChanged(QString)),this,SLOT(shadeAmbientChanged(QString)));

   QGroupBox *lineEditGroup_jpegQuality = createEdit(TR("JPEG Quality"), jpegQuality, &lineEdit_jpegQuality);
   connect(lineEdit_jpegQuality,SIGNAL(textChanged(QString)),this,SLOT(jpegQualityChanged(QString)));

   QGroupBox *lineEditGroup_contourSpacing = createEdit(TR("Contour Spacing"), contourSpacing, &lineEdit_contourSpacing);
   connect(lineEdit_contourSpacing,SIGNAL(textChanged(QString)),this,SLOT(contourSpacingChanged(QString)));

   QGroupBox *lineEditGroup_contourThickness = createEdit(TR("Contour Thickness"), contourThickness, &lineEdit_contourThickness);
   connect(lineEdit_contourThickness,SIGNAL(textChanged(QString)),this,SLOT(contourThicknessChanged(QString)));

   sliderButton = new QCheckBox(tr("Show Controls"));
   sliderButton->setChecked(false);

   QGroupBox *sliderButtonBox= new QGroupBox;
   QHBoxLayout *sliderButtonBoxLayout= new QHBoxLayout;
   sliderButtonBoxLayout->addWidget(sliderButton);
   sliderButtonBox->setLayout(sliderButtonBoxLayout);
   sliderBox->setVisible(sliderButton->isChecked());

   connect(sliderButton, SIGNAL(stateChanged(int)), this, SLOT(checkSliders(int)));

   prefLayout->addWidget(lineEditGroup_repoPath);
   prefLayout->addWidget(lineEditGroup_exportPath);
   prefLayout->addWidget(lineEditGroup_tmpPath);

   prefLayout->addWidget(lineEditGroup_gridLevel);
   prefLayout->addWidget(lineEditGroup_gridLevels);
   prefLayout->addWidget(lineEditGroup_gridStep);

   prefLayout->addWidget(lineEditGroup_shadePower);
   prefLayout->addWidget(lineEditGroup_shadeAmbient);
   prefLayout->addWidget(lineEditGroup_jpegQuality);

   prefLayout->addWidget(lineEditGroup_contourSpacing);
   prefLayout->addWidget(lineEditGroup_contourThickness);

   prefLayout->addWidget(sliderButtonBox);

   prefLayout->addStretch();

   prefGroup->setLayout(prefLayout);

   // tabs:

   QScrollArea *viewerGroupScrollArea = new QScrollArea;
   viewerGroupScrollArea->setWidgetResizable(true);
   viewerGroupScrollArea->setWidget(viewerGroup);
   viewerGroupScrollArea->setMinimumWidth(viewerGroup->sizeHint().width());

   QScrollArea *prefGroupScrollArea = new QScrollArea;
   prefGroupScrollArea->setWidgetResizable(true);
   prefGroupScrollArea->setWidget(prefGroup);
   prefGroupScrollArea->setMinimumWidth(prefGroup->sizeHint().width());

   tabWidget->addTab(viewerGroupScrollArea, tr("View"));
   tabWidget->addTab(prefGroupScrollArea, tr("Prefs"));

   // button group:

   openButton = new QPushButton(tr("Open"));
   clearButton = new QPushButton(tr("Clear"));
   quitButton = new QPushButton(tr("Quit"));

   connect(openButton, SIGNAL(pressed()), this, SLOT(open()));
   connect(clearButton, SIGNAL(pressed()), this, SLOT(clear()));
   connect(quitButton, SIGNAL(pressed()), this, SLOT(close()));

   buttonBox->addButton(openButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(clearButton, QDialogButtonBox::ActionRole);
   buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

   // main group:

   mainLayout->addWidget(mainSplitter);
   mainLayout->addWidget(buttonBox);

   mainSplitter->addWidget(tabWidget);
   mainSplitter->addWidget(viewerWindow);

   mainGroup->setLayout(mainLayout);

   // worker directories:

   repoPathChanged(repoPath.c_str());
   exportPathChanged(exportPath.c_str());
   tmpPathChanged(tmpPath.c_str());

   // worker settings:

   viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
   viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
   viewerWindow->setDefaultSettings(contourSpacing, contourThickness);

   // progress:

   connect(viewerWindow, SIGNAL(progress(double, ministring)),
           this, SLOT(reportProgress(double, ministring)));
}

QSlider *MainWindow::createSlider(int minimum, int maximum, int value)
{
   QSlider *slider = new QSlider(Qt::Horizontal);
   slider->setRange(minimum * 16, maximum * 16);
   slider->setSingleStep(16);
   slider->setPageStep((maximum - minimum) / 10 * 16);
   slider->setTickInterval((maximum - minimum) / 10 * 16);
   slider->setTickPosition(QSlider::TicksBelow);
   slider->setValue(value * 16);
   return(slider);
}

QGroupBox *MainWindow::createEdit(ministring name, ministring value,
                                  QLineEdit **lineEdit)
{
   QGroupBox *lineEditGroup = new QGroupBox(tr(name.c_str()));
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value.c_str());
   lineEditLayout->addWidget(*lineEdit);
   return(lineEditGroup);
}

QGroupBox *MainWindow::createPathEdit(ministring name, ministring value,
                                      QLineEdit **lineEdit, QPushButton **browseButton)
{
   QGroupBox *lineEditGroup = new QGroupBox(tr(name.c_str()));
   QVBoxLayout *lineEditLayout = new QVBoxLayout;
   lineEditGroup->setLayout(lineEditLayout);
   *lineEdit = new QLineEdit(value.c_str());
   lineEditLayout->addWidget(*lineEdit);
   *browseButton = new QPushButton(tr("Browse"));
   lineEditLayout->addWidget(*browseButton);
   lineEditLayout->setAlignment(*browseButton, Qt::AlignLeft);
   return(lineEditGroup);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About this program"),
                      tr(VIEWER_NAME" "VIEWER_VERSION));
}

void MainWindow::open()
{
   runAction("open");
}

void MainWindow::clear(bool all)
{
   int row = viewerTable->currentRow();

   if (row==-1 || all)
      runAction("delete_all");
   else
      runAction("delete", row);
}

void MainWindow::getNameInfo(Object *obj,
                             QString &name, QString &info)
{
   ministring url=obj->get_full_name();
   name=url.c_str();

   if (name.endsWith("/")) name.truncate(name.size()-1);
   if (name.endsWith("\\")) name.truncate(name.size()-1);

   if (name.lastIndexOf("/")>=0) name.remove(0,name.lastIndexOf("/")+1);
   if (name.lastIndexOf("\\")>=0) name.remove(0,name.lastIndexOf("\\")+1);

   info="object";

   ministrings *tags=viewerWindow->getTags(url);

   if (tags)
   {
      ministring tag=tags->get(0);
      if (tags->has("elevation")) tag="elevation";
      if (tags->has("imagery")) tag="imagery";
      info=tag.c_str();

      if (tag=="extent")
      {
         name="extent";
         info="movable";
      }
   }
}

void MainWindow::updateTable(ministring key)
{
   unsigned int row;
   int rows = viewerTable->rowCount();

   Object *obj=viewerWindow->getObject(key);

   BOOLINT present=FALSE;

   for (row=0; row<m_Keys.getsize(); row++)
      if (m_Keys[row]==key)
      {
         present=TRUE;
         break;
      }

   // add object
   if (!present && obj!=NULL)
   {
      QString name, info;

      getNameInfo(obj, name, info);

      viewerTable->insertRow(rows);
      viewerTable->setItem(rows, 0, new QTableWidgetItem(info));
      viewerTable->setItem(rows, 1, new QTableWidgetItem(name));

      m_Keys.growsize(rows+1);
      m_Keys[rows]=key;
   }
   // update object
   else if (present && obj!=NULL)
   {
      QString name, info;

      getNameInfo(obj, name, info);

      viewerTable->setItem(row, 0, new QTableWidgetItem(info));
      viewerTable->setItem(row, 1, new QTableWidgetItem(name));

      if (viewerWindow->hasTag(m_Keys[row], "selected"))
         viewerTable->item(row, 0)->setBackground(QBrush(QColor("blue")));
      else
         viewerTable->item(row, 0)->setBackground(QBrush(QColor("white")));

      if (viewerWindow->hasTag(m_Keys[row], "hidden"))
      {
         viewerTable->item(row, 0)->setForeground(QBrush(QColor("grey")));
         viewerTable->item(row, 1)->setForeground(QBrush(QColor("grey")));
      }
      else
      {
         viewerTable->item(row, 0)->setForeground(QBrush(QColor("black")));
         viewerTable->item(row, 1)->setForeground(QBrush(QColor("black")));
      }
   }
   // remove object
   else if (present && obj==NULL)
   {
      viewerTable->removeRow(row);
      m_Keys.dispose(row);
   }
}

void MainWindow::runAction(ministring action, int row)
{
   ministring key;

   if (row>=0 && (unsigned int)row<m_Keys.size())
      key = m_Keys[row];

   viewerWindow->runAction(action, key);
}

void MainWindow::reportProgress(double percentage, const ministring &job)
{
   ministring progress;

   if (percentage<100)
   {
      if (job == "thumb")
         progress = "creating thumbnail";
      else
         if (percentage==0.0)
            progress = job+" progress: working";
         else
            progress = job+" progress: "+(int)percentage+"%";
   }

   workerActivity->setText(progress.c_str());
}

void MainWindow::click(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   if (keyMod & Qt::ShiftModifier)
      runAction("select", row);
   else if (keyMod & Qt::AltModifier)
      runAction("toggle", row);
   else
      viewerTable->setCurrentCell(row, 1);
}

void MainWindow::doubleclick(int row, int col)
{
   QTableWidgetItem *item = viewerTable->item(row, col);
   QString text = item->text();

   runAction("goto", row);
}

void MainWindow::toggleStereo(int on)
{
   viewerWindow->toggleStereo(on);
}

void MainWindow::toggleWireFrame(int on)
{
   viewerWindow->toggleWireFrame(on);
}

void MainWindow::checkFog(int on)
{
   viewerWindow->checkFog(on);
}

void MainWindow::setFogDensity(int tick)
{
   double density = tick / 16.0 / 100.0;
   viewerWindow->setFogDensity(density);
}

void MainWindow::checkContours(int on)
{
   viewerWindow->checkContours(on);
}

void MainWindow::checkSeaLevel(int on)
{
   viewerWindow->checkSeaLevel(on);
}

void MainWindow::setSeaLevel(int tick)
{
   double level = tick / 16.0;
   viewerWindow->setSeaLevel(level);
}

void MainWindow::checkLight(int on)
{
   viewerWindow->checkLight(on);
}

void MainWindow::setLight(int tick)
{
   double hour = tick / 16.0;
   viewerWindow->setLight(hour);
}

void MainWindow::checkExagger(int on)
{
   viewerWindow->checkExagger(on);
}

void MainWindow::setExagger(int tick)
{
   double scale = tick / 16.0 / 100.0;
   viewerWindow->setExagger(scale);
}

void MainWindow::filterDEMToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_elevation");
      viewerWindow->runAction("hide_imagery");
   }
}

void MainWindow::filterIMGToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_imagery");
      viewerWindow->runAction("hide_elevation");
   }
}

void MainWindow::filterALLToggled(bool on)
{
   if (on)
   {
      viewerWindow->runAction("show_elevation");
      viewerWindow->runAction("show_imagery");
   }
}

void MainWindow::repoPathChanged(QString repo)
{
   repoPath = repo.toStdString().c_str();
   viewerWindow->runAction("repo", repoPath);
}

void MainWindow::exportPathChanged(QString expo)
{
   exportPath = expo.toStdString().c_str();
   viewerWindow->runAction("export", exportPath);
}

void MainWindow::tmpPathChanged(QString tmp)
{
   tmpPath = tmp.toStdString().c_str();
   viewerWindow->runAction("tmp", tmpPath);
}

void MainWindow::browseRepoPath()
{
   ministring dir = viewerWindow->browseDir(TR("Browse Repository Path"), repoPath);

   if (dir!="")
   {
      lineEdit_repoPath->setText(dir.c_str());
      repoPathChanged(dir.c_str());
   }
}

void MainWindow::browseExportPath()
{
   ministring dir = viewerWindow->browseDir(TR("Browse Export Path"), exportPath);

   if (dir!="")
   {
      lineEdit_exportPath->setText(dir.c_str());
      exportPathChanged(dir.c_str());
   }
}

void MainWindow::browseTmpPath()
{
   ministring dir = viewerWindow->browseDir(TR("Browse Temporary Path"), tmpPath);

   if (dir!="")
   {
      lineEdit_tmpPath->setText(dir.c_str());
      tmpPathChanged(dir.c_str());
   }
}

void MainWindow::gridLevelChanged(QString level)
{
   bool valid;
   double grid_level = level.toDouble(&valid);

   if (valid)
      {
      this->grid_level = grid_level;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::gridLevelsChanged(QString levels)
{
   bool valid;
   double grid_levels = levels.toDouble(&valid);

   if (valid)
      {
      this->grid_levels = grid_levels;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::gridStepChanged(QString step)
{
   bool valid;
   double grid_step = step.toDouble(&valid);

   if (valid)
      {
      this->grid_step = grid_step;
      viewerWindow->setResampleSettings(grid_level, grid_levels, grid_step);
      }
}

void MainWindow::shadePowerChanged(QString power)
{
   bool valid;
   double shadePower = power.toDouble(&valid);

   if (valid)
      {
      this->shadePower = shadePower;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::shadeAmbientChanged(QString ambient)
{
   bool valid;
   double shadeAmbient = ambient.toDouble(&valid);

   if (valid)
      {
      this->shadeAmbient = shadeAmbient;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::jpegQualityChanged(QString quality)
{
   bool valid;
   double jpegQuality = quality.toDouble(&valid);

   if (valid)
      {
      this->jpegQuality = jpegQuality;
      viewerWindow->setExportSettings(shadePower, shadeAmbient, jpegQuality);
      }
}

void MainWindow::contourSpacingChanged(QString spacing)
{
   bool valid;
   double contourSpacing = spacing.toDouble(&valid);

   if (valid)
      {
      this->contourSpacing = contourSpacing;
      viewerWindow->setDefaultSettings(contourSpacing, contourThickness);
      }
}

void MainWindow::contourThicknessChanged(QString thickness)
{
   bool valid;
   double contourThickness = thickness.toDouble(&valid);

   if (valid)
      {
      this->contourThickness = contourThickness;
      viewerWindow->setDefaultSettings(contourSpacing, contourThickness);
      }
}

void MainWindow::checkSliders(int on)
{
   sliderBox->setVisible(on);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_F)
      fogCheck->setChecked(!fogCheck->isChecked());
   else if (event->key() == Qt::Key_C)
      contourCheck->setChecked(!contourCheck->isChecked());
   else if (event->key() == Qt::Key_O)
      seaLevelCheck->setChecked(!seaLevelCheck->isChecked());
   else if (event->key() == Qt::Key_N)
      lightCheck->setChecked(!lightCheck->isChecked());
   else if (event->key() == Qt::Key_E)
      exaggerCheck->setChecked(!exaggerCheck->isChecked());
   else if (event->key() == Qt::Key_S)
      stereoCheck->setChecked(!stereoCheck->isChecked());
   else if (event->key() == Qt::Key_W)
      wireFrameCheck->setChecked(!wireFrameCheck->isChecked());
   else if (event->key() == Qt::Key_Return)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("select", row);
   }
   else if (event->key() == Qt::Key_T)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("toggle", row);
   }
   else if (event->key() == Qt::Key_Z)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("fullres", row);
   }
   else if (event->key() == Qt::Key_Backspace)
   {
      int row = viewerTable->currentRow();

      if (row!=-1)
         runAction("delete", row);
   }
   else
      QWidget::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
   QWidget::keyReleaseEvent(event);
}

QSize MyQTableWidget::minimumSizeHint() const
{
   return(QSize(MAIN_TABLE_MINWIDTH, MAIN_TABLE_MINHEIGHT));
}

QSize MyQTableWidget::sizeHint() const
{
   return(QSize(MAIN_TABLE_WIDTH, MAIN_TABLE_HEIGHT));
}

void MyQTableWidget::keyPressEvent(QKeyEvent *event)
{
   char c = event->text().toStdString()[0];

   if (isalpha(c))
      QWidget::keyPressEvent(event); // pass to parent
   else
      QTableWidget::keyPressEvent(event); // pass to widget
}

void MyQTableWidget::mousePressEvent(QMouseEvent *event)
{
   if (QApplication::keyboardModifiers() & Qt::AltModifier)
   {
      // get item
      int row = -1;
      QTableWidgetItem *item = itemAt(event->pos());

      // select item
      if (item)
      {
         row = item->row();
         emit(activate("select", row));
      }
   }
   else
      QTableWidget::mousePressEvent(event); // pass to widget
}

void MyQTableWidget::showContextMenu(const QPoint &pos)
{
    // map widget to global coordinates
    QPoint globalPos = mapToGlobal(pos);

    // get item
    int row = -1;
    QTableWidgetItem *item = itemAt(pos);

    // highlight item
    if (item)
    {
       row = item->row();
       setCurrentCell(row, 1);
    }

    // create contex menu
    QMenu myMenu;
    // create layers:
    QAction *openAction = new QAction(tr("open"), this);
    myMenu.addAction(openAction);
    QAction *createAction = new QAction(tr("create extent"), this);
    if (row != -1) myMenu.addAction(createAction);
    myMenu.addSeparator();
    // select layers:
    QAction *selectAction = new QAction(tr("select layer"), this);
    if (row != -1) myMenu.addAction(selectAction);
    QAction *selectAllAction = new QAction(tr("select all"), this);
    myMenu.addAction(selectAllAction);
    QAction *deselectAllAction = new QAction(tr("deselect all"), this);
    myMenu.addAction(deselectAllAction);
    myMenu.addSeparator();
    // toggle layers:
    QAction *infoAction = new QAction(tr("show info"), this);
    if (row != -1) myMenu.addAction(infoAction);
    QAction *showAction = new QAction(tr("show layer"), this);
    if (row != -1) myMenu.addAction(showAction);
    QAction *hideAction = new QAction(tr("hide layer"), this);
    if (row != -1) myMenu.addAction(hideAction);
    QAction *fullresAction = new QAction(tr("toggle full-res"), this);
    if (row != -1) myMenu.addAction(fullresAction);
    myMenu.addSeparator();
    // create tilesets:
    QAction *infoTileset = new QAction(tr("create a tileset:"), this);
    infoTileset->setEnabled(false);
    myMenu.addAction(infoTileset);
    QAction *resampleAction = new QAction(tr(" resample area to tileset"), this);
    if (row != -1) myMenu.addAction(resampleAction);
    QAction *resampleSelAction = new QAction(tr(" resample selection to tileset"), this);
    myMenu.addAction(resampleSelAction);
    QAction *resampleSelAreaAction = new QAction(tr(" resample selection in area"), this);
    if (row != -1) myMenu.addAction(resampleSelAreaAction);
    QAction *resampleAllAction = new QAction(tr(" resample all"), this);
    myMenu.addAction(resampleAllAction);
    myMenu.addSeparator();
    // create layers:
    QAction *infoLayer = new QAction(tr("create a layer:"), this);
    infoLayer->setEnabled(false);
    myMenu.addAction(infoLayer);
    QAction *shadeAction = new QAction(tr(" shade layer"), this);
    if (row != -1) myMenu.addAction(shadeAction);
    QAction *shadeSelAction = new QAction(tr(" shade selected layers"), this);
    myMenu.addAction(shadeSelAction);
    QAction *cropElevAction = new QAction(tr(" crop elevation to layer"), this);
    myMenu.addAction(cropElevAction);
    QAction *cropImagAction = new QAction(tr(" crop imagery to layer"), this);
    myMenu.addAction(cropImagAction);
    QAction *cropSelAction = new QAction(tr(" crop selection to layer"), this);
    myMenu.addAction(cropSelAction);
    myMenu.addSeparator();
    // abort job:
    QAction *abortAction = new QAction(tr("abort"), this);
    myMenu.addAction(abortAction);
    myMenu.addSeparator();
    // transform layers:
    QAction *infoTransform = new QAction(tr("transform a layer:"), this);
    infoTransform->setEnabled(false);
    if (row != -1) myMenu.addAction(infoTransform);
    QAction *treatBlackAction = new QAction(tr(" treat black as transparent"), this);
    if (row != -1) myMenu.addAction(treatBlackAction);
    QAction *treatWhiteAction = new QAction(tr(" treat white as transparent"), this);
    if (row != -1) myMenu.addAction(treatWhiteAction);
    QAction *maskBlackAction = new QAction(tr(" mask black to be transparent"), this);
    if (row != -1) myMenu.addAction(maskBlackAction);
    QAction *maskWhiteAction = new QAction(tr(" mask white to be transparent"), this);
    if (row != -1) myMenu.addAction(maskWhiteAction);
    QAction *removeBathyAction = new QAction(tr(" remove bathymetry elevation range"), this);
    if (row != -1) myMenu.addAction(removeBathyAction);
    QAction *keepBathyAction = new QAction(tr(" keep bathymetry elevation range"), this);
    if (row != -1) myMenu.addAction(keepBathyAction);
    QAction *fillMissingAction = new QAction(tr(" fill missing elevation points"), this);
    if (row != -1) myMenu.addAction(fillMissingAction);
    QAction *fillHolesAction = new QAction(tr(" fill elevation holes"), this);
    if (row != -1) myMenu.addAction(fillHolesAction);
    if (row != -1) myMenu.addSeparator();
    // colorize layers:
    QAction *infoColorize = new QAction(tr("colorize a layer:"), this);
    infoColorize->setEnabled(false);
    if (row != -1) myMenu.addAction(infoColorize);
    QAction *contourAction = new QAction(tr(" contour layer"), this);
    if (row != -1) myMenu.addAction(contourAction);
    if (row != -1) myMenu.addSeparator();
    // save layers:
    QAction *saveDBAction = new QAction(tr("save layer to DB file"), this);
    if (row != -1) myMenu.addAction(saveDBAction);
    QAction *saveGeoTiffAction = new QAction(tr("save layer to GeoTiff file"), this);
    if (row != -1) myMenu.addAction(saveGeoTiffAction);
    QAction *saveJpgInTifAction = new QAction(tr("save layer to JpgInTif file"), this);
    if (row != -1) myMenu.addAction(saveJpgInTifAction);
    if (row != -1) myMenu.addSeparator();
    // layer lists:
    QAction *loadAction = new QAction(tr("load layer list"), this);
    myMenu.addAction(loadAction);
    QAction *saveAction = new QAction(tr("save layer list"), this);
    myMenu.addAction(saveAction);
    QAction *saveGridAction = new QAction(tr("save layers to grid file"), this);
    myMenu.addAction(saveGridAction);
    myMenu.addSeparator();
    // delete layers:
    QAction *deleteAction = new QAction(tr("delete layer"), this);
    if (row != -1) myMenu.addAction(deleteAction);
    QAction *deleteSelAction = new QAction(tr("delete selection"), this);
    myMenu.addAction(deleteSelAction);
    QAction *deleteAllAction = new QAction(tr("delete all"), this);
    myMenu.addAction(deleteAllAction);

    // exec context menu
    QAction *selectedAction = myMenu.exec(globalPos);

    // process selected action
    if (selectedAction)
       //
       if (selectedAction == openAction)
          emit(activate("open"));
       else if (selectedAction == createAction)
          emit(activate("create_extent", row));
       //
       else if (selectedAction == selectAction)
          emit(activate("select", row));
       else if (selectedAction == selectAllAction)
          emit(activate("select_all"));
       else if (selectedAction == deselectAllAction)
          emit(activate("deselect_all"));
       //
       else if (selectedAction == infoAction)
          emit(activate("info", row));
       else if (selectedAction == showAction)
          emit(activate("show", row));
       else if (selectedAction == hideAction)
          emit(activate("hide", row));
       else if (selectedAction == fullresAction)
          emit(activate("fullres", row));
       //
       else if (selectedAction == shadeAction)
          emit(activate("shade_elevation", row));
       else if (selectedAction == shadeSelAction)
          emit(activate("shade_selected"));
       //
       else if (selectedAction == resampleAction)
          emit(activate("resample", row));
       else if (selectedAction == resampleSelAction)
          emit(activate("resample_selected"));
       else if (selectedAction == resampleSelAreaAction)
          emit(activate("resample_selected_area", row));
       else if (selectedAction == resampleAllAction)
          emit(activate("resample_all"));
       //
       else if (selectedAction == cropElevAction)
          emit(activate("crop_elevation", row));
       else if (selectedAction == cropImagAction)
          emit(activate("crop_imagery", row));
       else if (selectedAction == cropSelAction)
          emit(activate("crop_selected", row));
       //
       else if (selectedAction == abortAction)
          emit(activate("abort"));
       //
       else if (selectedAction == treatBlackAction)
          emit(activate("treat_black", row));
       else if (selectedAction == treatWhiteAction)
          emit(activate("treat_white", row));
       else if (selectedAction == maskBlackAction)
          emit(activate("mask_black", row));
       else if (selectedAction == maskWhiteAction)
          emit(activate("mask_white", row));
       else if (selectedAction == removeBathyAction)
          emit(activate("remove_bathy", row));
       else if (selectedAction == keepBathyAction)
          emit(activate("keep_bathy", row));
       else if (selectedAction == fillMissingAction)
          emit(activate("fill_missing", row));
       else if (selectedAction == fillHolesAction)
          emit(activate("fill_holes", row));
       //
       else if (selectedAction == contourAction)
          emit(activate("contour", row));
       //
       else if (selectedAction == saveDBAction)
          emit(activate("save_db", row));
       else if (selectedAction == saveGeoTiffAction)
          emit(activate("save_tif", row));
       else if (selectedAction == saveJpgInTifAction)
          emit(activate("save_jpgintif", row));
       //
       else if (selectedAction == saveAction)
          emit(activate("save"));
       else if (selectedAction == saveGridAction)
          emit(activate("save_grid", row));
       else if (selectedAction == loadAction)
          emit(activate("load"));
       //
       else if (selectedAction == deleteAction)
          emit(activate("delete", row));
       else if (selectedAction == deleteSelAction)
          emit(activate("delete_selected"));
       else if (selectedAction == deleteAllAction)
          emit(activate("delete_all"));
}

void MyQTableWidget::dragEnterEvent(QDragEnterEvent *event) {event->acceptProposedAction();}
void MyQTableWidget::dragMoveEvent(QDragMoveEvent *event) {event->acceptProposedAction();}
void MyQTableWidget::dropEvent(QDropEvent *event) {viewerWindow->dropEvent(event);}
void MyQTableWidget::dragLeaveEvent(QDragLeaveEvent *event) {event->accept();}
