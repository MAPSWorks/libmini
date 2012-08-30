// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINICRS_H
#define MINICRS_H

#include <math.h>

class minicrs
   {
   public:

   static const double EARTH_radius; // radius of the earth

   static const double WGS84_r_major; // WGS84 semi-major axis
   static const double WGS84_r_minor; // WGS84 semi-minor axis
   static const double WGS84_f;       // WGS84 flattening
   static const double WGS84_e2;      // WGS84 eccentricity squared
   static const double WGS84_ed2;     // WGS84 eccentricity derived
   static const double WGS84_e;       // WGS84 eccentricity

   //! transform Lat/Lon to UTM
   static void LL2UTM(double lat,double lon, // geographic input coordinates in arc-seconds (WGS84 datum)
                      int zone,int datum, // UTM zone and datum of output coordinates
                      double *x,double *y); // output UTM coordinates (Easting and Northing)

   static void LL2UTM(double lat,double lon,
                      int zone,int datum,
                      float *x,float *y);

   //! transform UTM to Lat/Lon
   static void UTM2LL(double x,double y, // input UTM coordinates (Easting and Northing)
                      int zone,int datum, // UTM zone and datum of input coordinates
                      double *lat,double *lon); // geographic output coordinates in arc-seconds (WGS84 datum)

   static void UTM2LL(double x,double y,
                      int zone,int datum,
                      float *lat,float *lon);

   //! transform Lat/Lon to UTM zone
   static int LL2UTMZ(double lat,double lon);

   //! transform UTM zone to Lon
   static double UTMZ2L(int zone);

   //! transform Lat/Lon to Mercator
   static void LL2MERC(double lat,double lon, // geographic input coordinates in arc-seconds (WGS84 datum)
                       double *x,double *y); // output Mercator coordinates (WGS84 datum)

   static void LL2MERC(double lat,double lon,
                       float *x,float *y);

   //! transform Mercator to Lat/Lon
   static void MERC2LL(double x,double y, // input Mercator coordinates (WGS84 datum)
                       double *lat,double *lon); // geographic output coordinates in arc-seconds (WGS84 datum)

   static void MERC2LL(double x,double y,
                       float *lat,float *lon);

   //! transform Lat/Lon/H to ECEF
   static void LLH2ECEF(double lat,double lon,double h, // geographic input coordinates in arc-seconds (WGS84 datum)
                        double xyz[3]); // output ECEF coordinates

   static void LLH2ECEF(double lat,double lon,double h,
                        float xyz[3]);

   //! transform ECEF to Lat/Lon/H
   static void ECEF2LLH(double xyz[3], // input ECEF coordinates
                        double *lat,double *lon,double *h); // geographic output coordinates in arc-seconds (WGS84 datum)

   static void ECEF2LLH(float xyz[3],
                        float *lat,float *lon,float *h);

   //! transform OG/H to ECEF
   static void OGH2ECEF(double x,double y,double h, // oblique gnomonic input coordinates in meters
                        int zone, // oblique gnomonic zone of input coordinates
                        double xyz[3]); // output ECEF coordinates

   static void OGH2ECEF(double x,double y,double h,
                        int zone,
                        float xyz[3]);

   //! transform ECEF to OG/H
   static void ECEF2OGH(double xyz[3], // input ECEF coordinates
                        double *x,double *y,double *h, // oblique gnomonic output coordinates in meters
                        int zone); // oblique gnomonic zone of output coordinates

   static void ECEF2OGH(float xyz[3],
                        float *x,float *y,float *h,
                        int zone);

   //! transform ECEF to OG/H zone
   static int ECEF2OGHZ(double xyz[3]); // input ECEF coordinates

   //! project ECEF to ellipsoid
   static void ECEF2PRJ(double xyz[3], // input ECEF coordinates
                        double prj[3], // output ECEF coordinates
                        double *h); // output altitude

   //! 1 arc-second equals about 30 meters
   static void arcsec2meter(double lat,double *as2m);
   static void arcsec2meter(double lat,float *as2m);

   protected:

   // UTM conversion functions
   static void initUTM(int zone,double ratio);
   static void calcLL2UTM(double lat,double lon,double *x,double *y);
   static void calcUTM2LL(double x,double y,double *lat,double *lon);

   // Mercator conversion functions
   static void calcLL2MERC(double lat,double lon,double *x,double *y,double lat_center,double lon_center);
   static void calcMERC2LL(double x,double y,double *lat,double *lon,double lat_center,double lon_center);

   // Molodensky transformation between two datums
   static void molodensky(int src,int dst,double *lat,double *lon);

   // Molodensky transformation based on ellipsoid change
   static void molodensky(double *lat,double *lon,double *h, // transformed coordinates
                          double r_maj,double f,             // semi-major axis and flattening
                          double dr_maj,double df,           // ellipsoid change
                          double dx,double dy,double dz);    // origin change

   private:

   static int crs_datum;
   static int crs_zone;
   static double crs_ratio;

   static double r_major,r_minor;
   static double o_dx,o_dy,o_dz;

   static double scale_factor;

   static double lon_center;
   static double e0,e1,e2,e3;
   static double e,es,esp;
   static double false_northing;
   static double false_easting;

   static void choose_ellipsoid(double r_maj,double r_min);
   static void choose_ellipsoid_flat(double r_maj,double f);

   static void choose_datum(int ellipsoid,
                            double dx,double dy,double dz);

   static void choose_datum(int datum);
   };

namespace minilon {

// longitude arithmetic:

//! add longitudes
inline double LONADD(double a,double b=0.0f)
   {
   double lon=a+b;
   double lonmul=floor(lon/(360*60*60));
   if (lonmul<0.0 || lonmul>1.0) lon-=lonmul*360*60*60;
   return(lon);
   }

//! subtract longitudes
inline double LONSUB(double a,double b=0.0f)
   {
   double diff=LONADD(a,-b);
   return(diff>180*60*60?diff-360*60*60:diff);
   }

//! return leftmost longitude
inline double LONLEFT(double a,double b)
   {
   if (LONSUB(a,b)<0.0) return(a);
   else return(b);
   }

//! return rightmost longitude
inline double LONRIGHT(double a,double b)
   {
   if (LONSUB(a,b)>0.0) return(a);
   else return(b);
   }

//! linear interpolation of longitudes
inline double LONLERP(double a,double b,double lerp=0.5)
   {
   double diff=LONADD(b,-a);
   return(LONSUB(a,-lerp*diff));
   }

//! average longitudes
inline double LONMEAN(double a,double b,double weight=0.5)
   {
   if (LONSUB(a,b)<0.0) return(LONLERP(a,b,weight));
   else return(LONLERP(b,a,1.0-weight));
   }

}

using namespace minilon;

#endif