// (c) by Stefan Roettger

// configuration section:

#define VIEWER_WINWIDTH 768
#define VIEWER_WINHEIGHT 512

#define VIEWER_WINTITLE "libMini Viewer"

#define VIEWER_FPS 25.0f

#define VIEWER_SCALE 100.0f
#define VIEWER_EXAGGER 1.0f

#define VIEWER_FOVY 60.0f
#define VIEWER_NEARP 10.0f
#define VIEWER_FARP 10000.0f

#define VIEWER_RES 1.0E3f
#define VIEWER_RANGE 0.001f

#define VIEWER_SEALEVEL -MAXFLOAT

#define VIEWER_UPLIFT 0.1f

#define VIEWER_MAXSPEED 50.0f
#define VIEWER_GRAVITY 9.81f
#define VIEWER_HOVER 1.81f

#define VIEWER_JUMP 20.0f
#define VIEWER_DAMP 10.0f
#define VIEWER_BOUNCE 5.0f

#define VIEWER_FOGSTART 0.5f
#define VIEWER_FOGDENSITY 0.5f;

#define VIEWER_CONTOURS 10.0f

#define VIEWER_SEABOTTOM -10.0f

#define VIEWER_SIGNPOSTHEIGHT 100.0f
#define VIEWER_SIGNPOSTRANGE 0.25f

#define VIEWER_BRICKSIZE 100.0f
#define VIEWER_BRICKRAD 1000.0f

#define VIEWER_BRICKSCROLL 0.5f

#define VIEWER_SAVFILE "settings.sav"

#define VIEWER_BATHYSTART 0.0f
#define VIEWER_BATHYEND -10.0f

#define VIEWER_BATHYWIDTH 256
#define VIEWER_BATHYHEIGHT 2
#define VIEWER_BATHYCOMPS 4

static unsigned char VIEWER_BATHYMAP[VIEWER_BATHYWIDTH*4*2];

#define VIEWER_NPRBATHYSTART 0.0f
#define VIEWER_NPRBATHYEND 1000.0f

#define VIEWER_NPRBATHYWIDTH 1024
#define VIEWER_NPRBATHYHEIGHT 2
#define VIEWER_NPRBATHYCOMPS 4

static unsigned char VIEWER_NPRBATHYMAP[VIEWER_NPRBATHYWIDTH*4*2];

#define VIEWER_NPRCONTOURS 100.0f

#define VIEWER_SBASE 0.5f

// main section:

#include <mini/minibase.h>

#include <mini/miniOGL.h>

#include <mini/minitile.h>
#include <mini/minitext.h>

#include <mini/miniv3d.h>
#include <mini/minihsv.h>

#include <mini/viewerbase.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

// maximum string length
#define MAXSTR 1000

// short usage: url to the tiles and textures
static char shorturl[MAXSTR]="";

// long usage: base url and id-path to the tiles and textures
static char baseurl[MAXSTR]="";
static char baseid[MAXSTR]="";

// sub-path to the tiles and textures
static char basepath1[MAXSTR]=""; // elevation
static char basepath2[MAXSTR]=""; // imagery

// window size
static int winwidth,winheight,winid;

// the viewer base class and its parameters
static viewerbase *viewer=NULL;
static viewerbase::VIEWER_PARAMS *params=NULL;

// eye point
static minicoord eye;
static double dez,aez;

// viewing angles
static double angle,turn,pitch,incline;

// viewing direction
static miniv3d dir,up,right;

// gliding parameters
static double maxspeed=VIEWER_MAXSPEED,speedinc=0.1,accel=0.1,gravity=0.0,hover=VIEWER_HOVER;

// jumping parameters
static double jump=VIEWER_JUMP,damp=VIEWER_DAMP,bounce=VIEWER_BOUNCE;

// steering parameters
static double oneturn=5.0,oneincline=10.0;

// gliding speed
static double speed,topspeed;

// stereo base
static const float sbase=VIEWER_SBASE;

// consumed time per frame
static double accu_delta=0.0;
static double accu_idle=0.0;
static double avg_delta=0.0;
static double avg_idle=1.0/VIEWER_FPS;
static int avg_count=0;

// command line switches
static int sw_stereo=0;
static int sw_anaglyph=0;
static int sw_full=0;
static int sw_pnm=0;
static int sw_reset=0;
static int sw_autos3tc=0;
static int sw_bricks=0;
static int sw_mpass=0;

// user switches
static int sw_hud=0;
static int sw_hide=0;
static int sw_cross=0;

