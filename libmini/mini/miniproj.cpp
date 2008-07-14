// (c) by Stefan Roettger

#include "miniOGL.h"

#include "miniproj.h"

// default constructor
miniproj::miniproj()
   {
   EMI=0.0f;
   RHO=0.0f;

   initglsetup();
   setupprogs();
   }

// destructor
miniproj::~miniproj()
   {deleteprogs();}

// calculate whether or not a triangle is front- or back-facing
inline BOOLINT miniproj::isfront(const miniv3d &p,const miniv3d &v1,const miniv3d &v2,const miniv3d &v3,const miniv3d &e)
   {
   miniv3d n;

   n=(v2-v1)/(v3-v1);

   return((n*(v1-e)>0.0)?n*(p-v1)<0.0:n*(p-v1)>0.0);
   }

// calculate the intersection of a ray with a plane
inline double miniproj::intersect(const miniv3d &p,const miniv3d &d,const miniv3d &o,const miniv3d &d1,const miniv3d &d2,miniv3d &m)
   {
   miniv3d n;

   double lambda;

   n=d1/d2;

   lambda=n*d;
   if (lambda!=0.0) lambda=n*(o-p)/lambda;

   m=p+lambda*d;

   return(lambda);
   }

// draw a triangle fan with 3 triangles
inline void miniproj::draw3fan(const miniv3d &v1,const double c1,
                               const miniv3d &v2,const double c2,
                               const miniv3d &v3,const double c3,
                               const miniv3d &v4,const double c4,
                               const miniv3d &eye)
   {
   miniv3d d1,d2,m,n;

   double lambda,
          w1,w2,w3,
          c234;

   d1=v3-v2;
   d2=v4-v2;

   // calculate thick vertex
   lambda=intersect(v1,v1-eye,v2,d1,d2,m);

   n=d1/d2;

   n.x=FABS(n.x);
   n.y=FABS(n.y);
   n.z=FABS(n.z);

   // barycentric interpolation at thick vertex
   if (n.x>n.y)
      if (n.x>n.z)
         {
         w1=FABS((v3.y-m.y)*(v4.z-m.z)-(v3.z-m.z)*(v4.y-m.y));
         w2=FABS((v2.y-m.y)*(v4.z-m.z)-(v2.z-m.z)*(v4.y-m.y));
         w3=FABS((v2.y-m.y)*(v3.z-m.z)-(v2.z-m.z)*(v3.y-m.y));

         c234=(w1*c2+w2*c3+w3*c4)/n.x;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }
   else
      if (n.y>n.z)
         {
         w1=FABS((v3.z-m.z)*(v4.x-m.x)-(v3.x-m.x)*(v4.z-m.z));
         w2=FABS((v2.z-m.z)*(v4.x-m.x)-(v2.x-m.x)*(v4.z-m.z));
         w3=FABS((v2.z-m.z)*(v3.x-m.x)-(v2.x-m.x)*(v3.z-m.z));

         c234=(w1*c2+w2*c3+w3*c4)/n.y;
         }
      else
         {
         w1=FABS((v3.x-m.x)*(v4.y-m.y)-(v3.y-m.y)*(v4.x-m.x));
         w2=FABS((v2.x-m.x)*(v4.y-m.y)-(v2.y-m.y)*(v4.x-m.x));
         w3=FABS((v2.x-m.x)*(v3.y-m.y)-(v2.y-m.y)*(v3.x-m.x));

         c234=(w1*c2+w2*c3+w3*c4)/n.z;
         }

   // check orientation
   if (lambda<0.0)
      {
      // calculate thickness
      lambda=(v1-m).getlength();

      // calculate back-facing normal
      n=(v2-v1)/(v3-v1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);

      // calculate back-facing normal
      n=(v3-v1)/(v4-v1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);

      // calculate back-facing normal
      n=(v4-v1)/(v2-v1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c1,c234,lambda);
      fanvertex(m.x,m.y,m.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      }
   else
      {
      // calculate thickness
      lambda=(m-v1).getlength();

      // calculate back-facing normal
      n=(v2-m)/(v3-m);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);

      // calculate back-facing normal
      n=(v3-m)/(v4-m);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);

      // calculate back-facing normal
      n=(v4-m)/(v2-m);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c234,c1,lambda);
      fanvertex(v1.x,v1.y,v1.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      }
   }

