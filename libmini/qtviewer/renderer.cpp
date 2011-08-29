#include <QtGui>
#include <QtOpenGL/qgl.h>

#include <mini/minibase.h>
#include <mini/miniOGL.h>
#include <mini/minirgb.h>

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
{
   this->window = window;
   m_bIsInited = false;

   m_strURL=NULL;

   viewer=NULL;
   camera=NULL;
}

Renderer::~Renderer()
{
   if (m_strURL!=NULL)
      free(m_strURL);

   if (viewer!=NULL)
      delete viewer;

   if (camera!=NULL)
      delete camera;
}

void Renderer::setMapURL(const char* url)
{
   if (m_strURL!=NULL)
      free(m_strURL);

   m_strURL=strdup(url);
}

void Renderer::init()
{
   if (m_bIsInited) return;

   // viewport size
   viewportwidth=window->width();
   viewportheight=window->height();

   // create the viewer object
   viewer=new viewerbase();

   // initialize VIS bathy map
   initVISbathymap();

   // init libMini parameters
   initParameters();

   // load layered tileset
   if (!viewer->getearth()->loadLTS(m_strURL, TRUE, TRUE, VIEWER_LEVELS)) //!! LTS broken
   {
      QString message;
      message.sprintf("Unable to load map data from url=%s\n", m_strURL);
      QMessageBox::warning(window, "Error", message, QMessageBox::Ok);
      return;
   }

   // load optional features
   viewer->getearth()->loadopts();

   // create the camera object
   camera=new minicam(viewer->getearth());

   // initialize the view
   initView();

   // initialize the camera transition
   initTransition();

   // load textures
   loadTextureFromResource(":/images/crosshair.png", m_CrosshairTextureId);

   m_bIsInited = true;
}

void Renderer::setCamera(float latitude, float longitude, float altitude, float heading, float pitch)
{
   if (camera==NULL) return;

   minicoord eye(miniv3d(latitude * 3600.0, longitude * 3600.0, altitude),
                 minicoord::MINICOORD_LLH);

   camera->set_eye(eye,heading,pitch);
}

// initialize libMini parameters
void Renderer::initParameters()
{
   // the viewing parameters
   viewerbase::VIEWER_PARAMS viewerParams;
   viewer->get(viewerParams);
   viewerParams.winwidth = viewportwidth;
   viewerParams.winheight = viewportheight;
   viewerParams.fps = VIEWER_FPS;
   viewerParams.fovy = VIEWER_FOVY;
   viewerParams.nearp = VIEWER_NEARP;
   viewerParams.farp = VIEWER_FARP;
   viewerParams.usewireframe = FALSE;
   viewer->set(viewerParams);
   m_pViewerParams = viewer->get();

   // the earth parameters
   miniearth::MINIEARTH_PARAMS earthParams;
   viewer->getearth()->get(earthParams);
   earthParams.warpmode = WARPMODE_AFFINE_REF;
   earthParams.nonlin = TRUE;
   earthParams.usefog = TRUE;
   earthParams.useshaders = TRUE;
   earthParams.usediffuse = FALSE;
   earthParams.usevisshader = TRUE;
   earthParams.usebathymap = TRUE;
   earthParams.usecontours = FALSE;
   earthParams.useearth = TRUE;
   earthParams.fogstart = VIEWER_FOGSTART;
   earthParams.fogdensity = VIEWER_FOGDENSITY;
   earthParams.voidstart = VIEWER_VOIDSTART;
   earthParams.abyssstart = VIEWER_ABYSSSTART;
   viewer->getearth()->set(earthParams);
   m_pEarthParams = viewer->getearth()->get();

   // the terrain parameters
   miniterrain::MINITERRAIN_PARAMS terrainParams;
   viewer->getearth()->getterrain()->get(terrainParams);
   terrainParams.scale = 1.0f;
   terrainParams.exaggeration = VIEWER_EXAGGER;
   terrainParams.res = VIEWER_RES;
   terrainParams.relres1 = VIEWER_RELRES;
   terrainParams.range = VIEWER_RANGE;
   terrainParams.relrange1 = VIEWER_RELRANGE;
   terrainParams.sealevel = VIEWER_SEALEVEL;
   terrainParams.genmipmaps = TRUE;
   terrainParams.automipmap = TRUE;
   terrainParams.autocompress = FALSE;
   terrainParams.lod0uncompressed = FALSE;
   terrainParams.contours = VIEWER_CONTOURS;
   terrainParams.seabottom = VIEWER_SEABOTTOM;
   terrainParams.bathystart = VIEWER_BATHYSTART;
   terrainParams.bathyend = VIEWER_BATHYEND;
   terrainParams.bathymap = m_BathyMap;
   terrainParams.bathywidth = VIEWER_BATHYWIDTH;
   terrainParams.bathyheight = VIEWER_BATHYHEIGHT;
   terrainParams.bathycomps = VIEWER_BATHYCOMPS;
   viewer->getearth()->getterrain()->set(terrainParams);
   m_pTerrainParams  =  viewer->getearth()->getterrain()->get();
}