// initialize the viewing parameters
void initparams()
   {
   viewerbase::VIEWER_PARAMS prms;

   viewer->get(prms);

   prms.fps=VIEWER_FPS;
   prms.scale=VIEWER_SCALE;
   prms.exaggeration=VIEWER_EXAGGER;
   prms.fovy=VIEWER_FOVY;
   prms.nearp=VIEWER_NEARP;
   prms.farp=VIEWER_FARP;
   prms.res=VIEWER_RES;
   prms.range=VIEWER_RANGE;
   prms.sealevel=VIEWER_SEALEVEL;

   if (sw_autos3tc!=0) prms.autocompress=TRUE;

   prms.usefog=TRUE;
   prms.useshaders=TRUE;
   prms.usebathymap=FALSE;
   prms.usecontours=FALSE;
   prms.usewireframe=FALSE;
   prms.useskydome=TRUE;
   prms.usewaypoints=TRUE;
   prms.usebricks=FALSE;
   prms.useearth=FALSE;

   prms.fogstart=VIEWER_FOGSTART;
   prms.fogdensity=VIEWER_FOGDENSITY;

   prms.contours=VIEWER_CONTOURS;
   prms.seabottom=VIEWER_SEABOTTOM;

   prms.bathystart=VIEWER_BATHYSTART;
   prms.bathyend=VIEWER_BATHYEND;

   prms.bathymap=VIEWER_BATHYMAP;
   prms.bathywidth=VIEWER_BATHYWIDTH;
   prms.bathyheight=VIEWER_BATHYHEIGHT;
   prms.bathycomps=VIEWER_BATHYCOMPS;

   prms.nprbathystart=VIEWER_NPRBATHYSTART;
   prms.nprbathyend=VIEWER_NPRBATHYEND;

   prms.nprbathymap=VIEWER_NPRBATHYMAP;
   prms.nprbathywidth=VIEWER_NPRBATHYWIDTH;
   prms.nprbathyheight=VIEWER_NPRBATHYHEIGHT;
   prms.nprbathycomps=VIEWER_NPRBATHYCOMPS;

   prms.nprcontours=VIEWER_NPRCONTOURS;

   prms.signpostheight=VIEWER_SIGNPOSTHEIGHT;
   prms.signpostrange=VIEWER_SIGNPOSTRANGE;

   if (sw_bricks!=0) prms.usebricks=TRUE;

   prms.bricksize=VIEWER_BRICKSIZE;
   prms.brickrad=VIEWER_BRICKRAD;

   if (sw_mpass!=0) prms.brickpasses=2;
   else prms.brickpasses=1;

   prms.brickscroll=VIEWER_BRICKSCROLL;

   viewer->set(prms);
   }

// initialize the render window
void initwindow(int width,int height)
   {
   viewerbase::VIEWER_PARAMS prms;

   if (sw_full==0)
      {
      winwidth=width;
      winheight=height;
      }
   else
      {
      viewer->get(prms);

      winwidth=glutGameModeGet(GLUT_GAME_MODE_WIDTH);
      winheight=glutGameModeGet(GLUT_GAME_MODE_HEIGHT);

      prms.fps=glutGameModeGet(GLUT_GAME_MODE_REFRESH_RATE);

      if (winwidth<=0 || winheight<=0 || prms.fps<=0.0f)
         {
         winwidth=width;
         winheight=height;

         prms.fps=VIEWER_FPS;
         }

      viewer->set(prms);
      }

   if (winwidth<1) winwidth=1;
   if (winheight<1) winheight=1;

   viewer->setwinsize(winwidth,winheight);

   glViewport(0,0,winwidth,winheight);
   }

// initialize the view point
void initview(minicoord e,double a,double p,double dh=0.0)
   {
   minicoord el;
   double elev;

   initwindow(winwidth,winheight);

   eye=e;

   elev=viewer->getheight(eye);

   el=viewer->map_e2l(eye);

   if (elev!=-MAXFLOAT) el.vec.z=FMAX(el.vec.z,viewer->len_e2l(elev+hover+dh));

   eye=viewer->map_l2e(el);

   viewer->initeyepoint(eye);

   dez=aez=0.0;

   angle=turn=a;
   pitch=incline=p;

   speed=topspeed=0.0;
   }

