// (c) by Stefan Roettger, licensed under GPL 2+

#include <string>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include <QtCore/QUrl>

#include <mini/mini_format.h>
#include <grid/grid.h>

#include "renderer.h"
#include "worker.h"

#include "viewerconst.h"
#include "viewerwindow.h"

ViewerWindow::ViewerWindow()
   : viewer(NULL), bLeftButtonDown(false), bRightButtonDown(false)
{
   setFocusPolicy(Qt::WheelFocus);
   setMouseTracking(true);

   setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::StencilBuffer));

   setCursor(Qt::CrossCursor);

   vertical = TRUE;

   // init viewer
   viewer = new Renderer(this);
   if (viewer==NULL) MEMERROR();

   // init worker
   worker = new WorkerThread;
   if (worker==NULL) MEMERROR();

   // setup queued worker connection
   qRegisterMetaType<ministring>("ministring");
   qRegisterMetaType<ministrings>("ministrings");
   connect(worker, SIGNAL(reportProgress(double)),
           this, SLOT(reportProgress(double)),
           Qt::QueuedConnection);
   connect(worker, SIGNAL(finishedJob(ministring, ministrings)),
           this, SLOT(finishedJob(ministring, ministrings)),
           Qt::QueuedConnection);
   connect(worker, SIGNAL(failedJob(ministring, ministrings)),
           this, SLOT(failedJob(ministring, ministrings)),
           Qt::QueuedConnection);

   // worker settings
   grid_level = 0;
   grid_levels = 1;
   grid_step = 2;

   // accept drag and drop
   setAcceptDrops(true);
}

ViewerWindow::~ViewerWindow()
{
   if (worker!=NULL)
      delete worker;

   if (viewer!=NULL)
      delete viewer;
}

Renderer *ViewerWindow::getViewer()
{
   return(viewer);
}

WorkerThread *ViewerWindow::getWorker()
{
   return(worker);
}

void ViewerWindow::setVertical(BOOLINT on)
{
   vertical=on;
}

QSize ViewerWindow::minimumSizeHint() const
{
   if (vertical)
      return(QSize(VIEWER_MINWIDTH, VIEWER_MINWIDTH));
   else
      return(QSize(VIEWER_MINWIDTH, VIEWER_MINWIDTH/VIEWER_ASPECT));
}

QSize ViewerWindow::sizeHint() const
{
   if (vertical)
      return(QSize(VIEWER_WIDTH, VIEWER_WIDTH));
   else
      return(QSize(VIEWER_WIDTH, VIEWER_WIDTH/VIEWER_ASPECT));
}

void ViewerWindow::initializeGL()
{
   if (!viewer->isInited())
   {
      // initialize viewer here as it needs GL context to init
      viewer->init();

      // load objects url from arguments
      QStringList dataPathList = QCoreApplication::arguments();
      for (int i=1; i<dataPathList.size(); i++)
         runAction("open", dataPathList[i].toStdString().c_str());
   }

   qglClearColor(Qt::black);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
}

void ViewerWindow::resizeGL(int, int)
{
   viewer->resizeWindow();
}

void ViewerWindow::paintGL()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   viewer->draw();
}

void ViewerWindow::mousePressEvent(QMouseEvent *event)
{
   reportModifiers();

   if (event->buttons() & Qt::LeftButton)
      bLeftButtonDown = true;
   else if (event->buttons() & Qt::RightButton)
      bRightButtonDown = true;
   else
      event->ignore();

   lastPos = movedPos = event->pos();
}

void ViewerWindow::mouseReleaseEvent(QMouseEvent *event)
{
   int dx = event->x()-lastPos.x();
   int dy = event->y()-lastPos.y();

   reportModifiers();

   // if mouse did not move and we have buttons down, it is a click
   if (abs(dx)<3 && abs(dy)<3)
   {
      // a left-right button click
      if (bLeftButtonDown && bRightButtonDown)
      {}
      // a left button click
      else if (bLeftButtonDown)
         viewer->getCamera()->focusOnTarget();
      // a right button click
      else if (bRightButtonDown)
      {}
      else
         event->ignore();
   }
   else
      event->ignore();

   bLeftButtonDown = false;
   bRightButtonDown = false;
}

void ViewerWindow::mouseMoveEvent(QMouseEvent *event)
{
   float dx = (float)(event->x()-movedPos.x())/width();
   float dy = (float)(event->y()-movedPos.y())/height();

   reportModifiers();

   viewer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   if (event->buttons() & Qt::LeftButton)
      viewer->getCamera()->rotateCamera(dx, dy);
   else if (event->buttons() & Qt::MiddleButton)
   {}
   else if (event->buttons() & Qt::RightButton)
   {}

   movedPos = event->pos();
}