// initialize VIS bathy map
void Renderer::initVISbathymap()
{
   float rgba[4];
   static const float hue1=0.0f;
   static const float hue2=240.0f;

   for (int i=0; i<VIEWER_BATHYWIDTH; i++)
   {
      float t=(float)i/(VIEWER_BATHYWIDTH-1);

      hsv2rgb(hue1+(hue2-hue1)*t,1.0f,1.0f,rgba);

      rgba[3]=fpow(1.0f-fabs(2.0f*(t-0.5f)),1.0f/3);

      m_BathyMap[4*i]=m_BathyMap[4*(i+VIEWER_BATHYWIDTH)]=ftrc(255.0f*rgba[0]+0.5f);
      m_BathyMap[4*i+1]=m_BathyMap[4*(i+VIEWER_BATHYWIDTH)+1]=ftrc(255.0f*rgba[1]+0.5f);
      m_BathyMap[4*i+2]=m_BathyMap[4*(i+VIEWER_BATHYWIDTH)+2]=ftrc(255.0f*rgba[2]+0.5f);
      m_BathyMap[4*i+3]=m_BathyMap[4*(i+VIEWER_BATHYWIDTH)+3]=ftrc(255.0f*rgba[3]+0.5f);
   }
}

// initialize the view point
void Renderer::initView()
{
   resizeViewport();

   camera->move_down(-CAMERA_HEIGHT_START);
   camera->move_above(CAMERA_HEIGHT_FLOOR);

   viewer->initeyepoint(camera->get_eye());
}

// initialize the camera transition
void Renderer::initTransition()
{
   m_TransitionTimerId = -1;
   m_IdlingTimerId = -1;

   m_bInCameraTransition = false;

   m_CursorValid = false;
}

// resize window
void Renderer::resizeWindow(int width, int height)
{
   viewportwidth = width;
   viewportheight = height;
   resizeViewport();

   window->updateGL();
}

// initialize the render window
void Renderer::resizeViewport()
{
   int winWidth = viewportwidth;
   int winHeight = viewportheight;

   if (winWidth<1) winWidth=1;
   if (winHeight<1) winHeight=1;

   m_pViewerParams->winwidth=winWidth;
   m_pViewerParams->winheight=winHeight;
   viewer->set(m_pViewerParams);

   glViewport(0, 0, winWidth, winHeight);
}

void Renderer::draw()
{
   setupMatrix();
   renderTerrain();
   renderHUD();

   startIdling();
}

