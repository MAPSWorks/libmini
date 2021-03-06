// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef DATAGRID_H
#define DATAGRID_H

#include "minibase.h"

#include "miniv3d.h"
#include "miniv4d.h"

#include "minidyna.h"
#include "minimesh.h"
#include "minibspt.h"

#include "minicoord.h"
#include "miniwarp.h"

#include "minilayer.h"

#include "database.h"

class datagrid
   {
   public:

   //! default constructor
   datagrid();

   //! destructor
   virtual ~datagrid();

   //! set coordinate system
   void setcrs(const minicoord::MINICOORD crs=minicoord::MINICOORD_ECEF);

   //! create data brick id
   unsigned int create(const unsigned int slot, // data slot
                       const BOOLINT flip=FALSE, // decomposition flip
                       const BOOLINT bbox=FALSE); // bbox insertion

   //! assign slot
   void assign(const unsigned int id, // data brick id
               const unsigned int slot); // data slot

   //! load data
   void load(const unsigned int id, // data brick id
             const databuf &buf); // data buffer

   //! specify data
   void spec(const unsigned int id, // data brick id
             const minicoord vtx[8], // corner position
             const miniv3d vec[8]); // corner vector

   //! remove data
   void remove(const unsigned int id); // data brick id

   //! move data
   void move(const unsigned int id, // data brick id
             const float swx,const float swy, // SW corner of data brick
             const float nwx,const float nwy, // NW corner of data brick
             const float nex,const float ney, // NE corner of data brick
             const float sex,const float sey, // SE corner of data brick
             const float h0,const float dh,  // base elevation and height of data brick
             const float t0,const float dt); // time frame start and exposure time

   //! clip data
   void clip(const unsigned int id, // data brick id
             const float we1=0.0,const float we2=1.0, // normalized clip coords in west-east direction
             const float sn1=0.0,const float sn2=1.0, // normalized clip coords in south-north direction
             const float bt1=0.0,const float bt2=1.0); // normalized clip coords in bottom-top direction

   //! reference to layer
   void reference(const unsigned int id, // data brick id
                  minilayer *layer); // reference layer

   //! clear all data bricks
   void clear();

   //! check if the data bricks are all clear
   BOOLINT isclear();

   //! specify matrix to be applied before producing a tetrahedral mesh
   void specmtxpre(const miniv4d mtx[3]);

   //! enable usage of the pre matrix
   void usemtxpre(const BOOLINT enable=FALSE);

   //! specify matrix to be applied after producing a tetrahedral mesh
   void specmtxpost(const miniv4d mtx[3]);

   //! enable usage of the post matrix
   void usemtxpost(const BOOLINT enable=FALSE);

   //! construct tetrahedral mesh from all data bricks
   //! needs to be called at least once before pulling the trigger
   void construct();

   //! construct tetrahedral mesh one step at a time
   BOOLINT preprocess();

   //! check if the constructed tetrahedral mesh is empty
   BOOLINT isempty();

   //! check if the grid is below sea level
   BOOLINT isbelowsealevel();

   //! trigger pushing the tetrahedral mesh for a particular time step
   void trigger(const double time);

   //! trigger pushing the tetrahedral mesh for a particular time step and eye point
   void trigger(const double time,
                const minicoord &eye,const miniv3d &dir,
                const float nearp,const float farp,const float fovy,const float aspect,
                const double maxradius=MAXFLOAT,
                const int zcliptexid=0);

   //! push the tetrahedral mesh for a particular time step
   virtual void push(const minimesh &mesh,
                     const double time);

   //! push the tetrahedral mesh for a particular time step and eye point
   virtual void push(const minimesh &mesh,
                     const double time,
                     const miniv3d &eye,const miniv3d &dir,
                     const float nearp,const float farp,const float fovy,const float aspect,
                     const float scale,
                     const int zcliptexid);

   //! get a data brick
   const databuf *getdata(const unsigned int id) const; // data brick id

   protected:

   minicoord::MINICOORD CRS;

   minidyna<BOOLINT> FLAG;
   minidyna<unsigned int> SLOT;
   minidyna<BOOLINT> FLIP;
   minidyna<BOOLINT> BBOX;
   minidyna<miniv3d> CRD1;
   minidyna<miniv3d> CRD2;
   minidyna<minilayer *> REF;
   minidyna<databuf> DATA;

   minidyna<BOOLINT> SPEC;
   minidyna< minidyna<minicoord,8> > VTX;
   minidyna< minidyna<miniv3d,8> > VEC;

   miniv4d MTXPRE[3],MTXPOST[3];
   BOOLINT USEPRE,USEPOST;

   private:

   BOOLINT INVALID;

   minibsptree BSPT;
   BOOLINT CONSTRUCTED;
   minibsptree LAST;

   minimesh MESH;
   BOOLINT DECOMPOSED;
   unsigned int PHASE,STEP;

   minimesh UNSORTED;

   minicoord::MINICOORD getcrs(unsigned int idx);

   BOOLINT decompose();
   minimesh decompose(unsigned int idx);

   minicoord interpolate(unsigned int idx,
                         const miniv3d &crd);

   void push_post(const minimesh &mesh,
                  const double time);

   void push_post(minimesh &mesh,
                  const double time,
                  const miniv3d &eye,const miniv3d &dir,
                  const float nearp,const float farp,const float fovy,const float aspect,
                  const int zcliptexid);
   };

#endif