// load settings
void loadsettings()
   {
   FILE *file;

   miniv3f e;
   minicoord::MINICOORD type;

   float a,p;

   viewerbase::VIEWER_PARAMS prms;

   int flag;

   char *savname=viewer->getcache()->getfile(VIEWER_SAVFILE);

   if (savname!=NULL)
      {
      viewer->get(prms);

      if ((file=fopen(savname,"rb"))==NULL) ERRORMSG();
      free(savname);

      // load essential parameters:

      if (fscanf(file,"ex=%f\n",&e.x)!=1) ERRORMSG();
      if (fscanf(file,"ey=%f\n",&e.y)!=1) ERRORMSG();
      if (fscanf(file,"ez=%f\n",&e.z)!=1) ERRORMSG();

      if (fscanf(file,"type=%d\n",&type)!=1) ERRORMSG();

      if (fscanf(file,"angle=%f\n",&a)!=1) ERRORMSG();
      if (fscanf(file,"pitch=%f\n",&p)!=1) ERRORMSG();

      if (fscanf(file,"farp=%f\n",&prms.farp)!=1) ERRORMSG();
      if (fscanf(file,"res=%f\n",&prms.res)!=1) ERRORMSG();
      if (fscanf(file,"range=%f\n",&prms.range)!=1) ERRORMSG();

      if (fscanf(file,"fovy=%f\n",&prms.fovy)!=1) ERRORMSG();

      if (fscanf(file,"sealevel=%g\n",&prms.sealevel)!=1) ERRORMSG();
      if (prms.sealevel<-MAXFLOAT/2) prms.sealevel=-MAXFLOAT;

      // load optional parameters:

      if (fscanf(file,"usefog=%d\n",&flag)!=1) ERRORMSG();
      prms.usefog=flag;

      if (fscanf(file,"usecontours=%d\n",&flag)!=1) ERRORMSG();
      prms.usecontours=flag;

      if (fscanf(file,"useskydome=%d\n",&flag)!=1) ERRORMSG();
      prms.useskydome=flag;

      if (fscanf(file,"usewaypoints=%d\n",&flag)!=1) ERRORMSG();
      prms.usewaypoints=flag;

      if (fscanf(file,"usebricks=%d\n",&flag)!=1) ERRORMSG();
      prms.usebricks=flag;

      if (fscanf(file,"fogdensity=%f\n",&prms.fogdensity)!=1) ERRORMSG();

      fclose(file);

      viewer->set(prms);

      initview(minicoord(miniv4d(e),type),a,p);
      }
   }

// save settings
void savesettings()
   {
   FILE *file;

   char *savname=viewer->getcache()->getfile(VIEWER_SAVFILE);

   if (savname==NULL) savname=strdup(VIEWER_SAVFILE);

   if ((file=fopen(savname,"wb"))==NULL) ERRORMSG();
   free(savname);

   // save essential parameters:

   fprintf(file,"ex=%f\n",eye.vec.x);
   fprintf(file,"ey=%f\n",eye.vec.y);
   fprintf(file,"ez=%f\n",eye.vec.z);

   fprintf(file,"type=%d\n",eye.type);

   fprintf(file,"angle=%f\n",angle);
   fprintf(file,"pitch=%f\n",pitch);

   fprintf(file,"farp=%f\n",params->farp);
   fprintf(file,"res=%f\n",params->res);
   fprintf(file,"range=%f\n",params->range);

   fprintf(file,"fovy=%f\n",params->fovy);

   fprintf(file,"sealevel=%g\n",params->sealevel);

   // save optional parameters:

   fprintf(file,"usefog=%d\n",params->usefog);
   fprintf(file,"usecontours=%d\n",params->usecontours);
   fprintf(file,"useskydome=%d\n",params->useskydome);
   fprintf(file,"usewaypoints=%d\n",params->usewaypoints);
   fprintf(file,"usebricks=%d\n",params->usebricks);

   fprintf(file,"fogdensity=%f\n",params->fogdensity);

   fclose(file);
   }

// initialize VIS bathy map
void initVISbathymap()
   {
   int i;

   float t;

   float rgba[4];

   static const float hue1=0.0f;
   static const float hue2=240.0f;

   for (i=0; i<VIEWER_BATHYWIDTH; i++)
      {
      t=(float)i/(VIEWER_BATHYWIDTH-1);

      hsv2rgb(hue1+(hue2-hue1)*t,1.0f,1.0f,rgba);

      rgba[3]=fsqrt(1.0f-fabs(2.0f*(t-0.5f)));

      VIEWER_BATHYMAP[4*i]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)]=ftrc(255.0f*rgba[0]+0.5f);
      VIEWER_BATHYMAP[4*i+1]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+1]=ftrc(255.0f*rgba[1]+0.5f);
      VIEWER_BATHYMAP[4*i+2]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+2]=ftrc(255.0f*rgba[2]+0.5f);
      VIEWER_BATHYMAP[4*i+3]=VIEWER_BATHYMAP[4*(i+VIEWER_BATHYWIDTH)+3]=ftrc(255.0f*rgba[3]+0.5f);
      }
   }

