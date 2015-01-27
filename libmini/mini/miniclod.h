// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICLOD_H
#define MINICLOD_H

#include "minipath.h"

//! C-LOD core class for geo-referenced paths
class miniclod
   {
   public:

   //! default constructor
   miniclod();

   //! destructor
   virtual ~miniclod();

   //! set path
   void set(const minipath &path);

   //! load path
   void load(ministring filename);

   //! create path geometry
   void create(miniv3d eye, // actual eye point
               double maxdev=1,double atdist=100, // maximum deviation at specific distance
               double maxwidth=1, // maximum width at specific distance
               double minv=0.0,double maxv=30.0,double sat=1.0,double val=1.0, // velocity color mapping
               double weight=1.0, // weight of color mapping
               int update=100); // vertices per update

   // create path geometry incrementally
   void create_inc(miniv3d eye); // actual eye point

   //! get path
   minipath *getpath()
      {return(&path_);}

   protected:

   struct state_struct
      {
      int left;
      int right;
      BOOLINT add;
      };

   struct point_struct
      {
      miniv3d pos;
      miniv3d nrm;
      miniv3d col;
      double wdt;
      };

   minipath path_;

   minidyna<float> dc_; // constant deviations

   minidyna<float> d2_; // geometric deviations
   minidyna<float> dm_; // geometric maximum

   void calcDC();

   float calcD2(int left,int right,int center);
   float calcDM(int left,int right);

   void calcD2();
   float calcD2(int left,int right);

   void addpoint(miniv3d p,double v,BOOLINT start=FALSE);
   BOOLINT subdiv(int left,int right);

   void calcpath();
   void calcpath(int left,int right);

   void calcpath_inc(miniv3d eye,int update=100);
   void calcpath_inc();

   virtual void updated(minidyna<struct point_struct> points) = 0;

   private:

   miniv3d EYE_;
   float C_,D_,W_;

   float MINV_,MAXV_,SAT_,VAL_;
   float WEIGHT_;

   BOOLINT UPDATED_;
   int UPDATE_;

   minidyna<struct state_struct> STACK_;
   minidyna<struct point_struct> POINTS_;
   };

#endif