void ViewerWindow::mouseDoubleClickEvent(QMouseEvent *)
{
   reportModifiers();

   viewer->getCamera()->focusOnTarget(0.75);
}

void ViewerWindow::keyPressEvent(QKeyEvent *event)
{
   if (event->key() == Qt::Key_Space)
      viewer->getCamera()->focusOnTarget();

   QGLWidget::keyPressEvent(event);

   reportModifiers();
}

void ViewerWindow::keyReleaseEvent(QKeyEvent *event)
{
   QGLWidget::keyReleaseEvent(event);

   reportModifiers();
}

void ViewerWindow::wheelEvent(QWheelEvent *event)
{
   double numDegrees = event->delta()/8.0;

   reportModifiers();

   viewer->getCamera()->moveCursor(event->pos().x(), event->pos().y());

   if (event->orientation() == Qt::Vertical)
      viewer->getCamera()->moveCameraForward(numDegrees/360.0);
   else
      viewer->getCamera()->moveCameraSideward(numDegrees/360.0);

   event->accept();
}

void ViewerWindow::timerEvent(QTimerEvent *event)
{
   reportModifiers();
   reportProgress();

   viewer->getCamera()->timerEvent(event->timerId());
}

void ViewerWindow::reportModifiers()
{
   Qt::KeyboardModifiers keyMod = QApplication::keyboardModifiers();

   viewer->getCamera()->modifierKey(ModifierShift, keyMod & Qt::ShiftModifier);
   viewer->getCamera()->modifierKey(ModifierControl, keyMod & Qt::ControlModifier);
   viewer->getCamera()->modifierKey(ModifierAlt, keyMod & Qt::AltModifier);
   viewer->getCamera()->modifierKey(ModifierMeta, keyMod & Qt::MetaModifier);
}

void ViewerWindow::reportProgress()
{
   if (worker->running())
      emit progress(worker->get_progress()); // synchronous
}

void ViewerWindow::setRepo(ministring path)
{
   repository_path = path;

   if (repository_path.endswith("\\"))
      if (repository_path.size()>1)
         repository_path.shrinksize();
      else
         repository_path = "/";

   if (repository_path.size()>0)
      if (!repository_path.endswith("/"))
         repository_path += "/";

   objects.set_repo(repository_path);
}

void ViewerWindow::setExport(ministring path)
{
   export_path = path;

   if (export_path.endswith("\\"))
      if (export_path.size()>1)
         export_path.shrinksize();
      else
         export_path = "/";

   if (export_path.size()>0)
      if (!export_path.endswith("/"))
         export_path += "/";
}

void ViewerWindow::setTmp(ministring path)
{
   tmp_path = path;

   if (tmp_path.endswith("\\"))
      if (tmp_path.size()>1)
         tmp_path.shrinksize();
      else
         tmp_path = "/";

   if (tmp_path.size()>0)
      if (!tmp_path.endswith("/"))
         tmp_path += "/";
}

void ViewerWindow::setResampleSettings(int level, int levels, int step)
{
   grid_level = level;
   grid_levels = levels;
   grid_step = step;
}

void ViewerWindow::setExportSettings(double power, double quality)
{
   shadePower = power;
   jpegQuality = quality;
}

void ViewerWindow::loadURL(ministring url)
{
   if (url.startswith("file://"))
      url = url.suffix("file://");

   if (getObject(url) != NULL) // already existing?
      gotoObject(url);
   else
   {
      // check imagery extensions
      if (url.endswith(".jpg") ||
          url.endswith(".png") ||
          url.endswith(".tif") ||
          url.endswith(".jpgintif"))
         loadImage(url);
      // check elevation extensions
      else if (url.endswith(".bt"))
         loadImage(url);
      // check tileset extensions
      else if (url.endswith(".ini"))
         loadMap(url);
      // check for directories
      else if (!url.suffix("/").contains(".") ||
               !url.suffix("\\").contains("."))
         loadMap(url);
      else
         loadImage(url); // try unknown format via gdal
   }
}

void ViewerWindow::loadURLs(ministrings urls)
{
   for (unsigned int i=0; i<urls.getsize(); i++)
      loadURL(urls[i]);
}

void ViewerWindow::loadMap(ministring url)
{
   if (url.endswith(".ini"))
   {
      unsigned int lio, lio1, lio2;

      lio = url.getsize();

      if (url.findr("/", lio1)) lio = lio1;
      else if (url.findr("\\", lio2)) lio = lio2;

      url.truncate(lio);
   }

   Object_tileset *tileset = new Object_tileset(url, repository_path, viewer);
   if (tileset == NULL) MEMERROR();

   if (!addObject(url, tileset, "tileset"))
   {
      delete tileset;

      notify("Unable to load map from url="+url);
   }
}