// draw a triangle fan with 4 triangles
void miniproj::draw4fan(const miniv3d &v1,const double c1,
                        const miniv3d &v2,const double c2,
                        const miniv3d &v3,const double c3,
                        const miniv3d &v4,const double c4,
                        const miniv3d &eye)
   {
   miniv3d m1,m2,d,n;

   double lambda,
          alpha,beta,
          c12,c34;

   // calculate thick vertex (first intersection)
   alpha=intersect(v1,v2-v1,eye,v3-eye,v4-eye,m1);

   // calculate thick vertex (second intersection)
   beta=intersect(v3,v4-v3,eye,v1-eye,v2-eye,m2);

   // linear interpolation at thick vertex
   c12=(1.0f-alpha)*c1+alpha*c2;
   c34=(1.0f-beta)*c3+beta*c4;

   d=m2-m1;

   // calculate thickness
   lambda=d.getlength();

   // check orientation
   if (d*(m1-eye)<0.0f)
      {
      // calculate back-facing normal
      n=(v1-m1)/(v3-m1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);

      // calculate back-facing normal
      n=(v3-m1)/(v2-m1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);

      // calculate back-facing normal
      n=(v2-m1)/(v4-m1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);

      // calculate back-facing normal
      n=(v4-m1)/(v1-m1);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c12,c34,lambda);
      fanvertex(m2.x,m2.y,m2.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      }
   else
      {
      // calculate back-facing normal
      n=(v1-m2)/(v3-m2);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);

      // calculate back-facing normal
      n=(v3-m2)/(v2-m2);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c3,c3,0.0f);
      fanvertex(v3.x,v3.y,v3.z);
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);

      // calculate back-facing normal
      n=(v2-m2)/(v4-m2);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c2,c2,0.0f);
      fanvertex(v2.x,v2.y,v2.z);
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);

      // calculate back-facing normal
      n=(v4-m2)/(v1-m2);
      n.normalize();

      // render thick vertex
      beginfan();
      normal(n.x,n.y,n.z);
      texcoord(c34,c12,lambda);
      fanvertex(m1.x,m1.y,m1.z);

      // render silhouette vertices
      texcoord(c4,c4,0.0f);
      fanvertex(v4.x,v4.y,v4.z);
      texcoord(c1,c1,0.0f);
      fanvertex(v1.x,v1.y,v1.z);
      }
   }

// project a tetrahedron
// needs to be in front of the near plane
void miniproj::proj(const miniv3d &v1,const double c1,
                    const miniv3d &v2,const double c2,
                    const miniv3d &v3,const double c3,
                    const miniv3d &v4,const double c4,
                    const miniv3d &col,
                    const miniv3d &eye)
   {
   int ff;

   if (c1==0.0 && c2==0.0 && c3==0.0 && c4==0.0) return;

   color(col.x,col.y,col.z);

   ff=0;

   if (isfront(v4,v1,v2,v3,eye)) ff|=1;
   if (isfront(v3,v1,v4,v2,eye)) ff|=2;
   if (isfront(v1,v2,v4,v3,eye)) ff|=4;
   if (isfront(v2,v3,v4,v1,eye)) ff|=8;

   switch (ff)
      {
      case 1: case 14: draw3fan(v4,c4,v1,c1,v2,c2,v3,c3,eye); break;
      case 2: case 13: draw3fan(v3,c3,v1,c1,v2,c2,v4,c4,eye); break;
      case 3: case 12: draw4fan(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
      case 4: case 11: draw3fan(v1,c1,v2,c2,v3,c3,v4,c4,eye); break;
      case 5: case 10: draw4fan(v1,c1,v4,c4,v2,c2,v3,c3,eye); break;
      case 6: case  9: draw4fan(v1,c1,v3,c3,v2,c2,v4,c4,eye); break;
      case 7: case  8: draw3fan(v2,c2,v1,c1,v3,c3,v4,c4,eye); break;
      }
   }

// initialize projection state
void miniproj::initproj(float emi,float rho)
   {
   if (emi<0.0f || rho<0.0f) ERRORMSG();

   EMI=emi;
   RHO=rho;

   initstate();
   disableculling();
   enableblending();
   disableZwriting();

   enablevtxshader();
   enablepixshader();

   beginfans();
   }

// de-initialize projection state
void miniproj::exitproj()
   {
   endfans();

   disablevtxshader();
   disablepixshader();

   enableZwriting();
   enableBFculling();
   disableblending();
   exitstate();
   }

// vertex and fragment programs:

// enable vertex shader
void miniproj::enablevtxshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   GLuint vtxprogid;

   initglexts();
   initwglprocs();

   if (GLEXT_MT!=0 && GLEXT_VP!=0 && GLEXT_FP!=0)
      if (VTXPROG!=NULL)
         {
         if (VTXPROGID==0)
            {
            glGenProgramsARB(1,&vtxprogid);
            glBindProgramARB(GL_VERTEX_PROGRAM_ARB,vtxprogid);
            glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(VTXPROG),VTXPROG);
            VTXPROGID=vtxprogid;
            }

         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,VTXPROGID);
         glEnable(GL_VERTEX_PROGRAM_ARB);
         }

