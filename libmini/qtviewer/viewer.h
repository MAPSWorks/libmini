// (c) by Stefan Roettger

#ifndef VIEWER_H
#define VIEWER_H

#include <mini/viewerbase.h>

class minicam;

class Viewer : public viewerbase
{
public:
   Viewer(minicam *camera);
   ~Viewer();

   void render_geometry();

protected:
   void setup_matrix();
   void render_terrain_geometry();
   void render_ecef_geometry();

   minicam *m_camera;
};

#endif
