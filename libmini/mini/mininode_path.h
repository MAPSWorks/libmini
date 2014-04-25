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
      {return(&path);}

   protected:

   minipath path;
   };

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_path_clod: public mininode_geometry
   {
   public:

   //! default constructor
   mininode_geometry_path_clod();

   //! destructor
   virtual ~mininode_geometry_path_clod();

   //! recreate geometry from actual view point
   void recreate(miniv3d eye,
                 double maxsize,double maxdist);

   //! get path
   minipath *getpath()
      {return(&path);}

   protected:

   minipath path;
   };

#endif
