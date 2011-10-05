// (c) by Stefan Roettger

#ifndef RENDERER_H
#define RENDERER_H

#include <QtOpenGL/qgl.h>

#include <QtCore/QTime>
#include <QtCore/QPoint>

#include <mini/miniearth.h>
#include <mini/miniterrain.h>
#include <mini/minilayer.h>

#include <mini/minicam.h>
#include <mini/minianim.h>

#include <mini/viewerbase.h>
#include "viewerconst.h"

enum modifierKeys
{
   ModifierShift,
   ModifierControl,
   ModifierAlt,
   ModifierMeta
};

class Viewer : public viewerbase
{
   void render_ecef_geometry();
};

class Renderer
{
public:
   Renderer(QGLWidget* window);
   ~Renderer();

   void       init();
   bool       isInited() {return(m_bIsInited);}
   void       resizeWindow(int width, int height);
   minilayer* loadMap(const char* url);
   void       clearMaps();

   void       draw();

   void       setCamera(float latitude, float longitude, float altitude, float heading, float pitch);
   minicam*   getCamera() {return(camera);}

   void       rotateCamera(float dx, float dy);

   void       moveCameraForward(float delta);
   void       moveCameraSideward(float delta);

   void       focusOnTarget(double zoom=0.0); // zoom in factor 0.0=none 1.0=full
   void       focusOnMap(minilayer *layer);

   void       timerEvent(int timerId);
   void       moveCursor(const QPoint& pos);
   void       modifierKey(modifierKeys modifier, bool pressed);

   void       toggleWireFrame(bool on);

   void       checkFog(bool on);
   void       setFogDensity(double density);
   void       checkContours(bool on);
   void       checkSeaLevel(bool on);
   void       setSeaLevel(double level);
   void       checkLight(bool on);
   void       setLight(double hour);

protected:
   void       initParameters();
   void       initBathyMap();

   void       resizeViewport();

   void       initView();
   void       initTransition();

   void       setupMatrix();
   void       renderTerrain();
   void       renderHUD();

   void       startIdling();
   void       stopIdling();

   void       startTransition(minianim target, double dangle, double dpitch, double dtime, double follow);
   void       stopTransition();

   void       processTransition(double t, double dt);

   miniv3d    unprojectMouse();

   miniv3d    hitVector();
   miniv3d    targetVector();
   miniv3d    cursorVector(double zoom=0.0);

private:
   void       loadTextureFromResource(const char* respath, GLuint& texId);
   void       drawText(float x, float y, QString& str, QColor color = QColor(255, 255, 255), bool bIsDoublePrint = true);

protected:
   QGLWidget* window;

   int viewportwidth;
   int viewportheight;

   bool m_bIsInited;

   // viewer
   Viewer* viewer;
   viewerbase::VIEWER_PARAMS* m_pViewerParams; // the viewing parameters
   miniearth::MINIEARTH_PARAMS* m_pEarthParams; // the earth parameters
   miniterrain::MINITERRAIN_PARAMS* m_pTerrainParams; // the terrain parameters
   unsigned char m_BathyMap[VIEWER_BATHYWIDTH*4*2]; // bathy color map

   // camera
   minicam*  camera;

private:
   // camera idling timer
   int       m_IdlingTimerId;
   QTime     m_IdlingTimer;

   // camera transition animation
   minianim  m_TargetCameraAnim;
   double    m_TargetDeltaAngle;
   double    m_TargetDeltaPitch;
   double    m_TargetDeltaTime;
   double    m_TargetCameraFollow;
   bool      m_bInCameraTransition;
   int       m_TransitionTimerId;
   QTime     m_TransitionTimer;
   QTime     m_TransitionStart;

protected:
   // cursor position
   QPoint    m_CursorScreenPos;
   bool      m_CursorValid;

   // modifier keys
   bool      m_Shift, m_Control, m_Alt, m_Meta;

private:
   // texture ids
   GLuint    m_CrosshairTextureId;

   double    m_FogDensity;
   double    m_SeaLevel;
   double    m_DayHour;

   double    delta(double a, double b);
};

#endif