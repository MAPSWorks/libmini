// (c) by Stefan Roettger

#ifndef DATABASE_H
#define DATABASE_H

#include "minibase.h"

/* This class represents a single 1D, 2D, 3D or 4D data buffer with
   additional metadata such as dimension and type. */

class databuf
   {
   public:

   enum DATABUF_TYPE
      {
      DATABUF_TYPE_BYTE=0,
      DATABUF_TYPE_SHORT=1,
      DATABUF_TYPE_FLOAT=2,
      DATABUF_TYPE_RGB=3,
      DATABUF_TYPE_RGBA=4,
      DATABUF_TYPE_RGB_S3TC=5,
      DATABUF_TYPE_RGBA_S3TC=6,
      DATABUF_TYPE_RGB_MM=7,
      DATABUF_TYPE_RGBA_MM=8,
      DATABUF_TYPE_RGB_MM_S3TC=9,
      DATABUF_TYPE_RGBA_MM_S3TC=10
      };

   enum DATABUF_CRS
      {
      DATABUF_CRS_LINEAR=0,
      DATABUF_CRS_LLH=1,
      DATABUF_CRS_UTM=2,
      DATABUF_CRS_MERC=3
      };

   enum DATABUF_DATUM
      {
      // USGS datum codes
      DATABUF_DATUM_NAD27=1,
      DATABUF_DATUM_WGS72=2,
      DATABUF_DATUM_WGS84=3,
      DATABUF_DATUM_NAD83=4,
      // libMini datum codes
      DATABUF_DATUM_SPHERE=5,
      DATABUF_DATUM_ED50=6,
      DATABUF_DATUM_ED79=7,
      DATABUF_DATUM_OldHawaiian=8,
      DATABUF_DATUM_Luzon=9,
      DATABUF_DATUM_Tokyo=10,
      DATABUF_DATUM_OSGB1936=11,
      DATABUF_DATUM_Australian1984=12,
      DATABUF_DATUM_Geodetic1949=13,
      DATABUF_DATUM_SouthAmerican1969=14
      };

   enum DATABUF_EXTFMT
      {
      DATABUF_EXTFMT_PLAIN=0,
      DATABUF_EXTFMT_JPEG=1,
      DATABUF_EXTFMT_PNG=2,
      DATABUF_EXTFMT_Z=3
      };

   //! default constructor
   databuf();

   //! mandatory metadata
   unsigned int xsize;  // data size along the x-axis of 1D data
   unsigned int ysize;  // optional data size along the y-axis of 2D data
   unsigned int zsize;  // optional data size along the z-axis of 3D data
   unsigned int tsteps; // optional number of frames for time-dependent data
   unsigned int type;   // 0 = unsigned byte, 1 = signed short, 2 = float, 3 = RGB, 4 = RGBA, 5 = compressed RGB (S3TC DXT1), 6 = compressed RGBA (S3TC DXT1 with 1-bit alpha), 7 = mip-mapped RGB, 8 = mip-mapped RGBA, 9 = mip-mapped compressed RGB, 10 = mip-mapped compressed RGBA

   //! optional comment
   char *comment;

   //! optional metadata
   double swx,swy; // SW corner of data brick
   double nwx,nwy; // NW corner of data brick
   double nex,ney; // NE corner of data brick
   double sex,sey; // SE corner of data brick
   double h0,dh;   // base elevation and height of data brick
   double t0,dt;   // time frame start and exposure time

   //! optional corner points in Lat/Lon (WGS84, degrees)
   double LLWGS84_swx,LLWGS84_swy; // SW corner of data brick
   double LLWGS84_nwx,LLWGS84_nwy; // NW corner of data brick
   double LLWGS84_nex,LLWGS84_ney; // NE corner of data brick
   double LLWGS84_sex,LLWGS84_sey; // SE corner of data brick

   //! optional scaling
   float scaling; // scale factor of data values, default=1.0f
   float bias;    // bias of data values, default=0.0f

   //! optional minimum and maximum value
   float minvalue;
   float maxvalue;

   //! coordinate system indicator
   int crs; // 0 = none, 1 = LL, 2 = UTM, 3 = Merc
   int zone; // coordinate system zone
   int datum; // coordinate system datum

   //! no-data indicator
   float nodata;

   //! indicator for external format
   unsigned int extformat; // 0=plain, 1=JPEG, 2=PNG, 3=Z

   //! indicator for implicit format
   unsigned int implformat;

   //! data chunk
   void *data;         // pointer to raw data, null pointer indicates missing data
   unsigned int bytes; // number of raw data bytes

   //! check for missing data
   int missing() const {return(data==NULL || extformat!=DATABUF_EXTFMT_PLAIN || implformat!=0);}

   //! allocate a new memory chunk
   void alloc(unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! set data to memory chunk
   void set(void *chunk,unsigned int length,
            unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! copy data from memory chunk
   void copy(void *chunk,unsigned int length,
             unsigned int xs,unsigned int ys,unsigned int zs,unsigned int ts=1,unsigned int ty=0);

   //! copy data from buffer
   void copy(databuf *buf);

   //! duplicate data from buffer (by allocating memory)
   void duplicate(databuf *buf);

   //! clear buffer
   void clear(float value=0.0f);

   //! reset buffer (without releasing memory)
   void reset();

   //! release buffer
   void release();

   //! set comment string
   void set_comment(char *str);

   //! set native extents
   void set_extents(double left,double right,double bottom,double top);

   //! set LLWGS84 extents
   void set_LLWGS84extents(double left,double right,double bottom,double top);

   //! set native extents
   void set_corners(double sw_corner_x,double sw_corner_y,
                    double se_corner_x,double se_corner_y,
                    double nw_corner_x,double nw_corner_y,
                    double ne_corner_x,double ne_corner_y);

   //! set LLWGS84 corners
   void set_LLWGS84corners(double sw_corner_x,double sw_corner_y,
                           double se_corner_x,double se_corner_y,
                           double nw_corner_x,double nw_corner_y,
                           double ne_corner_x,double ne_corner_y);

   //! set height extent
   void set_height(double bottom,double height);

   //! set time range
   void set_time(double time,double range);

   //! set coordinate system
   void set_crs(int crs_type,int crs_zone=0,int crs_datum=0);

   //! native input/output
   int savedata(const char *filename,unsigned int extfmt=DATABUF_EXTFMT_PLAIN); // data is saved in MSB byte order
   int loaddata(const char *filename,int stub=0,unsigned int tstart=0,unsigned int tstop=0); // data is converted from MSB to native byte order

   //! convert byte order
   void swap2(int msb=1);

   //! set conversion hook for external formats
   static void setconversion(int (*conversion)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data),void *data=NULL);

   //! check conversion hook
   static int check_conversion() {return((CONVERSION_HOOK==NULL)?0:1);}

   //! set automatic mip-mapping
   static void setautomipmap(int autos3tcmipmap=0);

   //! check automatic mip-mapping
   static int check_automipmap() {return(AUTOS3TCMIPMAP);}

   //! automatic mip-mapping
   void automipmap();

   //! check mip-mapping
   int check_mipmap();

   //! set hook for automatic s3tc compression
   static void setautocompress(void (*autocompress)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,void *data),void *data=NULL);

   //! check autocompress hook
   static int check_autocompress() {return((AUTOCOMPRESS_HOOK==NULL)?0:1);}

   //! automatic s3tc compression
   void autocompress();

   //! check s3tc compression
   int check_s3tc();

   //! set hook for automatic s3tc decompression
   static void setautodecompress(void (*autodecompress)(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,unsigned char **rawdata,unsigned int *rawbytes,int width,int height,void *data),void *data=NULL);

   //! check autodecompress hook
   static int check_autodecompress() {return((AUTODECOMPRESS_HOOK==NULL)?0:1);}

   //! automatic s3tc decompression
   void autodecompress();

   //! set interpreter hook for implicit format
   static void setinterpreter(void (*parser)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data),void *data,
                              void (*interpreter)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data));

   //! check interpreter hook
   static int check_interpreter() {return((INTERPRETER_HOOK==NULL)?0:1);}

   //! data is loaded from PNM file
   int loadPNMdata(const char *filename);

   //! data is converted from PPM into compressed native format
   int loadPPMcompressed(const char *filename);

   //! data is converted from normalized PPM into compressed native format
   int loadPPMnormalized(const char *filename,const char *normalizedpath);

   //! data is loaded from PVM file
   int loadPVMdata(const char *filename,
                   double midx=0.0,double midy=0.0,double basez=0.0,
                   double dx=1.0,double dy=1.0,double dz=1.0);

   //! data is loaded from PVM time series
   //! the actual time step n is appended to the file name, e.g. filename.pvm-t01
   int loadPVMdata(const char *filename,
                   unsigned int t, unsigned int n,
                   double timestart,double timestep,
                   double midx,double midy,double basez,
                   double dx,double dy,double dz);

   //! data is loaded from MOE file
   int loadMOEdata(const char *filename,float *useful_smallest=0,float *useful_greatest=0);

   //! data is saved as plain PNM image
   void savePNMdata(const char *filename);

   //! data is saved as PVM volume
   void savePVMdata(const char *filename);

   //! data is generated from plane equation
   void generateplane(int size, // grid size
                      double px,double py,double pz, // point on plane
                      double nx,double ny,double nz, // plane normal
                      double dx,double dy,double dz); // dimension of box

   //! convert data from one type to another
   void convertdata(unsigned int newtype);

   //! resample data at a higher resolution
   void resampledata(unsigned int xs,unsigned int ys,unsigned int zs);

   //! set the no-data indicator
   void setnodata(float value);

   //! get the minimum and maximum scalar value
   void getminmax(float *minval=0,float *maxval=0);

   //! get the minimum and maximum scalar value within a useful range
   void getminmax(float usefs,float usefg, // useful range
                  float *minval=0,float *maxval=0); // result range

   //! replace data values
   unsigned int replacedata(float value1,float value2);

   //! check for no-data values
   int checknodata();

   //! replace no-data values
   unsigned int replacenodata(float value);

   //! fill-in no-data values
   unsigned int fillnodata(int radius=5);

   //! replace invalid values
   unsigned int replaceinvalid(float usefs,float usefg,float useful);

   //! clamp to range
   unsigned int clamp(float usefs,float usefg);

   //! compute absolute values
   void computeabsolute();

   //! check for invalid value (and nan)
   inline int checknodata(const float value)
      {
      if (value==nodata) return(1);
      else if (checknan(value)) return(1);
      else return(0);
      }

   //! check for valid value (and nan)
   inline int checkval(const float value)
      {
      if (value==nodata) return(0);
      else if (checknan(value)) return(0);
      else return(1);
      }

   //! set a single scalar value
   void setval(const unsigned int i,const unsigned int j,const unsigned int k,const float value);
   void setval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float value);

   //! get a single scalar value
   float getval(const unsigned int i,const unsigned int j,const unsigned int k);
   float getval(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t);

   //! get an interpolated scalar value
   float getvalue(float x,float y,float z,unsigned int t);

   //! set rgb[a] color
   void setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const float *value);
   void setrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value);
   void setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const float *value);
   void setrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,const float *value);

   //! get rgb[a] color
   void getrgb(const unsigned int i,const unsigned int j,const unsigned int k,float *value);
   void getrgb(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,float *value);
   void getrgba(const unsigned int i,const unsigned int j,const unsigned int k,float *value);
   void getrgba(const unsigned int i,const unsigned int j,const unsigned int k,const unsigned int t,float *value);

   //! print values
   void print();

   protected:

   static unsigned int MAGIC1;
   static unsigned int MAGIC2;
   static unsigned int MAGIC3;
   static unsigned int MAGIC4;
   static unsigned int MAGIC5;
   static unsigned int MAGIC6;

   private:

   static unsigned short int INTEL_CHECK;

   static int AUTOS3TCMIPMAP;

   static int (*CONVERSION_HOOK)(int israwdata,unsigned char *srcdata,unsigned int bytes,unsigned int extformat,unsigned char **newdata,unsigned int *newbytes,databuf *obj,void *data);
   static void *CONVERSION_DATA;

   static void (*AUTOCOMPRESS_HOOK)(int isrgbadata,unsigned char *rawdata,unsigned int bytes,unsigned char **s3tcdata,unsigned int *s3tcbytes,int width,int height,void *data);
   static void *AUTOCOMPRESS_DATA;

   static void (*AUTODECOMPRESS_HOOK)(int isrgbadata,unsigned char *s3tcdata,unsigned int bytes,unsigned char **rawdata,unsigned int *rawbytes,int width,int height,void *data);
   static void *AUTODECOMPRESS_DATA;

   static void (*INTERPRETER_INIT)(unsigned int implformat,char *code,int bytes,databuf *obj,void *data);
   static void (*INTERPRETER_HOOK)(float *value,int comps,float x,float y,float z,float t,databuf *obj,void *data);
   static void *INTERPRETER_DATA;

   void writeparam(const char *tag,double v,FILE *file,int digits=10);
   int readparam(const char *tag,double *v,FILE *file);

   int readparamf(const char *tag,float *v,FILE *file);
   int readparami(const char *tag,int *v,FILE *file);
   int readparamu(const char *tag,unsigned int *v,FILE *file);

   void writestring(const char *tag,const char *str,FILE *file);
   int readstring(const char *tag,char **str,FILE *file);

   void loadblock(FILE *file);

   char *readoneline(FILE *file);

   void convertchunk(int israw,unsigned int extfmt);
   void interpretechunk(unsigned int implfmt);

   void autocompress_mipmaps(int isrgbadata,unsigned char **s3tcdata,unsigned int *s3tcbytes);

   void swapbytes();
   };

#endif
