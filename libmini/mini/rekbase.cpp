// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minibase.h"
#include "rawbase.h"

#include "rekbase.h"

float REK_TARGET_RATIO=0.5f;
long long REK_TARGET_CELLS=250000000;

// analyze 2048 byte REK header
BOOLINT readREKheader(FILE *file,
                      long long *width,long long *height,long long *depth,unsigned int *components,
                      float *scalex,float *scaley,float *scalez)
   {
   unsigned char data16[2];
   unsigned char data32[4];

   union
      {
      unsigned char uchar[4];
      float float32;
      } float32;

   long long rekwidth,rekheight,rekdepth;
   unsigned int rekbits,rekcomps;

   unsigned int rekheader;
   unsigned int rekmajor,rekminor,rekrevision;

   unsigned int rekX,rekY,rekZ;
   float reknorm;
   float rekvoxelX,rekvoxelZ;

   // volume width
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekwidth=data16[0]+256*data16[1];

   // volume height
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekheight=data16[0]+256*data16[1];

   // volume bits (16 = unsigned short int)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekbits=data16[0]+256*data16[1];

   // volume depth (slices / images per file)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekdepth=data16[0]+256*data16[1];

   if (rekwidth<1 || rekheight<1 || rekdepth<1) return(FALSE);
   if (rekbits!=8 && rekbits!=16) return(FALSE);

   if (rekbits==8) rekcomps=1;
   else rekcomps=2;

   if (rekcomps!=1 && components==NULL) return(FALSE);

   // header size (2048)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekheader=data16[0]+256*data16[1];

   // major version (2)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekmajor=data16[0]+256*data16[1];

   // minor version (5)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekminor=data16[0]+256*data16[1];

   // revision (0)
   if (fread(&data16,1,2,file)!=2) return(FALSE);
   rekrevision=data16[0]+256*data16[1];

   if (rekmajor!=2 || rekminor<5) return(FALSE);
   if (rekrevision>1000) ERRORMSG();

   // fseek to reconstruction params
   if (fseek(file,0x238,SEEK_SET)==-1) return(FALSE);

   // reconstruction width
   if (fread(&data32,1,4,file)!=4) return(FALSE);
   rekX=data32[0]+256*(data32[1]+256*(data32[2]+256*data32[3]));

   // reconstruction height
   if (fread(&data32,1,4,file)!=4) return(FALSE);
   rekY=data32[0]+256*(data32[1]+256*(data32[2]+256*data32[3]));

   // reconstruction depth
   if (fread(&data32,1,4,file)!=4) return(FALSE);
   rekZ=data32[0]+256*(data32[1]+256*(data32[2]+256*data32[3]));

   if (rekX!=rekwidth || rekY!=rekheight || rekZ!=rekdepth) return(FALSE);

   // reconstruction norm (scaling factor, abscoeff = greyvalue / norm)
   if (fread(float32.uchar,1,4,file)!=4) return(FALSE);
   reknorm=float32.float32;

   if (reknorm<=0.0f) return(FALSE);
   reknorm=1.0f/reknorm;

   // reconstruction voxel size X (micrometers, equal to voxel size Y)
   if (fread(float32.uchar,1,4,file)!=4) return(FALSE);
   rekvoxelX=float32.float32;

   // reconstruction voxel size Z (micrometers)
   if (fread(float32.uchar,1,4,file)!=4) return(FALSE);
   rekvoxelZ=float32.float32;

   // seek to raw data
   if (fseek(file,rekheader,SEEK_SET)==-1) return(FALSE);

   *width=rekwidth;
   *height=rekheight;
   *depth=rekdepth;

   if (components!=0) *components=rekcomps;

   // map micrometers to meters
   if (scalex!=NULL) *scalex=rekvoxelX/1E6;
   if (scaley!=NULL) *scaley=rekvoxelX/1E6;
   if (scalez!=NULL) *scalez=rekvoxelZ/1E6;

   return(TRUE);
   }

// read a REK volume (Fraunhofer EZRT volume format)
unsigned char *readREKvolume(const char *filename,
                             long long *width,long long *height,long long *depth,unsigned int *components,
                             float *scalex,float *scaley,float *scalez)
   {
   FILE *file;

   unsigned char *volume;
   long long bytes;

   // open REK file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze REK header
   if (!readREKheader(file,width,height,depth,components,
                      scalex,scaley,scalez))
      {
      fclose(file);
      return(NULL);
      }

   bytes=(*width)*(*height)*(*depth)*(*components);

   if ((volume=(unsigned char *)malloc(bytes))==NULL) ERRORMSG();

   // read volume
   if (fread(volume,bytes,1,file)!=1)
      {
      free(volume);
      fclose(file);
      return(NULL);
      }

   fclose(file);

   return(volume);
   }

