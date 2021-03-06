// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINITREE_H
#define MINITREE_H

#include "miniOGL.h"

#include "minitile.h"
#include "minicache.h"
#include "miniwarp.h"

class minitree
   {
   public:

   //! default constructor
   minitree(minicache *cache,minitile *tile);

   //! destructor
   ~minitree();

   //! configuring
   void configure_zfight(float zscale=0.95f); // shift scene towards the viewer to avoid Z-fighting
   void configure_blend(int blend=0); // enable blending of tree textures

   void setmode(int treemode=2);

   //! setup of negative modes
   void setmode_mx(float base,float tr,float tg,float tb,float ta);
   void setmode_m2(const char *texfile,float texsize);

   //! setup of positive modes
   void setmode_x(float res,float sqrfactor,int maxlevel,float minspace,float minheight,float treewidth,float treevariance,float tr,float tg,float tb);
   void setmode_3(float colfluct);
   void setmode_4(const char *texfile_rgb,const char *texfile_a,float treeaspect,float minalpha);
   void setmode_6(float shrubfrac,float shrubprob);
   void setmode_7(const char *texfile_rgb,const char *texfile_a,int treenum);
   void setmode_8(float treestep,float treerand);
   void setmode_9(const char *volfile,float texsize,float grassdepth,float minheight,float baseheight,float maxheight,float treestart,float mipmaprange,int mipmaplevels,float grassalpha);
   void setmode_10(float perturbsize,int perturbres,int perturbstart,float perturbpers,float perturbseed,float perturbfx);
   void setmode_12(float redwgt,float greenwgt,float bluewgt,float alphathres,float alphaslope);

   //! return actual tree mode
   int getmode();

   //! return actual number of cached trees
   int gettrees();

   protected:

   minicache *CACHE;
   int ATTACHED_ID;

   int TREEMODE;

   private:

   struct TREECACHE_STRUCT
      {
      float *buf,*crd;
      int size,maxsize;

      int treecnt;

      float *grass_buf,*grass_crd;
      int grass_size,grass_maxsize;
      };

   typedef TREECACHE_STRUCT TREECACHE_TYPE;

   int PRISM_ID;

   int TREECACHE_NUM;

   TREECACHE_TYPE TREECACHE[2];

   float TREECACHE_LAMBDA;

   int TREECACHE_COUNT;

   float TREECACHE_X1,TREECACHE_Y1,TREECACHE_Z1,TREECACHE_H1;
   float TREECACHE_X2,TREECACHE_Y2,TREECACHE_Z2,TREECACHE_H2;

   float TREECACHE_EX,TREECACHE_EY,TREECACHE_EZ;

   int TREE_TEXID,GRASS_TEXID,GRASS_PERTURBID,RENDER_TEXID;

   int TREE_VTXPROGID1,TREE_VTXPROGID2,RENDER_VTXPROGID;
   int GRASS_VTXPROGID,GRASS_FRGPROGID1,GRASS_FRGPROGID2,GRASS_FRGPROGID3,GRASS_FRGPROGID4;
   int RENDER_FRGPROGID1,RENDER_FRGPROGID2;

   int GRASS_CLASSES;

   static void prismedge(float x,float y,float yf,float z,void *data);
   static void prismcache(int phase,float scale,float ex,float ey,float ez,void *data);
   static int prismrender(float *cache,int cnt,float lambda,miniwarp *warp,void *data);
   static int prismtrigger(int phase,void *data);
   static void prismsync(int id,void *data);

   void treeedge(float x,float y,float yf,float z);
   void treecache(int phase,float scale,float ex,float ey,float ez);
   int treetrigger(int phase);
   void treesync(int id);

   void treedata(float x1,float y1,float z1,float h1,
                 float x2,float y2,float z2,float h2,
                 float x3,float y3,float z3,float h3,
                 int level=0);

   inline void cachedata(float x,float y,float z);
   inline void cachedata(float x,float y,float z,float s,float t,float r=0.0f);
   inline void cachegrass(float x,float y,float z,float s,float t,float r=0.0f);

   int rendertrees(float *cache,float *coords,int cnt,miniwarpbase *warp,
                   float tr=0.0f,float tg=0.25f,float tb=0.05f);

   int rendergrass(float *cache,float *coords,int cnt,miniwarpbase *warp);

   int renderprisms(float *cache,int cnt,float lambda,miniwarpbase *warp,
                    float tr=0.0f,float tg=0.25f,float tb=0.05f,float ta=0.9f);

   unsigned char *build3Dmipmap(unsigned char *volume,
                                int width,int height,int depth,int components,
                                int levels);

   unsigned char *shrink3Dtexture(unsigned char *volume,
                                  int width,int height,int depth,int components);

   void put3Dtexture(unsigned char *texture,int width,int height,int depth,int components,
                     unsigned char *volume,int width2,int height2);

   inline void put3Dcolor(unsigned char *color,int components,
                          unsigned char *slice,int width,int height,
                          float s,float t);

   float pn_getrandom(float seed=-1.0f);
   float pn_interpolateC(float v0,float v1,float v2,float v3,float x);
   float pn_interpolate2DC(float *octave,int size,float c1,float c2);
   unsigned char *pn_perlin2D(int size,int start,float persist=0.5f,float seed=-1.0f);

   float CONFIGURE_ZSCALE;
   int CONFIGURE_BLEND;

   // parameters for negative modes:

   float TREEMODE_MX_BASE;
   float TREEMODE_MX_TR;
   float TREEMODE_MX_TG;
   float TREEMODE_MX_TB;
   float TREEMODE_MX_TA;

   // parameters for mode -2:

   char *TREEMODE_M2_TEXFILE;
   float TREEMODE_M2_TEXSIZE;

   // parameters for positive modes:

   float TREEMODE_X_RES;
   float TREEMODE_X_SQRFACTOR;
   int TREEMODE_X_MAXLEVEL;
   float TREEMODE_X_MINSPACE;
   float TREEMODE_X_MINHEIGHT;
   float TREEMODE_X_TREEWIDTH;
   float TREEMODE_X_TREEVARIANCE;
   float TREEMODE_X_TR;
   float TREEMODE_X_TG;
   float TREEMODE_X_TB;

   // parameters for mode 3:

   float TREEMODE_3_COLFLUCT;

   // parameters for mode 4+:

   char *TREEMODE_4_TEXFILE_RGB;
   char *TREEMODE_4_TEXFILE_A;
   float TREEMODE_4_TREEASPECT;
   float TREEMODE_4_MINALPHA;

   // parameters for mode 6+:

   float TREEMODE_6_SHRUBFRAC;
   float TREEMODE_6_SHRUBPROB;

   // parameters for mode 7+:

   char *TREEMODE_7_TEXFILE_RGB;
   char *TREEMODE_7_TEXFILE_A;
   int TREEMODE_7_TREENUM;

   // parameters for mode 8+:

   float TREEMODE_8_TREESTEP;
   float TREEMODE_8_TREERAND;

   // parameters for mode 9+:

   char *TREEMODE_9_VOLFILE;
   float TREEMODE_9_TEXSIZE;
   float TREEMODE_9_GRASSDEPTH;
   float TREEMODE_9_MINHEIGHT;
   float TREEMODE_9_BASEHEIGHT;
   float TREEMODE_9_MAXHEIGHT;
   float TREEMODE_9_TREESTART;
   float TREEMODE_9_MIPMAPRANGE;
   int TREEMODE_9_MIPMAPLEVELS;
   float TREEMODE_9_GRASSALPHA;

   // parameters for mode 10+:

   float TREEMODE_10_PERTURBSIZE;
   int TREEMODE_10_PERTURBRES;
   int TREEMODE_10_PERTURBSTART;
   float TREEMODE_10_PERTURBPERS;
   float TREEMODE_10_PERTURBSEED;
   float TREEMODE_10_PERTURBFX;

   // parameters for mode 12:

   float TREEMODE_12_REDWGT;
   float TREEMODE_12_GREENWGT;
   float TREEMODE_12_BLUEWGT;
   float TREEMODE_12_ALPHATHRES;
   float TREEMODE_12_ALPHASLOPE;
   };

#endif
