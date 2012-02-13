// (c) by Stefan Roettger

#ifndef MINICURVE_H
#define MINICURVE_H

#include "minidyna.h"
#include "minicoord.h"

class minicurve : public minidyna<minicoord>
   {
   public:

   minicurve() {}

   void append_sector(const minicoord &p1,const minicoord &p2,
                      int n=0,double maxl=0.0);

   void sort();
   void sample(double maxl=0.0);

   private:

   void bisect(const minicoord &p1,const minicoord &p2,
               int level,int maxlevel);
   };

#endif