// initialize NPR bathy map
void initNPRbathymap()
   {
   int i;

   float t;
   float alpha;

   float rgba[4];

   static const float hue1=120.0f;
   static const float hue2=60.0f;

   static const float hue_ctr=60.0f;
   static const float sat_ctr=0.75f;
   static const float val_ctr=0.25f;

   for (i=0; i<VIEWER_NPRBATHYWIDTH; i++)
      {
      t=(float)i/(VIEWER_NPRBATHYWIDTH-1);

      alpha=t*fabs(VIEWER_NPRBATHYEND-VIEWER_NPRBATHYSTART)/VIEWER_CONTOURS;
      alpha=alpha-ftrc(alpha);

      if (t<0.5f) hsv2rgb(hue1+(hue2-hue1)*t,t,1.0f,rgba);
      else hsv2rgb(hue1+(hue2-hue1)*t,1.0f-t,1.0f,rgba);

      rgba[3]=0.5f;

      if (alpha>0.9f) hsv2rgb(hue_ctr,sat_ctr,val_ctr,rgba);

      VIEWER_NPRBATHYMAP[4*i]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)]=ftrc(255.0f*rgba[0]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+1]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+1]=ftrc(255.0f*rgba[1]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+2]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+2]=ftrc(255.0f*rgba[2]+0.5f);
      VIEWER_NPRBATHYMAP[4*i+3]=VIEWER_NPRBATHYMAP[4*(i+VIEWER_NPRBATHYWIDTH)+3]=ftrc(255.0f*rgba[3]+0.5f);
      }
   }

// render compass
void rendercompass()
   {
   const float sx=0.05f;
   const float sy=0.05f;

   const float xr=0.15f;
   const float yr=0.15f;

   const float alpha=0.5f;

   glLoadIdentity();
   glTranslatef(sx,sy,0.0f);
   glScalef(xr,yr,0.0f);
   glTranslatef(0.5f,0.5f,0.0f);
   glRotatef(turn,0.0f,0.0f,1.0f);

   glColor4f(0.5f,0.75f,1.0f,alpha);

   glBegin(GL_TRIANGLE_FAN);
   glVertex2f(0.0f,0.5f);
   glVertex2f(0.1f,-0.5f);
   glVertex2f(0.0f,-0.4f);
   glVertex2f(-0.1f,-0.5f);
   glEnd();

   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.5f,1.0f,1.0f,"North");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f," ne ");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f,"East");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f," se ");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,0.0f,0.5f,1.0f,1.0f,"South");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f," sw ");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f,"West");
   glTranslatef(0.15f,-0.55f,0.0f);
   glRotatef(-45.0f,0.0f,0.0f,1.0f);
   glTranslatef(-0.15f,0.55f,0.0f);
   minitext::drawstring(0.3f,120.0f,0.0f,1.0f,1.0f," nw ");
   }

// render tileset info
void renderinfo()
   {
   int i,j,t;

   const float sx=0.6f;
   const float sy=0.02f;

   const float xr=0.38f;
   const float yr=0.38f;

   const float size=0.3f;
   const float alpha=0.5f;

   minitile *mt=viewer->getterrain()->getminitile();

   int vcol=mt->getvisibleleft();
   int vrow=mt->getvisiblebottom();

   int vcols=mt->getvisibleright()-mt->getvisibleleft()+1;
   int vrows=mt->getvisibletop()-mt->getvisiblebottom()+1;

   int pcol=mt->getpreloadedleft();
   int prow=mt->getpreloadedbottom();

   int pcols=mt->getpreloadedright()-mt->getpreloadedleft()+1;
   int prows=mt->getpreloadedtop()-mt->getpreloadedbottom()+1;

   glLoadIdentity();
   glTranslatef(sx,sy+yr,0.0f);
   glScalef(xr/pcols,-yr/prows,0.0f);
   glTranslatef(0.5f,0.5f,0.0f);

   glColor4f(1.0f,1.0f,1.0f,0.25f);

   glBegin(GL_QUADS);
   glVertex2f(-size,-size);
   glVertex2f(pcols-1+size,-size);
   glVertex2f(pcols-1+size,prows-1+size);
   glVertex2f(-size,prows-1+size);
   glEnd();

   glColor4f(1.0f,1.0f,1.0f,0.75f);

   glBegin(GL_QUADS);
   glVertex2f(vcol-pcol-size,vrow-prow-size);
   glVertex2f(vcol-pcol+vcols-1+size,vrow-prow-size);
   glVertex2f(vcol-pcol+vcols-1+size,vrow-prow+vrows-1+size);
   glVertex2f(vcol-pcol-size,vrow-prow+vrows-1+size);
   glEnd();

   glBegin(GL_QUADS);

   for (i=0; i<pcols; i++)
      for (j=0; j<prows; j++)
         {
         t=mt->gettexw(pcol+i,prow+j);

         switch (t)
            {
            case 0: glColor4f(0.0f,0.0f,0.0f,alpha); break;
            case 2: glColor4f(0.1f,0.1f,0.1f,alpha); break;
            case 4: glColor4f(0.25f,0.25f,0.25f,alpha); break;
            case 8: glColor4f(0.5f,0.5f,0.5f,alpha); break;
            case 16: glColor4f(0.75f,0.75f,0.75f,alpha); break;
            case 32: glColor4f(1.0f,1.0f,1.0f,alpha); break;
            case 64: glColor4f(0.0f,0.0f,1.0f,alpha); break;
            case 128: glColor4f(0.0f,0.5f,1.0f,alpha); break;
            case 256: glColor4f(0.0f,1.0f,0.0f,alpha); break;
            case 512: glColor4f(0.5f,1.0f,0.0f,alpha); break;
            case 1024: glColor4f(1.0f,1.0f,0.0f,alpha); break;
            case 2048: glColor4f(1.0f,0.5f,0.0f,alpha); break;
            default: glColor4f(1.0f,0.0f,0.0f,alpha); break;
            }

         glVertex2f(i-size,j-size);
         glVertex2f(i+size,j-size);
         glVertex2f(i+size,j+size);
         glVertex2f(i-size,j+size);
         }

   glEnd();
   }

