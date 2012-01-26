// (c) by Stefan Roettger

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
   : Viewer(window)
{}

Renderer::~Renderer()
{}

mininode_group *Renderer::build_ecef_geometry()
{
   mininode_group *group = new mininode_group;

   // define ecef z-axis:

   mininode *axis = new mininode_group;
   mininode_ref pole(new mininode_geometry_tube(10000, 500000));

   axis->append_child(new mininode_coord(minicoord(miniv3d(0, -90*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0, 0.0, 0.5)))->
      append_child(pole);

   axis->append_child(new mininode_coord(minicoord(miniv3d(0, 90*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0, 0.0, 1.0)))->
      append_child(pole);

   group->append_child(axis);

   // define equator:

   minidyna<miniv3d> pos;
   static const int eqlines = 500;

   for (int i=0; i<=eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600, 0.0, 0.0), minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);
      pos.append(c.vec);
      }

   group->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_geometry_band(pos, pos, 20000));

#ifdef TEST

   // tetrahedron /w procedural 2D texture:

   group->append_child(new mininode_coord(minicoord(miniv3d(-159*3600, 21*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_image("data/textures/Checker.db"))->
      append_child(new mininode_scale(10000))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_tet(10));

   // tetrahedron /w procedural 3D texture:

   group->append_child(new mininode_coord(minicoord(miniv3d(-159*3600, 23*3600, 0), minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_volume("data/textures/Marble.db"))->
      append_child(new mininode_scale(10000))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_tet(10));

   // house:

   // define local coordinate system at Lat/Lon=49/11
   mininode *house = new mininode_coord(minicoord(miniv3d(11*3600, 49*3600, 0), minicoord::MINICOORD_LLH));

   // scale up by a factor of 10000
   mininode *scale = house->append_child(new mininode_scale(10000));

   // white cube with base size 10x10 and height 5 meters
   // origin of the cube is its barycenter
   // translate up 2.5 meters
   scale->append_child(new mininode_color(miniv3d(1.0, 1.0, 1.0)))->
      append_child(new mininode_texgen_translate(miniv3d(0.5,0.5,0.5)))->
      append_child(new mininode_texgen_rotate(2,miniv3d(0,1,0)))->
      append_child(new mininode_texgen_rotate(90,miniv3d(1,0,0)))->
      append_child(new mininode_texgen_scale(0.7))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_volume("data/textures/Wood.db"))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 2.5)))->
      append_child(new mininode_geometry_cube(10.0, 10.0, 5.0));

   // grey prism as roof with base size 10x10
   // origin of the prism is the center of its base plane
   // translate up 5 meters
   scale->append_child(new mininode_color(miniv3d(0.5, 0.5, 0.5)))->
      append_child(new mininode_texgen_scale(0.1))->
      append_child(new mininode_image("data/textures/wood.jpg"))->
      append_child(new mininode_translate(miniv3d(0.0, 0.0, 5.0)))->
      append_child(new mininode_geometry_prism(10.0, 10.0, 2.5));

   group->append_child(house);

#endif

   return(group);
}