void ViewerWindow::clearMaps()
{
   removeObjects(listObjects("tileset"));
}

void ViewerWindow::loadImage(ministring url)
{
   if (url.startswith("file://"))
      url.substitute("file://", "");

   Object_image *image = new Object_image(url, repository_path, viewer);

   if (image == NULL) MEMERROR();

   if (!addObject(url, image, "image"))
   {
      delete image;

      notify("Unable to load image from url="+url);
   }
   else
   {
      if (image->is_imagery())
         addTag(url, "imagery");
      else
         addTag(url, "elevation");

      image->focus();

      ThumbJob *job = new ThumbJob;
      if (job == NULL) MEMERROR();

      job->append(image->get_full_name());
      worker->run_job(job);
   }
}

void ViewerWindow::clearImages()
{
   removeObjects(listObjects("image"));
}

BOOLINT ViewerWindow::addObject(ministring key, Object *obj, ministring tag)
{
   if (objects.add(key, obj, tag))
   {
      emit changed(key);
      return(TRUE);
   }

   return(FALSE);
}

Object *ViewerWindow::getObject(ministring key)
{
   return(objects.get(key));
}

ministrings *ViewerWindow::getTags(ministring key)
{
   return(objects.get_tags(key));
}

void ViewerWindow::addTag(ministring key, ministring tag)
{
   objects.add_tag(key, tag);
   emit changed(key);
}

void ViewerWindow::removeTag(ministring key, ministring tag)
{
   objects.remove_tag(key, tag);
   emit changed(key);
}

void ViewerWindow::toggleTag(ministring key, ministring tag)
{
   if (hasTag(key, tag))
      removeTag(key, tag);
   else
      addTag(key, tag);
}

BOOLINT ViewerWindow::hasTag(ministring key, ministring tag)
{
   return(objects.has_tag(key, tag));
}

ministrings ViewerWindow::listObjects()
{
   return(objects.get_items());
}

ministrings ViewerWindow::listObjects(ministring tag)
{
   return(objects.get_items(tag));
}

void ViewerWindow::gotoObject(ministring key)
{
   Object *obj=objects.get(key);

   if (obj!=NULL) obj->focus();
}

void ViewerWindow::removeObject(ministring key)
{
   objects.remove(key);
   emit changed(key);
}

void ViewerWindow::removeObjects(ministrings keys)
{
   unsigned int i;

   for (i=0; i<keys.getsize(); i++)
      removeObject(keys[i]);
}

void ViewerWindow::clearObjects()
{
   removeObjects(listObjects());
}

void ViewerWindow::toggleStereo(bool on)
{
   viewer->toggleStereo(on);
}

void ViewerWindow::toggleWireFrame(bool on)
{
   viewer->toggleWireFrame(on);
}

void ViewerWindow::checkFog(bool on)
{
   viewer->checkFog(on);
}

void ViewerWindow::setFogDensity(double density)
{
   viewer->setFogDensity(density);
}

void ViewerWindow::checkContours(bool on)
{
   viewer->checkContours(on);
}

void ViewerWindow::checkSeaLevel(bool on)
{
   viewer->checkSeaLevel(on);
}

void ViewerWindow::setSeaLevel(double level)
{
   viewer->setSeaLevel(level);
}

void ViewerWindow::checkLight(bool on)
{
   viewer->checkLight(on);
}

void ViewerWindow::setLight(double hour)
{
   viewer->setLight(hour);
}

void ViewerWindow::checkExagger(bool on)
{
   viewer->checkExagger(on);
}

void ViewerWindow::setExagger(double scale)
{
   viewer->setExagger(scale);
}

void ViewerWindow::dragEnterEvent(QDragEnterEvent *event)
{
   event->acceptProposedAction();
}

void ViewerWindow::dragMoveEvent(QDragMoveEvent *event)
{
   event->acceptProposedAction();
}

void ViewerWindow::dropEvent(QDropEvent *event)
{
   const QMimeData *mimeData = event->mimeData();

   if (mimeData->hasUrls())
   {
      event->acceptProposedAction();

      QList<QUrl> urlList = mimeData->urls();

      for (int i=0; i<urlList.size(); i++)
      {
         QUrl qurl = urlList.at(i);
         ministring url=qurl.toString().toStdString().c_str();

         runAction("open", url);
      }
   }
}

void ViewerWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}

void ViewerWindow::runAction(ministring action,
                             ministring value)
{
   if (action == "repo")
   {
      setRepo(value);
   }
   else if (action == "export")
   {
      setExport(value);
   }
   else if (action == "tmp")
   {
      setTmp(value);
   }
   else if (action == "open")
   {
      if (value != "")
         loadURL(value);
      else
         loadURLs(browse("Open File", repository_path));
   }
   else if (action == "select")
   {
      toggleTag(value, "selected");
   }
   else if (action == "select_all")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (!hasTag(keys[i], "selected"))
             addTag(keys[i], "selected");
   }
   else if (action == "deselect_all")
   {
      ministrings keys = listObjects();

      for (unsigned int i=0; i<keys.size(); i++)
         if (hasTag(keys[i], "selected"))
             removeTag(keys[i], "selected");
   }
   else if (action == "info")
   {
      Object *obj=getObject(value);
      if (obj) notify(obj->get_info());
   }
   else if (action == "show")
   {
      Object *obj=getObject(value);
      if (obj) obj->show();

      removeTag(value, "hidden");
   }
   else if (action == "hide")
   {
      Object *obj=getObject(value);
      if (obj) obj->show(FALSE);

      addTag(value, "hidden");
   }
   else if (action == "toggle")
   {
      Object *obj=getObject(value);
      if (obj)
         if (obj->is_shown())
         {
            obj->show(FALSE);
            addTag(value, "hidden");
         }
         else
         {
            obj->show();
            removeTag(value, "hidden");
         }
   }
   else if (action == "shade")
   {
      shade(value);
   }
   else if (action == "resample")
   {
      if (hasTag(value, "image") &&
          hasTag(value, "elevation"))
      {
         ministrings keys = value;
         resample_list(keys, grid_level, grid_levels, grid_step);
      }
   }
   else if (action == "resample_selected")
   {
      ministrings keys = listObjects("image");

      ministrings sel_keys;
      for (unsigned int i=0; i<keys.getsize(); i++)
         if (hasTag(keys[i], "selected"))
            sel_keys.append(keys[i]);

      resample_list(sel_keys, grid_level, grid_levels, grid_step);
   }
   else if (action == "resample_all")
   {
      ministrings keys = listObjects("image");
      resample_list(keys, grid_level, grid_levels, grid_step);
   }
   else if (action == "abort")
   {
      worker->abort_jobs();
   }
   else if (action == "save_db")
   {
      SaveJob *job = new SaveJob("","",TRUE);
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_full_name());
      worker->run_job(job);
   }
   else if (action == "save_tif")
   {
      SaveJob *job = new SaveJob;
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_full_name());
      worker->run_job(job);
   }
   else if (action == "save_jpgintif")
   {
      SaveJob *job = new SaveJob("","",FALSE,FALSE,FALSE,TRUE,jpegQuality);
      if (job == NULL) MEMERROR();

      job->append(getObject(value)->get_full_name());
      worker->run_job(job);
   }
   else if (action == "save_grid")
   {
      ministrings keys = listObjects("image");
      save_list(keys, "", grid_level);
   }
   else if (action == "delete")
   {
      removeObject(value);
   }
   else if (action == "delete_all")
   {
      removeObjects(listObjects());
   }
   else if (action == "delete_selected")
   {
      removeObjects(listObjects("selected"));
   }
   else if (action == "goto")
   {
      gotoObject(value);
   }
}

void ViewerWindow::shade(ministring key)
{
   Object *obj=getObject(key);
   if (obj!=NULL)
   {
      Object_image *image=dynamic_cast<Object_image *>(obj);
      if (image!=NULL)
         if (!image->is_elevation())
            notify("Shading requires an elevation layer");
         else
         {
            ShadeJob *job = new ShadeJob("",shadePower);
            if (job == NULL) MEMERROR();

            job->append(image->get_full_name());
            worker->run_job(job);
         }
   }
}

BOOLINT ViewerWindow::check_list(ministrings keys)
{
   unsigned int i;

   unsigned int elev,imag;

   elev=imag=0;

   for (i=0; i<keys.size(); i++)
   {
      Object *obj=getObject(keys[i]);
      if (obj!=NULL)
      {
         Object_image *image=dynamic_cast<Object_image *>(obj);
         if (image!=NULL)
         {
            if (image->is_elevation()) elev++;
            if (image->is_imagery()) imag++;
         }
      }
   }

   if (elev==0)
   {
      notify("Resampling requires at least one elevation layer");
      return(FALSE);
   }

   if (elev>1 && imag==0)
   {
      notify("Resampling requires at least one imagery or shaded layer");
      return(FALSE);
   }

   return(TRUE);
}