void Renderer::setupMatrix()
{
   glViewport(0.0f, 0.0f, viewportwidth, viewportheight);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   float fovy = m_pViewerParams->fovy;
   float aspectRatio = (float)viewportwidth/viewportheight;
   double nearpGL = m_pViewerParams->nearp;
   double farpGL = m_pViewerParams->farp;

   gluPerspective(fovy, aspectRatio, nearpGL, farpGL);

   minicoord eyeGL = camera->get_eye_opengl();
   miniv3d dirGL = camera->get_dir_opengl();
   miniv3d upGL = camera->get_up_opengl();

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(eyeGL.vec.x, eyeGL.vec.y, eyeGL.vec.z,
             eyeGL.vec.x+dirGL.x, eyeGL.vec.y+dirGL.y, eyeGL.vec.z+dirGL.z,
             upGL.x,upGL.y,upGL.z);
}

void Renderer::renderTerrain()
{
   minilayer *nst;

   // start timer
   viewer->starttimer();

   // set reference layer
   nst=viewer->getearth()->getnearest(camera->get_eye());
   viewer->getearth()->setreference(nst);

   // update scene
   float fAspectRatio = (float)viewportwidth/viewportheight;
   viewer->cache(camera->get_eye(), camera->get_dir(), camera->get_up(), fAspectRatio);

   // render scene
   viewer->clear();
   viewer->render();

   // get time spent
   double delta=viewer->gettimer();

   // update quality parameters
   viewer->adapt(delta);
}

void Renderer::loadTextureFromResource(const char* respath, GLuint& texId)
{
   QImage tex, buf;
   bool bLoaded = buf.load(respath);

   if (bLoaded)
      tex = QGLWidget::convertToGLFormat(buf);

   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);
   glTexImage2D(GL_TEXTURE_2D, 0, 4, tex.width(), tex.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex.bits());
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Renderer::renderHUD()
{
   // draw crosshair:

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glBindTexture(GL_TEXTURE_2D, m_CrosshairTextureId);
   glEnable(GL_TEXTURE_2D);
   glDisable(GL_CULL_FACE);

   float lx = 60.0f/viewportwidth;
   float ly = 60.0f/viewportheight;

   glColor4f(1.0f,1.0f,1.0f, 1.0f);

   glBegin(GL_QUADS);
   glTexCoord2f(0.0f,  0.0f); glVertex2f(-lx, ly); // P0
   glTexCoord2f(1.0f,  0.0f); glVertex2f(lx, ly); // P1
   glTexCoord2f(1.0f,  1.0f); glVertex2f(lx, -ly); // P2
   glTexCoord2f(0.0f,  1.0f);  glVertex2f(-lx, -ly); // P3
   glEnd();

   glEnable(GL_DEPTH_TEST);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glBindTexture(GL_TEXTURE_2D, 0);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_CULL_FACE);

   // render text:

   minicoord cameraPosLLH = camera->get_eye();
   if (cameraPosLLH.type!=minicoord::MINICOORD_LINEAR) cameraPosLLH.convert2(minicoord::MINICOORD_LLH);

   double cameraElev=camera->get_elev();
   if (cameraElev==-MAXFLOAT) cameraElev=0.0f;

   QString str;
   const QColor color(255, 255, 255);
   window->qglColor(color);
   int x = 10;
   int y = window->height() - 20;
   int line_space = -16;
   int second_column_offset = 90;

   str.sprintf("Latitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.x/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Longitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f", cameraPosLLH.vec.y/3600.0);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Altitude:");
   drawText(x, y, str);
   str.sprintf("%-6.2f m", cameraPosLLH.vec.z);
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Heading:");
   drawText(x, y, str);
   str.sprintf("%-6.0f", camera->get_angle());
   drawText(x+second_column_offset, y, str);
   y+=line_space;

   str.sprintf("Pitch:");
   drawText(x, y, str);
   str.sprintf("%-6.0f", camera->get_pitch());
   drawText(x+second_column_offset, y, str);
   y+=line_space;
}

