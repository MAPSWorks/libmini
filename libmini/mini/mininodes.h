// (c) by Stefan Roettger

#ifndef MININODES_H
#define MININODES_H

#include <iostream>

#include "minibase.h"
#include "minimath.h"

#include "minimath.h"
#include "minicoord.h"
#include "mininode.h"

#include "miniOGL.h"
#include "minicam.h"

//! camera node
class mininode_cam: public mininode, public minicam
   {
   public:

   static const unsigned int ID=1;

   //! default constructor
   mininode_cam(miniearth *earth,
                double lat=21.39,double lon=-157.72,double height=7E6,
                double mindist=0.0)
      : mininode(ID), minicam(earth,lat,lon,height,mindist)
      {}

   //! destructor
   virtual ~mininode_cam()
      {}
   };

//! transformation node
class mininode_transform: public mininode
   {
   public:

   static const unsigned int ID=2;

   //! default constructor
   mininode_transform(const miniv4d mtx[3]=NULL)
      : mininode(ID)
      {if (mtx!=NULL) mtxget(mtx,oglmtx);}

   //! destructor
   virtual ~mininode_transform()
      {}

   protected:

   double oglmtx[16];

   virtual void traverse_pre()
      {mtxpush(); mtxmult(oglmtx);}

   virtual void traverse_post()
      {mtxpop();}
   };

//! translation node
class mininode_translate: public mininode_transform
   {
   public:

   //! default constructor
   mininode_translate(const miniv3d &v)
      : mininode_transform()
      {
      miniv4d mtx[3]={miniv4d(1,0,0,v.x),miniv4d(0,1,0,v.y),miniv4d(0,0,1,v.z)};
      mtxget(mtx,oglmtx);
      }
   };

//! rotation node
class mininode_rotate: public mininode_transform
   {
   public:

   //! default constructor
   mininode_rotate(double d,const miniv3d &a)
      : mininode_transform()
      {
      miniv3d rot[3];
      rot_mtx(rot,d,a);
      mtxget(rot,oglmtx);
      }
   };

//! scale node
class mininode_scale: public mininode_transform
   {
   public:

   //! default constructor
   mininode_scale(double s)
      : mininode_transform()
      {
      miniv3d mtx[3]={miniv3d(s,0,0),miniv3d(0,s,0),miniv3d(0,0,s)};
      mtxget(mtx,oglmtx);
      }
   };

//! coordinate node
class mininode_coord: public mininode_transform
   {
   public:

   //! default constructor
   mininode_coord(const minicoord &c)
      : mininode_transform()
      {
      minicoord ecef=c;
      if (ecef.type!=minicoord::MINICOORD_LINEAR) ecef.convert2(minicoord::MINICOORD_ECEF);

      miniv3d p=ecef.vec;
      miniv3d u=p;
      u.normalize();
      miniv3d d=miniv3d(0,0,1);
      miniv3d r=d/u;
      r.normalize();
      if (r.getlength2()==0.0) r=miniv3d(0,1,0);
      d=u/r;
      d.normalize();

      miniv4d mtx[3]={miniv4d(r.x,u.x,d.x,p.x),miniv4d(r.y,u.y,d.y,p.y),miniv4d(r.z,u.z,d.z,p.z)};
      mtxget(mtx,oglmtx);
      }
   };

//! color node
class mininode_color: public mininode
   {
   public:

   static const unsigned int ID=3;

   //! default constructor
   mininode_color(const miniv4d &c)
      : mininode(ID)
      {rgba=c;}

   mininode_color(const miniv3d &c)
      : mininode(ID)
      {rgba=miniv4d(c,1);}

   //! destructor
   virtual ~mininode_color()
      {}

   protected:

   miniv4d rgba;

   virtual void traverse_pre()
      {color(rgba);}

   virtual void traverse_past()
      {color(rgba);}
   };

//! geometry node
class mininode_geometry: public mininode, public ministrip
   {
   public:

   static const unsigned int ID=4;

   //! default constructor
   mininode_geometry(int colcomps=0,int nrmcomps=0,int texcomps=0)
      : mininode(ID), ministrip(colcomps,nrmcomps,texcomps)
      {}

   //! destructor
   virtual ~mininode_geometry()
      {}

   protected:

   virtual void traverse_pre()
      {render(1);}
   };

//! tube geometry node
class mininode_geometry_tube: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_tube(double radius,double height,int tessel=16)
      : mininode_geometry(0,3,0)
      {
      for (int i=0; i<=tessel; i++)
         {
         double w=2*PI*i/tessel;
         double x=sin(w)*radius;
         double y=cos(w)*radius;

         setnrm(miniv3d(x,y,0.0));
         addvtx(miniv3d(x,y,0.0));
         addvtx(miniv3d(x,y,height));
         }
      }

   mininode_geometry_tube(const miniv3d &p1,const miniv3d &p2,double radius,int tessel=16)
      : mininode_geometry(0,3,0)
      {
      miniv3d dir=p2-p1;
      miniv3d right,up;
      if (dabs(dir.x)>dabs(dir.y) && dabs(dir.x)>dabs(dir.z)) right=miniv3d(0,0,dir.x);
      else if (dabs(dir.y)>dabs(dir.x) && dabs(dir.y)>dabs(dir.z)) right=miniv3d(0,0,dir.y);
      else right=miniv3d(dir.z,0,0);
      up=right/dir;
      right=dir/up;
      right.normalize();
      right*=radius;

      miniv3d rot[3];
      rot_mtx(rot,360.0/tessel,dir);

      for (int i=0; i<=tessel; i++)
         {
         setnrm(right);
         addvtx(p1+right);
         addvtx(p2+right);

         right=mlt_vec(rot,right);
         }
      }

   };

#endif