// render head-up display
void renderhud()
   {
   double elev,sea;

   double dist;

   char str[MAXSTR];

   minitile *mt=viewer->getterrain()->getminitile();

   minitext::configure_zfight(1.0f);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0.0f,1.0f,0.0f,1.0f,-1.0f,1.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   if (sw_hud)
      {
      glTranslatef(0.0f,0.675f,0.0f);

      glColor4f(1.0f,1.0f,1.0f,0.5f);
      glBegin(GL_QUADS);
      glVertex2f(0.0f,0.0f);
      glVertex2f(1.0f,0.0f);
      glVertex2f(1.0f,0.3f);
      glVertex2f(0.0f,0.3f);
      glEnd();

      glTranslatef(0.033f,0.0f,0.0f);

      elev=viewer->getheight(eye);
      if (elev==-MAXFLOAT) elev=0.0f;

      sea=params->sealevel;
      if (sea==-MAXFLOAT) sea=0.0f;

      snprintf(str,MAXSTR,"Position:                \n\n x= %11.1f\n y= %11.1f\n z= %11.1fm (%.1fm)\n\n dir= %.1f\n yon= %.1f\n\nSettings:\n\n farp= %.1fm (f/F)\n\n res=   %.1f (t/T)\n range= %.1fm (r/R)\n\n sea= %.1f (u/U)\n\n gravity= %.1f (g)\n",
               eye.vec.x,eye.vec.y,eye.vec.z,elev/params->exaggeration,turn,incline, // position/elevation and direction
               params->farp,params->res,params->range*params->farp,sea,gravity); // adjustable parameters

      minitext::drawstring(0.3f,240.0f,1.0f,0.25f,1.0f,str);

      glTranslatef(0.3f,0.0f,0.0f);

      snprintf(str,MAXSTR,"Tile Set:                \n\n vis area= [%d-%d]x[%d-%d]\n\n fps= %.1fHz (%.1f%%)\n\n mem= %.1fMB\n tex= %.1fMB (%.1fMB)\n\nStreaming:\n\n pending= %d\n\n cache= %.1fMB\n\nGeometry:\n\n fans=     %d\n vertices= %d\n",
               mt->getvisibleleft(),mt->getvisibleright(),mt->getvisiblebottom(),mt->getvisibletop(), // visible area of tileset
               1.0/(avg_delta+avg_idle),100*(1.0-avg_idle*params->fps), // actual frame rate and load
               viewer->getterrain()->getmem(),viewer->getterrain()->gettexmem()/6,viewer->getterrain()->gettexmem(), // memory consumed by tileset
               viewer->getcache()->getpending(), // number of pending tiles
               viewer->getcache()->getmem(), // memory foot print of cache
               viewer->getbuffer()->getfancnt(), // rendered triangles fans
               viewer->getbuffer()->getvtxcnt()); // rendered vertices

      minitext::drawstring(0.3f,240.0f,1.0f,0.25f,1.0f,str);

      glTranslatef(0.3f,0.0f,0.0f);

      snprintf(str,MAXSTR,"Controls:                \n\n left=       a\n right=      d\n accelerate= w\n decelerate= s\n\n stop=      spc\n jump=      j\n look down= <\n look up=   >\n\n hud/hide=  h/H\n crosshair= c\n waypoints= p\n wireframe= l\n\n load/save= o/O\n quit=      Q esc\n");

      minitext::drawstring(0.3f,240.0f,1.0f,0.25f,1.0f,str);

      if (sw_cross!=0 || incline>89.9f)
         {
         glLoadIdentity();
         glTranslatef(0.5f,0.5f,0.0f);
         glScalef((float)winheight/winwidth,1.0f,1.0f);
         glColor3f(0.0f,0.0f,0.0f);
         glBegin(GL_LINES);
         glVertex2f(-0.025,-0.025f);
         glVertex2f(0.025,0.025f);
         glVertex2f(-0.025,0.025f);
         glVertex2f(0.025,-0.025f);
         glEnd();

         if (sw_cross!=0)
            {
            dist=viewer->shoot(eye,dir);

            if (dist!=MAXFLOAT)
               {
               snprintf(str,MAXSTR,"dist=%3.3f elev=%3.3f",dist,viewer->getheight(eye+dir*dist));

               glTranslatef(0.05f,0.0f,0.0f);
               minitext::drawstring(0.3f,240.0f,1.0f,0.25f,1.0f,str);
               }
            }
         }

      rendercompass();
      renderinfo();
      }
   else
      if (sw_hide==0)
         {
         glTranslatef(0.033f,0.95f,0.0f);
         minitext::drawstring(0.3f,240.0f,1.0f,0.25f,1.0f,"[Press h for HUD]",1.0f,0.25f);

         glLoadIdentity();
         rendercompass();
         }

   glDisable(GL_BLEND);
   }

