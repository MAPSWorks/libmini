// terrain rendering core
// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minicore.h"

#define SINGLEPHASE (0)
#define CALCMAPPHASE (1)
#define DRAWMAPPHASE (2)
#define DRAWSEAPHASE (3)

#define SAFETYZONE (0.5f)

#define X(i) (((i)-S/2)*Dx)
#define Z(j) ((S/2-(j))*Dz)

#define Y(i,j) (y[i][j]*SCALE)
#define YF(i,j) (yf[i][j]*LAMBDA)

#define cpr(fc) (ftrc(65535.0f*fc))
#define dcpr(i,j,s2) ((bc[i-s2][j]+(bc[i][j-s2]<<8))/65535.0f)

float minres=9.0f; // minimum resolution
float maxd2v=100.0f; // maximum d2-value
float sead2v=0.5f; // sea level d2-value
float minoff=0.1f; // minimum base offset
int maxcull=8; // maximum culling depth
int skirts=0; // use skirts?

MINIDATA **y; // height field
MINIDATA *DH; // height differences

unsigned char **bc; // d2-values

int tid; // texture id
int twidth; // texture width
int theight; // texture height
int tmipmaps; // texture mipmaps enabled

unsigned char **yf; // ground fog map

int S=0; // map size
float Dx,Dz,D; // cell dimensions
float SCALE; // vertical scaling

float LAMBDA; // ground fog height
float DISPLACE; // vertical fog displacement
float EMISSION; // fog emission per unit
float FOGATT; // fog attenuation
float FR,FG,FB; // fog color

float c, // resolution
      EX,EY,EZ, // eye point
      FX,FY,FZ, // focus point
      DX,DY,DZ, // view direction
      RX,RY,RZ, // right direction
      UX,UY,UZ, // up direction
      OX,OY,OZ, // map origin
      DF2; // focus offset

// culling constants
float NEARP,FARP,k1,k2, // near and far clipping plane
      nx1,ny1,nz1, // left clipping plane
      nx2,ny2,nz2, // right clipping plane
      nx3,ny3,nz3, // bottom clipping plane
      nx4,ny4,nz4, // top clipping plane
      k11,k12, // left clipping coeffs
      k21,k22, // right clipping coeffs
      k31,k32, // bottom clipping coeffs
      k41,k42; // top clipping coeffs

// projection and culling mode
BOOLINT ORTHO,CULLING;

// neighbours
unsigned char **bcn[4];
int S2[4];

// prism cache
float *PRISMCACHE=0;
int PRISMMAX=1,PRISMCNT=0;
int FANSTATE;

// sea level
float SEALEVEL=-MAXFLOAT;
float SEALEVELMIN=-MAXFLOAT;
float SEALEVELMAX=-MAXFLOAT;
int SEASTATE;

// triangle fan callbacks
void (*beginfan_callback)();
void (*fanvertex_callback)(float i,float y,float j);
void (*notify_callback)(int i,int j,int s);
void (*prismedge_callback)(float x,float y,float yf,float z);
