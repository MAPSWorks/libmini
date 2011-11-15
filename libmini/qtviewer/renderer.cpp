// (c) by Stefan Roettger

#include "renderer.h"

Renderer::Renderer(QGLWidget* window)
   : Viewer(window)
{}

Renderer::~Renderer()
{}

mininode *Renderer::build_ecef_geometry()
{
   mininode_group *group=new mininode_group;

   // define ecef z-axis:

   mininode *axis=new mininode_group;
   mininoderef pole=mininoderef(new mininode_geometry_tube(10000,500000));

   axis->append_child(new mininode_coord(minicoord(miniv3d(0,-90*3600,0),minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0,0.0,0.5)))->
      append_child(pole);

   axis->append_child(new mininode_coord(minicoord(miniv3d(0,90*3600,0),minicoord::MINICOORD_LLH)))->
      append_child(new mininode_color(miniv3d(0.0,0.0,1.0)))->
      append_child(pole);

   group->append_child(axis);

   // define equator:

   minidyna<miniv3d> pos;
   static const int eqlines=500;

   for (int i=0; i<=eqlines; i++)
      {
      minicoord c(miniv3d((double)i/eqlines*360*3600,0.0,0.0),minicoord::MINICOORD_LLH);
      c.convert2(minicoord::MINICOORD_ECEF);
      pos.append(c.vec);
      }

   group->append_child(new mininode_color(miniv3d(0.5,0.5,0.5)))->
      append_child(new mininode_geometry_band(pos,pos,20000));

   // define house:

   mininode *house=new mininode_coord(minicoord(miniv3d(11*3600,49*3600,0),minicoord::MINICOORD_LLH));
   mininode *scale=house->append_child(new mininode_scale(10000));
   scale->append_child(new mininode_color(miniv3d(1.0,1.0,1.0)))->
      append_child(new mininode_translate(miniv3d(0,0,2.5)))->
      append_child(new mininode_geometry_cube(10,10,5));
   scale->append_child(new mininode_color(miniv3d(0.5,0.5,0.5)))->
      append_child(new mininode_translate(miniv3d(0,0,5)))->
      append_child(new mininode_geometry_prism(10,10,2.5));
   group->append_child(house);

   return(group);
}