// GLUT display function
void displayfunc()
   {
   double delta,idle;

   double sina,cosa;
   double sinp,cosp;

   double elev,coef;

   minicoord ep,el,ei;
   miniv3d di,ui,ri;

   // start timer
   viewer->starttimer();

   // update eye point:

   el=viewer->map_e2l(eye);

   sina=sin(2.0*PI/360.0*turn);
   cosa=cos(2.0*PI/360.0*turn);

   sinp=sin(2.0*PI/360.0*incline);
   cosp=cos(2.0*PI/360.0*incline);

   el.vec.x+=sina*speed/params->fps;
   el.vec.y+=cosa*speed/params->fps;

   ep=viewer->map_l2e(el);

   elev=viewer->len_e2l(viewer->getheight(ep));

   dir.x=sina*cosp;
   dir.y=cosa*cosp;
   dir.z=-sinp;

   up.x=sina*sinp;
   up.y=cosa*sinp;
   up.z=cosp;

   right.x=cosa*sbase;
   right.y=sina*sbase;
   right.z=0.0;

   // update eye movement:

   speed+=accel*(topspeed-speed);

   turn+=accel*(angle-turn);
   incline+=accel*(pitch-incline);

   coef=(el.vec.z-elev)/hover-1.0;
   if (coef>1.0) coef=1.0;
   else if (coef<-1.0) coef=-1.0;

   aez=-coef*gravity;
   aez*=1.0-FABS(dez/maxspeed);

   dez+=aez/params->fps;
   dez*=pow(1.0/(1.0+damp),1.0/params->fps);

   el.vec.z+=dez/params->fps;

   if (el.vec.z<elev+hover)
      {
      dez=-dez;
      dez*=1.0/(1.0+bounce);
      el.vec.z=elev+hover;
      }

   eye=viewer->map_l2e(el);

   ei=viewer->map_e2i(eye);
   di=viewer->rot_l2i(dir,eye);
   ui=viewer->rot_l2i(up,eye);
   ri=viewer->rot_l2i(right,eye);

   params->signpostturn=turn;
   params->signpostincline=-incline;

   // setup OpenGL state:

   glClearColor(params->fogcolor[0],params->fogcolor[1],params->fogcolor[2],1.0f);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(params->fovy,(float)winwidth/winheight,viewer->len_e2i(params->nearp),viewer->len_e2i(params->farp));

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(ei.vec.x,ei.vec.y,ei.vec.z,ei.vec.x+di.x,ei.vec.y+di.y,ei.vec.z+di.z,ui.x,ui.y,ui.z);

   // update vertex arrays
   viewer->cache(eye,dir,up);

   // render scene
   if (sw_stereo==0) viewer->render(); // render vertex arrays
   else
      {
      // left channel:

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(ei.vec.x-ri.x,ei.vec.y-ri.y,ei.vec.z-ri.z,ei.vec.x+di.x-ri.x,ei.vec.y+di.y-ri.y,ei.vec.z+di.z-ri.z,ui.x,ui.y,ui.z);

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK_LEFT);
      else glColorMask(GL_TRUE,GL_FALSE,GL_FALSE,GL_FALSE);

      viewer->render();

      // right channel:

      glClear(GL_DEPTH_BUFFER_BIT);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      gluLookAt(ei.vec.x+ri.x,ei.vec.y+ri.y,ei.vec.z+ri.z,ei.vec.x+di.x+ri.x,ei.vec.y+di.y+ri.y,ei.vec.z+di.z+ri.z,ui.x,ui.y,ui.z);

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK_RIGHT);
      else glColorMask(GL_FALSE,GL_TRUE,GL_TRUE,GL_FALSE);

      viewer->render();

      if (sw_anaglyph==0) glDrawBuffer(GL_BACK);
      else glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_FALSE);
      }

   // render the head-up display
   renderhud();

   // swap buffers and wait for next frame:

   glutSwapBuffers();

   // get time spent
   delta=viewer->gettimer();
   idle=1.0/params->fps-delta;

   // idle for the remainder of the frame
   viewer->idle(delta);

   // update statistics:

   accu_delta+=delta;
   if (idle>0.0) accu_idle+=idle;

   if (++avg_count>params->fps)
      {
      avg_delta=accu_delta/avg_count;
      avg_idle=accu_idle/avg_count;
      accu_delta=0.0;
      accu_idle=0.0f;
      avg_count=0;
      }
   }

