// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MININODE_PATH_H
#define MININODE_PATH_H

#include "mininodes.h"
#include "minipath.h"

//! a geometry node that implements C-LOD for geo-referenced paths
class mininode_geometry_path: public mininode_geometry, minipath
   {
   public:

   //! default constructor
   mininode_geometry_path();

   //! destructor
   virtual ~mininode_geometry_path();

   //! recreate geometry from actual view point
   void recreate(double width);
   };

#endif