#endif

#endif
   }

// disable vertex shader
void miniproj::disablevtxshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_MT!=0 && GLEXT_VP!=0 && GLEXT_FP!=0)
      if (VTXPROG!=NULL)
         {
         glBindProgramARB(GL_VERTEX_PROGRAM_ARB,0);
         glDisable(GL_VERTEX_PROGRAM_ARB);
         }

#endif

#endif
   }

// enable pixel shader
void miniproj::enablepixshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   GLuint frgprogid;

   initglexts();
   initwglprocs();

   if (GLEXT_MT!=0 && GLEXT_VP!=0 && GLEXT_FP!=0)
      if (FRGPROG!=NULL)
         {
         if (FRGPROGID==0)
            {
            glGenProgramsARB(1,&frgprogid);
            glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,frgprogid);
            glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,strlen(FRGPROG),FRGPROG);
            FRGPROGID=frgprogid;
            }

         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,0,EMI,RHO,0.0f,0.0f);
         glProgramEnvParameter4fARB(GL_FRAGMENT_PROGRAM_ARB,1,0.5f,fexp(1.0f),1.0f,0.0f);

         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,FRGPROGID);
         glEnable(GL_FRAGMENT_PROGRAM_ARB);
         }

#endif

#endif
   }

// disable pixel shader
void miniproj::disablepixshader()
   {
#ifndef NOOGL

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   if (GLEXT_MT!=0 && GLEXT_VP!=0 && GLEXT_FP!=0)
      if (FRGPROG!=NULL)
         {
         glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,0);
         glDisable(GL_FRAGMENT_PROGRAM_ARB);
         }

#endif

#endif
   }

// initialize vertex and fragment program setup
void miniproj::setupprogs()
   {
   // vertex shader
   static char *vtxprog="!!ARBvp1.0 \n\
      PARAM mat[4]={state.matrix.mvp}; \n\
      PARAM invtra[4]={state.matrix.modelview.invtrans}; \n\
      TEMP vtx,col,nrm,tex,pos,vec,dir,len; \n\
      ### fetch actual vertex \n\
      MOV vtx,vertex.position; \n\
      MOV col,vertex.color; \n\
      MOV nrm,vertex.normal; \n\
      MOV tex,vertex.texcoord[0]; \n\
      ### transform vertex with modelview \n\
      DP4 pos.x,mat[0],vtx; \n\
      DP4 pos.y,mat[1],vtx; \n\
      DP4 pos.z,mat[2],vtx; \n\
      DP4 pos.w,mat[3],vtx; \n\
      ### transform normal with inverse transpose \n\
      DP4 vec.x,invtra[0],nrm; \n\
      DP4 vec.y,invtra[1],nrm; \n\
      DP4 vec.z,invtra[2],nrm; \n\
      DP4 vec.w,invtra[3],nrm; \n\
      ### normalize view vector \n\
      DP3 len.x,pos,pos; \n\
      RSQ len.x,len.x; \n\
      MUL dir,pos,len.x; \n\
      ### write resulting vertex \n\
      MOV result.position,pos; \n\
      MOV result.color,col; \n\
      MOV result.texcoord[0],vec; \n\
      MOV result.texcoord[1],tex; \n\
      MOV result.texcoord[2],pos; \n\
      MAD result.texcoord[3],dir,tex.z,pos; \n\
      END \n";

   // pixel shader
   static char *frgprog="!!ARBfp1.0 \n\
      PARAM c0=program.env[0]; \n\
      PARAM c1=program.env[1]; \n\
      TEMP col,nrm,tex,pos1,pos2,len; \n\
      ### fetch actual fragment \n\
      MOV col,fragment.color; \n\
      MOV nrm,fragment.texcoord[0]; \n\
      MOV tex,fragment.texcoord[1]; \n\
      MOV pos1,fragment.texcoord[2]; \n\
      MOV pos2,fragment.texcoord[3]; \n\
      ### calculate thickness \n\
      SUB pos1,pos2,pos1; \n\
      DP3 len.x,pos1,pos1; \n\
      POW len.x,len.x,c1.x; \n\
      ### calculate optical depth \n\
      ADD len.y,tex.x,tex.y; \n\
      MUL len.y,len.y,c1.x; \n\
      MUL len.x,len.x,len.y; \n\
      MUL len.x,len.x,c0.y; \n\
      ### calculate absorption \n\
      POW len.x,c1.y,-len.x; \n\
      SUB len.x,c1.z,len.x; \n\
      ### write resulting fragment \n\
      MUL result.color.xyz,col,c0.x; \n\
      MOV result.color.w,len.x; \n\
      END \n";

   VTXPROG=vtxprog;
   VTXPROGID=0;

   FRGPROG=frgprog;
   FRGPROGID=0;
   }

