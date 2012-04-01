// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef NODES_H
#define NODES_H

#include <mini/mininodes.h>
#include <grid/grid_extent.h>

//! grid_extent evaluator node
class node_grid_extent: public mininode_geometry_evaluator, public grid_extent
   {
   public:

   //! default constructor
   node_grid_extent();

   //! custom constructor
   node_grid_extent(const minicoord &leftbottom,const minicoord &rightbottom,const minicoord &lefttop,const minicoord &righttop);

   //! default destructor
   virtual ~node_grid_extent() {}

   protected:

   void construct();

   virtual miniv3d evaluate(double x,double y);
   };

#endif
