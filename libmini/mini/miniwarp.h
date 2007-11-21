// (c) by Stefan Roettger

#ifndef MINIWARP_H
#define MINIWARP_H

#include "minibase.h"

#include "miniv3d.h"
#include "miniv4d.h"

//! geo-referenced coordinates
class minicoord
   {
   public:

   //! generic coordinate systems
   enum MINICOORD
      {
      MINICOORD_NONE,   // undefined
      MINICOORD_ECEF,   // Earth Centered Earth Fixed
      MINICOORD_LLH,    // Lat/Lon/H WGS84
      MINICOORD_UTM,    // Universal Transverse Mercator
      MINICOORD_LINEAR, // linear space
      };

   //! default constructor
   minicoord();

   //! copy constructor
   minicoord(const minicoord &c);

   //! constructors
   minicoord(const miniv3d &v);
   minicoord(const miniv3d &v,const MINICOORD t);
   minicoord(const miniv3d &v,const MINICOORD t,const int zone,const int datum);
   minicoord(const miniv4d &v);
   minicoord(const miniv4d &v,const MINICOORD t);
   minicoord(const miniv4d &v,const MINICOORD t,const int zone,const int datum);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t);
   minicoord(const double cx,const double cy,const double cz,const MINICOORD t,const int zone,const int datum);

   //! destructor
   ~minicoord();

   //! associated operators
   inline minicoord& operator += (const minicoord &v);
   inline minicoord& operator -= (const minicoord &v);
   inline minicoord& operator *= (const double c);
   inline minicoord& operator /= (const double c);

   //! convert from 1 coordinate system 2 another
   void convert2(const MINICOORD t,const int zone=0,const int datum=0);

   //! linear conversion defined by 3x3 matrix and offset
   void convert(const miniv3d mtx[3],const miniv3d offset);

   //! linear conversion defined by 4x3 matrix
   void convert(const miniv4d mtx[3]); // fourth row is assumed to be (0,0,0,1)

   //! non-linear conversion defined by point 2 point correspondences
   void convert(const miniv3d src[2], // bounding box in original domain
                const miniv3d dst[8]); // 8 points in warp domain

   miniv4d vec; // geo-referenced coordinates (plus time)
   MINICOORD type; // actual coordinate system type

   int utm_zone,utm_datum; // actual UTM zone and datum

   protected:

   private:
   };

// associated arithmetic operator +=
inline minicoord& minicoord::operator += (const minicoord &v)
   {
   vec+=v.vec;
   return(*this);
   }

// associated arithmetic operator -=
inline minicoord& minicoord::operator -= (const minicoord &v)
   {
   vec-=v.vec;
   return(*this);
   }

// associated arithmetic operator *=
inline minicoord& minicoord::operator *= (const double c)
   {
   vec*=c;
   return(*this);
   }

// associated arithmetic operator /=
inline minicoord& minicoord::operator /= (const double c)
   {
   vec/=c;
   return(*this);
   }

// arithmetic inline operators
inline minicoord operator + (const minicoord &a,const minicoord &b);
inline minicoord operator - (const minicoord &a,const minicoord &b);
inline minicoord operator - (const minicoord &c);
inline minicoord operator * (const double a,const minicoord &b);
inline minicoord operator * (const minicoord &a,const double b);
inline minicoord operator / (const minicoord &a,const double b);
inline int operator == (const minicoord &a,const minicoord &b);
inline int operator != (const minicoord &a,const minicoord &b);

// output stream operator
inline std::ostream& operator << (std::ostream &out,const minicoord &c);

// implementation of inline operators:

