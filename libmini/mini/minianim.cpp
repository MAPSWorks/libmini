#include "minibase.h"

#include "minimath.h"

#include "minianim.h"

minianim::minianim()
   : minidyna<minicoord>()
   {cam=NULL;}

minianim::minianim(minicam *c)
   : minidyna<minicoord>()
   {cam=c;}

minianim::minianim(minicam *c,const minicoord &v)
  : minidyna<minicoord>(v)
   {cam=c;}

void minianim::append_sector(const minicoord &p1,const minicoord &p2,unsigned int n)
   {
   minicoord a=p1;
   minicoord b=p2;

   if (a.type!=minicoord::MINICOORD_LINEAR) a.convert2(minicoord::MINICOORD_ECEF);
   if (b.type!=minicoord::MINICOORD_LINEAR) b.convert2(minicoord::MINICOORD_ECEF);

   double ha=cam->get_dist(a);
   double hb=cam->get_dist(b);

   append(a);

   for (unsigned int i=1; i<n-1; i++)
      {
      double t=(double)i/n;

      minicoord ab=(1.0-t)*a+t*b;
      double hab=(1.0-t)*ha+t*hb;

      if (cam!=NULL)
         {
         cam->move_above(ab);
         double h=cam->get_dist(ab);
         miniv3d up=-cam->get_down();
         ab+=up*(hab-h);
         }

      append(ab);
      }

   append(b);
   }
