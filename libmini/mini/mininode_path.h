// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_PATH_H
#define MININODE_PATH_H

#include "mininodes.h"
#include "minipath.h"

//! a geometry node for geo-referenced paths
class mininode_geometry_path: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_path();

   //! constructor
   mininode_geometry_path(ministring filename,double width=10.0);

   //! destructor
   virtual ~mininode_geometry_path();

   //! recreate geometry with a particular band width
   void recreate(double width);

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   minipath path_;
   };

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_path_clod: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_path_clod();

   //! destructor
   virtual ~mininode_geometry_path_clod();

   //! load path
   void load(ministring filename);

   //! create geometry from actual view point
   void create(double maxdiff,double atdist,
               int update=100);

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   minipath path_;
   minidyna<float> d2_;
   minidyna<float> md_;

   protected:

   void calcD2();
   float calcD2(int left,int right);

   void addpoint(miniv3d p);
   BOOLINT subdiv(int left,int right);

   void calcpath();
   void calcpath(int left,int right);

   void calcpath_inc(int update);
   void calcpath_inc();

   virtual void traverse_pre()
      {
      calcpath_inc(UPDATE_); // update path incrementally
      mininode_geometry::traverse_pre(); // traverse base class
      }

   private:

   struct state_struct
      {
      int left;
      int right;
      BOOLINT add;
      };

   miniv3d EYE_;
   float C_;

   int UPDATE_;
   minidyna<struct state_struct> STACK_;

   minidyna<miniv3d> BAND_;
   minidyna<miniv3d> NRM_;
   minidyna<double> WIDTH_;
   };

#endif