inline minicoord operator + (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec+b.vec,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator - (const minicoord &a,const minicoord &b)
   {return(minicoord(a.vec-b.vec,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator - (const minicoord &c)
   {return(minicoord(-c.vec,c.type,c.utm_zone,c.utm_datum));}

inline minicoord operator * (const double a,const minicoord &b)
   {return(minicoord(a*b.vec,b.type,b.utm_zone,b.utm_datum));}

inline minicoord operator * (const minicoord &a,const double b)
   {return(minicoord(a.vec*b,a.type,a.utm_zone,a.utm_datum));}

inline minicoord operator / (const minicoord &a,const double b)
   {return(minicoord(a.vec/b,a.type,a.utm_zone,a.utm_datum));}

inline int operator == (const minicoord &a,const minicoord &b)
   {return(a.vec==b.vec && a.type==b.type && a.utm_zone==b.utm_zone && a.utm_datum==b.utm_datum);}

inline int operator != (const minicoord &a,const minicoord &b)
   {return(a.vec!=b.vec || a.type!=b.type || a.utm_zone!=b.utm_zone || a.utm_datum!=b.utm_datum);}

inline std::ostream& operator << (std::ostream &out,const minicoord &c)
   {return(out << '(' << c.vec << ',' << c.type << ',' << c.utm_zone << ',' << c.utm_datum << ')');}

//! warp kernel for global coordinate systems
class miniwarp
   {
   public:

   //! global coordinate systems
   enum MINIWARP
      {
      MINIWARP_GLOBAL=0,   // global coordinates
      MINIWARP_METRIC=1,   // metric coordinates
      MINIWARP_PLAIN=2,    // plain coordinates
      MINIWARP_CENTER=3,   // center coordinates
      MINIWARP_DATA=4,     // data coordinates
      MINIWARP_LOCAL=5,    // local coordinates
      MINIWARP_INTERNAL=6, // internal coordinates
      MINIWARP_AFFINE=7,   // affine coordinates
      MINIWARP_TILE=8,     // tile coordinates
      MINIWARP_WARP=9      // warp coordinates
      };

   //! default constructor
   miniwarp();

   //! destructor
   ~miniwarp();

   //! define global coordinates
   void def_global(const minicoord::MINICOORD sysGLB);

   //! define data coordinates
   void def_data(const minicoord bboxDAT[2]);

   //! define conversion to local coordinates
   void def_2local(const miniv3d &offsetLOC,const miniv3d &scalingLOC);

   //! define conversion to affine coordinates
   void def_2affine(const miniv4d mtxAFF[3]); // fourth row is assumed to be (0,0,0,1)

   //! define warp coordinates
   void def_warp(const minicoord::MINICOORD sysWRP);

   //! set actual warp
   void setwarp(MINIWARP from,MINIWARP to);

   //! get actual warp matrix
   void getwarp(miniv4d mtx[3]); // fourth row is assumed to be (0,0,0,1)

   //! get actual inverse transpose warp matrix
   void getinvtra(miniv4d invtra[3]); // fourth row is assumed to be (0,0,0,1)

   //! get actual scaling factor
   double getscale();

   //! get data coordinate system
   minicoord::MINICOORD getsys();

   //! get utm zone
   int getutmzone();

   //! get utm datum
   int getutmdatum();

   //! perform warp of a point
   minicoord warp(const miniv3d &p); // fourth component is assumed to be 1
   minicoord warp(const miniv4d &p); // fourth component is conserved

   //! perform warp of a coordinate
   minicoord warp(const minicoord &c); // fourth component is conserved

   //! perform warp of a vector using the inverse transpose
   miniv3d invtra(const miniv3d &v); // fourth component is assumed to be 1
   miniv4d invtra(const miniv4d &v); // fourth component is conserved

   protected:

   minicoord::MINICOORD SYSGLB;

   minicoord BBOXDAT[2];

   minicoord::MINICOORD SYSDAT;
   int UTMZONE,UTMDATUM;

   miniv3d OFFSETLOC,SCALINGLOC;

   miniv4d MTXAFF[3];

   minicoord::MINICOORD SYSWRP;

   BOOLINT HAS_DATA;

   miniv4d MTX_2PLN[3];
   miniv4d MTX_2CNT[3];
   miniv4d MTX_2DAT[3];
   miniv4d MTX_2LOC[3];
   miniv4d MTX_2INT[3];
   miniv4d MTX_2AFF[3];
   miniv4d MTX_2TIL[3];
   miniv4d MTX_2WRP[3];

   miniv4d INV_2PLN[3];
   miniv4d INV_2CNT[3];
   miniv4d INV_2DAT[3];
   miniv4d INV_2LOC[3];
   miniv4d INV_2INT[3];
   miniv4d INV_2AFF[3];
   miniv4d INV_2TIL[3];
   miniv4d INV_2WRP[3];

   miniv4d MTX_MET2DAT[3];
   miniv4d MTX_DAT2MET[3];

   MINIWARP FROM,TO;

   miniv4d MTX[3];
   miniv4d INVTRA[3];
   float SCALE;

   private:

   void update_mtx();
   void update_wrp();
   void update_inv();
   void update_scl();

   void calc_wrp();

   void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3]);
   void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3]);
   void mlt_mtx(miniv4d mtx[3],const miniv4d mtx1[3],const miniv4d mtx2[3],const miniv4d mtx3[3],const miniv4d mtx4[3]);

   void inv_mtx(miniv3d inv[3],const miniv3d mtx[3]);
   void inv_mtx(miniv4d inv[3],const miniv4d mtx[3]);
   };

#endif