// delete vertex and fragment program
void miniproj::deleteprogs()
   {
#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)

   GLuint progid;

   if (GLEXT_MT!=0 && GLEXT_VP!=0 && GLEXT_FP!=0)
      {
      if (VTXPROGID!=0)
         {
         progid=VTXPROGID;
         glDeleteProgramsARB(1,&progid);
         }

      if (FRGPROGID!=0)
         {
         progid=FRGPROGID;
         glDeleteProgramsARB(1,&progid);
         }
      }

#endif
   }

// OpenGL extensions:

// initialize OpenGL extension setup
void miniproj::initglsetup()
   {
   GLSETUP=0;
   WGLSETUP=0;

   GLEXT_MT=0;
   GLEXT_VP=0;
   GLEXT_FP=0;
   }

// check for OpenGL extensions
void miniproj::initglexts()
   {
#ifndef NOOGL

   char *gl_exts;

   if (GLSETUP==0)
      {
      GLEXT_MT=0;
      GLEXT_VP=0;
      GLEXT_FP=0;

      if ((gl_exts=(char *)glGetString(GL_EXTENSIONS))==NULL) ERRORMSG();

      if (strstr(gl_exts,"GL_ARB_multitexture")!=NULL) GLEXT_MT=1;
      if (strstr(gl_exts,"GL_ARB_vertex_program")!=NULL) GLEXT_VP=1;
      if (strstr(gl_exts,"GL_ARB_fragment_program")!=NULL) GLEXT_FP=1;

      GLSETUP=1;
      }

#endif
   }

// Windows OpenGL extension setup
void miniproj::initwglprocs()
   {
#ifndef NOOGL

#ifdef _WIN32

   if (WGLSETUP==0)
      {
#ifdef GL_ARB_multitexture
      glActiveTextureARB=(PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
      glClientActiveTextureARB=(PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");
#endif

#if defined(GL_ARB_vertex_program) && defined(GL_ARB_fragment_program)
      glGenProgramsARB=(PFNGLGENPROGRAMSARBPROC)wglGetProcAddress("glGenProgramsARB");
      glBindProgramARB=(PFNGLBINDPROGRAMARBPROC)wglGetProcAddress("glBindProgramARB");
      glProgramStringARB=(PFNGLPROGRAMSTRINGARBPROC)wglGetProcAddress("glProgramStringARB");
      glProgramEnvParameter4fARB=(PFNGLPROGRAMENVPARAMETER4FARBPROC)wglGetProcAddress("glProgramEnvParameter4fARB");
      glDeleteProgramsARB=(PFNGLDELETEPROGRAMSARBPROC)wglGetProcAddress("glDeleteProgramsARB");
#endif

      WGLSETUP=1;
      }

#endif

#endif
   }