void ViewerWindow::resample_list(ministrings keys,
                                 int level,int levels,int step)
{
   unsigned int i;

   if (!check_list(keys)) return;

   ResampleJob *job = new ResampleJob(repository_path, export_path,
                                      level, levels, step,
                                      5.0,0.0);

   if (job == NULL) MEMERROR();

   for (i=0; i<keys.size(); i++)
      job->append(getObject(keys[i])->get_relative_name());

   grid_resampler::set_tmp_dir(tmp_path);

   worker->run_job(job);
}

void ViewerWindow::save_list(ministrings keys,ministring filename,int level)
{
   unsigned int i;

   ministrings filenames;

   if (!check_list(keys)) return;

   for (i=0; i<keys.size(); i++)
      filenames.append(getObject(keys[i])->get_relative_name());

   ministrings grid_list = ResampleJob::make_grid_list(filenames,
                                                       repository_path, export_path,
                                                       level);

   if (filename=="")
   {
      ministrings files = browse("Save To Grid File", export_path, TRUE);
      if (files.size()==0) return;

      filename = files[0];
      if (!filename.endswith(".grid")) filename += ".grid";
   }

   grid_list.save(filename);
}

void ViewerWindow::notify(ministring text)
{
   QMessageBox::information(this, "Information",
                            text.c_str(),
                            QMessageBox::Ok);
}

ministrings ViewerWindow::browse(ministring title,
                                 ministring path,
                                 BOOLINT newfile)
{
   QFileDialog* fd = new QFileDialog(this, title.c_str());
   if (fd == NULL) MEMERROR();

   fd->setFileMode(QFileDialog::ExistingFiles);
   fd->setViewMode(QFileDialog::List);
   if (newfile) fd->setAcceptMode(QFileDialog::AcceptSave);
   fd->setFilter("All Files (*.*);;Ini Files (*.ini);;Binary Terrain (*.bt);;Images (*.tif *.tiff *.jpg *.png);; Grid Files (*.grid)");

   if (path!="") fd->setDirectory(path.c_str());

   ministrings files;

   if (fd->exec() == QDialog::Accepted)
      for (int i=0; i<fd->selectedFiles().size(); i++)
      {
         QString fileName = fd->selectedFiles().at(i);

         if (!fileName.isNull())
            files += fileName.toStdString().c_str();
      }

   return(files);
}

ministring ViewerWindow::browseDir(ministring title,
                                   ministring path)
{
   QFileDialog *fd = new QFileDialog(this, title.c_str());
   if (fd == NULL) MEMERROR();

   fd->setFileMode(QFileDialog::DirectoryOnly);
   fd->setViewMode(QFileDialog::List);
   if (path!="") fd->setDirectory(path.c_str());

   ministring dir;

   if (fd->exec() == QDialog::Accepted)
   {
      QString fileName = fd->selectedFiles().at(0);

      if (!fileName.isNull())
         dir=fileName.toStdString().c_str();
   }

   return(dir);
}

void ViewerWindow::reportProgress(double percentage)
{
   emit progress(percentage); // asynchronous
}

void ViewerWindow::finishedJob(const ministring &job, const ministrings &args)
{
   if (job=="thumb")
   {
      // autoload thumbs
      for (unsigned int i=0; i<args.size(); i++)
      {
         ministring thumb=ThumbJob::make_name(args[i]);

         databuf buf;
         buf.loaddata(thumb.c_str());

         Object *obj=getObject(args[i]);
         if (obj!=NULL)
         {
            Object_image *image=dynamic_cast<Object_image *>(obj);
            if (image!=NULL) image->set_thumb(&buf);
         }

         buf.release();
      }
   }
   else if (job=="shader")
   {
      // autoload shaded layers
      for (unsigned int i=0; i<args.size(); i++)
         runAction("open", ShadeJob::make_name(args[i]));
   }
   else if (job=="resampler")
   {
      // make resampled layers invisible
      for (unsigned int i=0; i<args.size(); i++)
         if (getObject(args[i]))
            runAction("hide", args[i]);
         else
            runAction("hide", repository_path+args[i]);

      // autoselect grid name
      ministring name=ResampleJob::make_grid_name(args,repository_path);

      // autoload resampled tileset
      if (name!="")
         runAction("open", export_path+name);
   }
}

void ViewerWindow::failedJob(const ministring &job, const ministrings &args)
{
   if (job=="shader")
   {
      notify("Shading failed");
   }
   else if (job=="resampler")
   {
      notify("Resampling failed");
   }
}