// read REK file format header
BOOLINT readREKheader(const char *filename,
                      long long *width,long long *height,long long *depth,unsigned int *components,
                      float *scalex,float *scaley,float *scalez)
   {
   FILE *file;

   // open REK file
   if ((file=fopen(filename,"rb"))==NULL) return(FALSE);

   // analyze REK header
   if (!readREKheader(file,width,height,depth,components,
                      scalex,scaley,scalez))
      {
      fclose(file);
      return(FALSE);
      }

   fclose(file);

   return(TRUE);
   }

// copy a REK volume to a RAW volume
char *copyREKvolume(const char *filename,const char *output)
   {
   FILE *file;

   long long width,height,depth;
   unsigned int components;
   float scalex,scaley,scalez;

   char *outname;

   // open REK file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze REK header
   if (!readREKheader(file,&width,&height,&depth,&components,
                      &scalex,&scaley,&scalez))
      {
      fclose(file);
      return(NULL);
      }

   // copy REK data to RAW file
   if (!(outname=copyRAWvolume(file,output,
                               width,height,depth,1,
                               components,8,FALSE,FALSE,
                               scalex,scaley,scalez)))
      {
      fclose(file);
      return(NULL);
      }

   fclose(file);

   return(outname);
   }

// copy a REK volume to a RAW volume with out-of-core cropping and non-linear quantization
char *processREKvolume(const char *filename,
                       const char *infix,
                       float ratio, // crop volume ratio
                       long long maxcells, // down-size threshold
                       void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   long long width,height,depth;
   unsigned int components;
   float scalex,scaley,scalez;

   char *output,*dot;
   char *outname;

   // open REK file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze REK header
   if (!readREKheader(file,&width,&height,&depth,&components,
                      &scalex,&scaley,&scalez))
      {
      fclose(file);
      return(NULL);
      }

   // use input file name as output prefix
   output=strdup(filename);
   dot=strrchr(output,'.');

   // remove suffix from output
   if (dot!=NULL)
      if (strcasecmp(dot,".rek")==0) *dot='\0';

   // copy REK data to RAW file
   if (!(outname=processRAWvolume(file,output,
                                  width,height,depth,1,
                                  components,8,FALSE,FALSE,
                                  scalex,scaley,scalez,
                                  infix,
                                  ratio,maxcells,
                                  feedback,obj)))
      {
      free(output);
      fclose(file);
      return(NULL);
      }

   free(output);
   fclose(file);

   return(outname);
   }

// read a REK volume out-of-core
unsigned char *readREKvolume_ooc(const char *filename,
                                 long long *width,long long *height,long long *depth,unsigned int *components,
                                 float *scalex,float *scaley,float *scalez,
                                 float ratio, // crop volume ratio
                                 long long maxcells, // down-size threshold
                                 void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   char *outname;

   unsigned char *volume;
   long long steps;

   volume=NULL;

   outname=processREKvolume(filename,NULL,ratio,maxcells,feedback,obj);

   if (outname!=NULL)
      {
      volume=readRAWvolume(outname,
                           width,height,depth,&steps,
                           components,NULL,NULL,NULL,
                           scalex,scaley,scalez);

      free(outname);
      }

   return(volume);
   }

// extract an iso-surface from a REK volume out-of-core
char *extractREKvolume(const char *filename,
                       const char *output,
                       double isovalue, // iso value to be extracted as surface
                       void (*feedback)(const char *info,float percent,void *obj),void *obj)
   {
   FILE *file;

   long long width,height,depth;
   unsigned int components;
   float scalex,scaley,scalez;

   char *outname;

   // open REK file
   if ((file=fopen(filename,"rb"))==NULL) return(NULL);

   // analyze REK header
   if (!readREKheader(file,&width,&height,&depth,&components,
                      &scalex,&scaley,&scalez))
      {
      fclose(file);
      return(NULL);
      }

   // extract iso surface
   if (!(outname=extractRAWvolume(file,output,
                                  width,height,depth,1,
                                  components,8,FALSE,FALSE,
                                  scalex,scaley,scalez,
                                  isovalue,
                                  feedback,obj)))
      {
      fclose(file);
      return(NULL);
      }

   fclose(file);

   return(outname);
   }