// GLUT reshape function
void reshapefunc(int width,int height)
   {initwindow(width,height);}

// GLUT keyboard function
void keyboardfunc(unsigned char key,int x,int y)
   {
   float mousex,mousey;

   mousex=(float)x/(winwidth-1);
   mousey=(float)y/(winwidth-1);

   switch (key)
      {
      case ' ':
         if (topspeed==0.0) topspeed=speedinc*maxspeed;
         else topspeed=0.0;
         break;
      case 'w':
      case 'W':
         topspeed+=speedinc*maxspeed;
         if (topspeed>maxspeed) topspeed=maxspeed;
         break;
      case 'a':
      case 'A':
         angle-=oneturn;
         if (angle<0.0)
            {
            angle+=360.0;
            turn+=360.0;
            }
         break;
      case 'd':
      case 'D':
         angle+=oneturn;
         if (angle>360.0)
            {
            angle-=360.0;
            turn-=360.0;
            }
         break;
      case 's':
      case 'S':
         topspeed-=speedinc*maxspeed;
         if (topspeed<-maxspeed) topspeed=-maxspeed;
         break;
      case '<':
         pitch+=oneincline;
         if (pitch>90.0) pitch=90.0;
         break;
      case '>':
         pitch-=oneincline;
         if (pitch<-90.0) pitch=-90.0;
         break;
      case 'j':
      case 'J':
         dez=jump;
         break;
      case 'g':
      case 'G':
         if (gravity==0.0) gravity=VIEWER_GRAVITY;
         else gravity=0.0;
         break;
      case 't':
         params->res/=1.1f;
         if (params->res<1.0f) params->res=1.0f;
         viewer->propagate();
         break;
      case 'T':
         params->res*=1.1f;
         if (params->res>1.0E10f) params->res=1.0E10f;
         viewer->propagate();
         break;
      case 'r':
         params->range/=1.1f;
         if (params->range<1.0E-5f) params->range=1.0E-5f;
         viewer->propagate();
         break;
      case 'R':
         params->range*=1.1f;
         if (params->range>1.0f) params->range=1.0f;
         viewer->propagate();
         break;
      case 'f':
         params->farp/=1.1f;
         if (params->farp<2*params->nearp) params->farp=2*params->nearp;
         viewer->propagate();
         break;
      case 'F':
         params->farp*=1.1f;
         if (params->farp>1.0E10f*params->nearp) params->farp=1.0E10f*params->nearp;
         viewer->propagate();
         break;
      case 'v':
         params->fovy-=5.0f;
         if (params->fovy<10.0f) params->fovy=10.0f;
         viewer->propagate();
         break;
      case 'V':
         params->fovy+=5.0f;
         if (params->fovy>170.0f) params->fovy=170.0f;
         viewer->propagate();
         break;
      case 'u':
         if (params->sealevel==-MAXFLOAT) params->sealevel=0.0f;
         else
            {
            params->sealevel+=0.5f;
            if (params->sealevel==0.0f) params->sealevel=-MAXFLOAT;
            }
         viewer->propagate();
         viewer->update();
         break;
      case 'U':
         if (params->sealevel==-MAXFLOAT) params->sealevel=0.0f;
         else
            {
            params->sealevel-=0.5f;
            if (params->sealevel==0.0f) params->sealevel=-MAXFLOAT;
            }
         viewer->propagate();
         viewer->update();
         break;
      case 'm':
         params->fogdensity*=0.9f;
         if (params->fogdensity<1.0E-3f) params->fogdensity=1.0E-3f;
         break;
      case 'M':
         params->fogdensity*=1.1f;
         if (params->fogdensity>10.0f) params->fogdensity=10.0f;
         break;
      case '1':
         if (!params->usefog) params->usefog=TRUE;
         else params->usefog=FALSE;
         break;
      case '2':
         if (!params->usebathymap) params->usebathymap=TRUE;
         else params->usebathymap=FALSE;
         break;
      case '3':
         if (!params->usecontours) params->usecontours=TRUE;
         else params->usecontours=FALSE;
         break;
      case '4':
         if (!params->useskydome) params->useskydome=TRUE;
         else params->useskydome=FALSE;
         break;
      case 'p':
         if (!params->usewaypoints) params->usewaypoints=TRUE;
         else params->usewaypoints=FALSE;
         break;
      case 'E':
         if (!params->useearth) params->useearth=TRUE;
         else params->useearth=FALSE;
         break;
      case 'N':
         if (!params->usenprshader) params->usenprshader=TRUE;
         else params->usenprshader=FALSE;
         break;
      case 'l':
         if (!params->usewireframe) params->usewireframe=TRUE;
         else params->usewireframe=FALSE;
         break;
      case 'h':
         if (sw_hud==0) sw_hud=1;
         else sw_hud=0;
         break;
      case 'H':
         if (sw_hide==0) sw_hide=1;
         else sw_hide=0;
         break;
      case 'c':
         if (sw_cross==0) sw_cross=1;
         else sw_cross=0;
         break;
      case 'o':
         loadsettings();
         break;
      case 'O':
         savesettings();
         break;
      case 'Q':
      case 27:
         delete viewer;
         if (sw_full==0) glutDestroyWindow(winid);
         else glutLeaveGameMode();
         exit(0);
      }
   }