void Renderer::drawText(float x, float y, QString& str, QColor color, bool bIsDoublePrint)
{
   int sx = x;
   int sy = window->height() - y;

   if (!bIsDoublePrint)
   {
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
   else
   {
      window->qglColor(QColor(0, 0, 0, 255));
      window->renderText(sx+2, sy+2, str);
      window->qglColor(color);
      window->renderText(sx, sy, str);
   }
}

void Renderer::rotateCamera(float dx, float dy)
{
   stopTransition();

   camera->rotate_right(180 *dx);
   camera->rotate_up(-180 * dy);

   //!!window->updateGL();
}

void Renderer::moveCameraForward(float delta)
{
   stopTransition();

   camera->move_forward(1000 * delta);

   //!!window->updateGL();
}

void Renderer::moveCameraSideward(float delta)
{
   stopTransition();

   camera->move_right(-1000 * delta);

   //!!window->updateGL();
}

void Renderer::focusOnTarget()
{
   if (!m_CursorValid) return;

   stopTransition();

   double x, y, z;
   GLdouble modelMatrix[16];
   GLdouble projMatrix[16];
   int viewport[4];

   glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
   glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
   glGetIntegerv(GL_VIEWPORT, viewport);

   // calculate the ray, starting from the opengl camera pos and passing through where mouse pointer is
   miniv3d pointerScreenPos(m_CursorScreenPos.x(), viewportheight-m_CursorScreenPos.y(), 0.0);
   gluUnProject(pointerScreenPos.x, pointerScreenPos.y, 0.0, modelMatrix, projMatrix, viewport, &x, &y, &z);
   miniv3d dir = miniv3d(x,y,z) - camera->get_eye_opengl();
   dir = viewer->rot_o2g(dir, camera->get_eye_opengl());
   dir.normalize();

   // trace to find the hit point under current focus
   minicoord hit = camera->get_hit();
   if (hit != camera->get_eye())
   {
      // trace to find the hit distance under current cursor
      double dist = viewer->shoot(camera->get_eye(), dir);
      if (dist != MAXFLOAT)
      {
         // find out the hit point under current cursor
         minicoord target = camera->get_eye() + dist * dir;

         // find out the target position of camera transition
         minicoord cameraTargetPos = target - hit + camera->get_eye();

         startTransition(cameraTargetPos);
      }
   }

   //!! window->updateGL();
}

void Renderer::processTransition(int deltaT)
{
   const double speed = 10000.0f; // speed is 10km/second

   double dt = deltaT / 1000.0f;
   miniv3d dir = m_TargetCameraPos.vec - camera->get_eye().vec;

   if (dir.getlength() > speed * dt)
   {
      dir.normalize();
      camera->move(dir * speed * dt);
   }
   else
   {
      camera->move(dir);
      stopTransition();
   }

   window->updateGL();
}

void Renderer::timerEvent(int timerId)
{
   if (timerId == m_IdlingTimerId)
   {
      //!!if (!m_bInCameraTransition)
      {
         draw();
         window->updateGL();
      }

      bool bPagingFinished = !viewer->getearth()->checkpending();

      //!!if (bPagingFinished)
         //!!stopIdling();
   }
   else if (timerId == m_TransitionTimerId)
   {
      int deltaT = m_Timer.restart();

      processTransition(deltaT);
   }
}

void Renderer::startIdling()
{
   if (m_IdlingTimerId == -1)
      m_IdlingTimerId = window->startTimer((int)(1000.0/VIEWER_FPS));
}

void Renderer::stopIdling()
{
   if (m_IdlingTimerId != -1)
   {
      window->killTimer(m_IdlingTimerId);
      m_IdlingTimerId=-1;
   }
}

void Renderer::startTransition(minicoord target)
{
   stopTransition();

   m_TransitionTimerId = window->startTimer((int)(1000.0/VIEWER_FPS));
   m_TargetCameraPos = target;
   m_bInCameraTransition = true;
   m_Timer.start();
}

void Renderer::stopTransition()
{
   if (m_TransitionTimerId != -1)
   {
      window->killTimer(m_TransitionTimerId);
      m_TransitionTimerId = -1;
      m_bInCameraTransition = false;
   }
}

void Renderer::moveCursor(const QPoint& pos)
{
   m_CursorScreenPos = pos;
   m_CursorValid = true;
}