// main function
int main(int argc,char *argv[])
   {
   int i;

   int argc_regular;

   // count regular arguments
   for (argc_regular=0,i=1; i<argc; i++)
      if (*argv[i]!='-') argc_regular++;
      else break;

   // check arguments
   if (argc_regular!=1 && argc_regular<4)
      {
      printf("short usage: %s <url> {-s | -a | -f | -p | -r | -c}\n",argv[0]);
      printf("long usage: %s <url> <tileset.path> <elevation.subpath> <imagery.subpath> {-s | -a | -f | -p | -r | -c}\n",argv[0]);
      printf("options: s=stereo a=anaglyph f=full-screen p=force-pnm r=reset-cache c=auto-s3tc\n");
      exit(1);
      }

   // path setup for elevation and imagery
   if (argc_regular!=1)
      {
      if (sscanf(argv[1],"%s",baseurl)!=1) exit(1);
      if (sscanf(argv[2],"%s",baseid)!=1) exit(1);
      if (sscanf(argv[3],"%s",basepath1)!=1) exit(1);
      if (sscanf(argv[4],"%s",basepath2)!=1) exit(1);
      }
   else
      if (sscanf(argv[1],"%s",shorturl)!=1) exit(1);

   // create the viewer object
   viewer=new viewerbase;

   // get the viewing parameters
   params=viewer->get();

   // process command line options
   for (i=argc_regular+1; i<argc; i++)
      if (strcmp(argv[i],"-s")==0) sw_stereo=1;
      else if (strcmp(argv[i],"-a")==0) sw_anaglyph=1;
      else if (strcmp(argv[i],"-f")==0) sw_full=1;
      else if (strcmp(argv[i],"-p")==0) sw_pnm=1;
      else if (strcmp(argv[i],"-r")==0) sw_reset=1;
      else if (strcmp(argv[i],"-c")==0) sw_autos3tc=1;
      else if (strcmp(argv[i],"-b")==0) sw_bricks=1;
      else if (strcmp(argv[i],"-B")==0) sw_bricks=sw_mpass=1;

   // open window with GLUT:

   winwidth=VIEWER_WINWIDTH;
   winheight=VIEWER_WINHEIGHT;

   glutInit(&argc,argv);
   glutInitWindowSize(winwidth,winheight);
   if (sw_stereo!=0 && sw_anaglyph==0) glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_STEREO);
   else glutInitDisplayMode(GLUT_RGB|GLUT_ALPHA|GLUT_DEPTH|GLUT_DOUBLE);
   if (sw_full==0) winid=glutCreateWindow(VIEWER_WINTITLE);
   else glutEnterGameMode();

   glutDisplayFunc(displayfunc);
   glutReshapeFunc(reshapefunc);
   glutMouseFunc(NULL);
   glutMotionFunc(NULL);
   glutKeyboardFunc(keyboardfunc);
   glutSpecialFunc(NULL);
   glutIdleFunc(displayfunc);

   // initialize the viewing parameters
   initparams();

   // load tileset (short version)
   if (argc_regular==1)
      if (!viewer->load(shorturl,sw_reset))
         {
         printf("unable to load tileset at url=%s\n",shorturl);
         exit(1);
         }

   // load tileset (long version)
   if (argc_regular!=1)
      if (!viewer->load(baseurl,baseid,basepath1,basepath2,sw_reset))
         {
         printf("unable to load tileset at url=%s%s%s (resp. %s)\n",baseurl,baseid,basepath1,basepath2);
         exit(1);
         }

   // load optional features
   viewer->loadopts();

   // set initial view point
   eye=viewer->getinitial();
   initview(eye,0.0,params->fovy/2,VIEWER_UPLIFT*params->farp);

   // initialize VIS bathy map
   initVISbathymap();

   // initialize NPR bathy map
   initNPRbathymap();

   // load settings
   loadsettings();

   // enter event loop
   glutMainLoop();

   // never reached
   return(0);
   }
