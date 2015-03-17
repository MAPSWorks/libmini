// (c) by Stefan Roettger

#include "minibase.h"

#include "minimath.h"
#include "minisort.h"

#include "minicurve.h"

void minicurve::set_orb(int orb)
   {
   crs_orb=orb;
   valid=FALSE;
   }

void minicurve::append(const minimeas &p)
   {
   minidyna<minimeas>::append(p);
   valid=FALSE;
   }

void minicurve::append(const minicurve &c)
   {
   minidyna<minimeas>::append(c);
   valid=FALSE;
   }

void minicurve::append_sector(const minicoord &p1,const minicoord &p2,
                              unsigned int n)
   {
   int maxlevel;

   minicoord a=p1;
   minicoord b=p2;

   a.convert2ecef();
   b.convert2ecef();

   if (n<2) n=2;

   maxlevel=ceil(log((double)n)/log(2.0)-0.5);
   bisect(a,b,0,maxlevel-1);
   }

void minicurve::append_sector(const minicoord &p,
                              unsigned int n)
   {
   if (empty()) append(p);
   else append_sector(last(),p,n);
   }

void minicurve::append_point(const minicoord &p,double t,
                             unsigned int n)
   {
   minicoord p0=p;

   p0.vec.w=t;
   append_sector(p0,n);
   }

void minicurve::bisect(const minicoord &p1,const minicoord &p2,
                       int level,int maxlevel)
   {
   minicoord a=p1;
   minicoord b=p2;

   a.convert2llh();
   b.convert2llh();

   double h1=a.vec.z;
   double h2=b.vec.z;

   minicoord p12=(p1+p2)/2.0;
   double h12=(h1+h2)/2.0;

   p12.convert2llh();
   p12.vec.z=h12;
   p12.convert2ecef();

   if (level==0)
      if (getsize()==0) append(p1);
      else if (p1!=(minicoord)last()) append(p1);

   if (level<maxlevel)
      {
      bisect(p1,p12,level+1,maxlevel);
      append(p12);
      bisect(p12,p2,level+1,maxlevel);
      }

   if (level==0) append(p2);
   }

void minicurve::dispose(unsigned int i)
   {
   minidyna<minimeas>::dispose(i);
   valid=FALSE;
   }

void minicurve::sort()
   {shellsort<minimeas>(*this);}

void minicurve::validate()
   {
   unsigned int i,j;

   double t;

   if (!valid)
      {
      // check for missing time
      for (i=0; i<getsize();)
         if (isNAN(get(i).vec.w)) dispose(i);
         else i++;

      // sort by time
      sort();
      valid=TRUE;

      // check for missing time step
      if (get_time_step_max()==0.0)
         for (i=0; i<getsize(); i++)
            {
            t=(double)i/(getsize()-1);
            t=curve_start+t*(curve_stop-curve_start);
            ref(i).vec.w=t;
            }
      else
         // check for double time step
         for (i=0; i+1<getsize();)
            if (get(i).vec.w==get(i+1).vec.w) dispose(i+1);
            else i++;

      // check for missing points
      for (i=0; i<getsize();)
         if (get(i).type==minicoord::MINICOORD_LLH ||
             get(i).type==minicoord::MINICOORD_UTM ||
             get(i).type==minicoord::MINICOORD_MERC)
            if (isNAN(get(i).vec.x) || isNAN(get(i).vec.y)) dispose(i);
            else i++;
         else
            if (isNAN(get(i).vec.x) || isNAN(get(i).vec.y) || isNAN(get(i).vec.z)) dispose(i);
            else i++;

      // check for missing heights
      for (i=0; i<getsize(); i++)
         if (get(i).type==minicoord::MINICOORD_LLH ||
             get(i).type==minicoord::MINICOORD_UTM ||
             get(i).type==minicoord::MINICOORD_MERC)
            if (isNAN(get(i).vec.z))
               {
               // reduplicate height from previous points
               for (j=i; j>0;)
                  {
                  --j;
                  if (get(j).type==minicoord::MINICOORD_LLH ||
                      get(j).type==minicoord::MINICOORD_UTM ||
                      get(j).type==minicoord::MINICOORD_MERC)
                     if (!isNAN(get(j).vec.z))
                        {
                        ref(i).vec.z=get(j).vec.z;
                        break;
                        }
                  }

               // reduplicate height from following points
               if (isNAN(get(i).vec.z))
                  for (j=i; j<getsize();)
                     {
                     j++;
                     if (get(j).type==minicoord::MINICOORD_LLH ||
                         get(j).type==minicoord::MINICOORD_UTM ||
                         get(j).type==minicoord::MINICOORD_MERC)
                        if (!isNAN(get(j).vec.z))
                           {
                           ref(i).vec.z=get(j).vec.z;
                           break;
                           }
                     }
               }

      // replace missing heights
      for (i=0; i<getsize(); i++)
         if (get(i).type==minicoord::MINICOORD_LLH ||
             get(i).type==minicoord::MINICOORD_UTM ||
             get(i).type==minicoord::MINICOORD_MERC)
            if (isNAN(get(i).vec.z)) ref(i).vec.z=0.0;

      // convert to ecef
      for (i=0; i<getsize(); i++)
         ref(i).convert2(minicoord::MINICOORD_ECEF,0,minicoord::MINICOORD_DATUM_NONE,crs_orb);

      // check for maximum time difference and travelled distance
      for (i=1; i<getsize(); i++)
         if (!get(i).start)
            {
            double t1=get(i-1).vec.w;
            double t2=get(i).vec.w;

            if (t2-t1>max_delta) ref(i).start=TRUE;

            miniv3d p1=get(i-1).getpos();
            miniv3d p2=get(i).getpos();

            double d=(p2-p1).getlength();

            if (d>max_length) ref(i).start=TRUE;
            }

      // remove consecutive start points
      for (i=0; i+1<getsize();)
         if (get(i).start && get(i+1).start) dispose(i);
         else i++;

      // remove trailing start points
      while (!empty())
         if (last().start) pop_back();
         else break;

      // apply constraints
      for (i=1; i+1<getsize();)
         if (!get(i).start)
            {
            double dt=get(i).gettime()-get(i-1).gettime();

            miniv3d p1=get(i-1).getpos();
            miniv3d p2=get(i).getpos();

            double d=(p2-p1).getlength();

            double v1=compute_velocity(i-1);
            double v2=compute_velocity(i);

            double a1=get(i-1).accuracy;
            double a2=get(i).accuracy;

            if (isNAN(a1)) a1=0.0;
            if (isNAN(a2)) a2=0.0;

            if (!check_constraints(d,dt,p1,p2,v1,v2,a1,a2)) dispose(i);
            else i++;
            }
         else i++;

      // check for missing velocity
      for (i=0; i<getsize(); i++)
         if (isNAN(get(i).velocity))
            ref(i).velocity=compute_velocity(i);

      // initialize bbox
      bboxmin=miniv3d(MAXFLOAT,MAXFLOAT,MAXFLOAT);
      bboxmax=miniv3d(-MAXFLOAT,-MAXFLOAT,-MAXFLOAT);

      // compute bbox
      for (i=0; i<getsize(); i++)
         {
         miniv3d p=get(i).vec;

         if (p.x<bboxmin.x) bboxmin.x=p.x;
         if (p.x>bboxmax.x) bboxmax.x=p.x;

         if (p.y<bboxmin.y) bboxmin.y=p.y;
         if (p.y>bboxmax.y) bboxmax.y=p.y;

         if (p.z<bboxmin.z) bboxmin.z=p.z;
         if (p.z>bboxmax.z) bboxmax.z=p.z;
         }
      }
   }

void minicurve::set_time_mapping(double map_start,double map_stop)
   {
   curve_map_start=map_start;
   curve_map_stop=map_stop;
   }

void minicurve::set_time_repeat(double repeat_start,double repeat_stop)
   {
   curve_repeat_start=repeat_start;
   curve_repeat_stop=repeat_stop;
   }

minimeas minicurve::interpolate(double t)
   {return(interpolate_cubic(t));}

minimeas minicurve::interpolate_cubic(double t)
   {
   double tt;

   validate();

   if (curve_repeat_start<curve_repeat_stop)
      {
      t=(t-curve_repeat_start)/(curve_repeat_stop-curve_repeat_start);
      t-=floor(t);
      t=curve_repeat_start+t*(curve_repeat_stop-curve_repeat_start);
      }

   if (curve_map_start<curve_map_stop)
      {
      if (t<curve_map_start) t=curve_map_start;
      if (t>curve_map_stop) t=curve_map_stop;
      }

   tt=(t-get_time_start())/(get_time_stop()-get_time_start());

   return(minidyna<minimeas>::interpolate_cubic_directional(tt));
   }

double minicurve::get_time_start()
   {
   validate();
   return(first().vec.w);
   }

double minicurve::get_time_stop()
   {
   validate();
   return(last().vec.w);
   }

double minicurve::get_time_period()
   {
   validate();
   return(last().vec.w-first().vec.w);
   }

double minicurve::get_time_step_min()
   {
   unsigned int i;

   double dt,mdt;

   validate();

   if (getsize()<2) return(0.0);

   mdt=get(1).vec.w-get(0).vec.w;

   for (i=1; i<getsize()-1; i++)
      {
      minimeas a=get(i);
      minimeas b=get(i+1);

      if (!b.start)
         {
         dt=b.vec.w-a.vec.w;
         if (dt<mdt) mdt=dt;
         }
      }

   return(mdt);
   }

double minicurve::get_time_step_max()
   {
   unsigned int i;

   double dt,mdt;

   validate();

   if (getsize()<2) return(0.0);

   mdt=get(1).vec.w-get(0).vec.w;

   for (i=1; i<getsize()-1; i++)
      {
      minimeas a=get(i);
      minimeas b=get(i+1);

      if (!b.start)
         {
         dt=b.vec.w-a.vec.w;
         if (dt>mdt) mdt=dt;
         }
      }

   return(mdt);
   }

double minicurve::get_time_step_avg()
   {
   unsigned int i;

   double dt,sdt;
   unsigned int c;

   validate();

   if (getsize()<2) return(0.0);

   sdt=get(1).vec.w-get(0).vec.w;
   c=1;

   for (i=1; i<getsize()-1; i++)
      {
      minimeas a=get(i);
      minimeas b=get(i+1);

      if (!b.start)
         {
         dt=b.vec.w-a.vec.w;
         sdt+=dt;
         c++;
         }
      }

   return(sdt/c);
   }

double minicurve::get_length()
   {
   unsigned int i;

   double length;

   validate();

   if (getsize()<2) return(0.0);

   length=0.0;

   for (i=0; i<getsize()-1; i++)
      {
      minimeas a=get(i);
      minimeas b=get(i+1);

      if (!b.start)
         {
         if (isNAN(a.vec.z) || isNAN(b.vec.z)) a.vec.z=b.vec.z=0.0;
         length+=(a.getpos()-b.getpos()).getlength();
         }
      }

   return(length);
   }

void minicurve::resample(double dt)
   {
   double t,rt;
   double t0,t1;

   unsigned int idx;
   double ta,tb;

   minicurve curve;

   validate();

   if (getsize()<2 || dt<=0.0) return;

   t0=get_time_start();
   t1=get_time_stop();

   t=t0;
   idx=1;

   curve.append(minidyna<minimeas>::interpolate_cubic_directional(0.0));

   do
      {
      t+=dt;
      if (t>t1) t=t1;

      while (get(idx).vec.w<t) idx++;

      ta=get(idx-1).vec.w;
      tb=get(idx).vec.w;

      rt=(idx-1+(t-ta)/(tb-ta))/(getsize()-1);

      curve.append(minidyna<minimeas>::interpolate_cubic_directional(rt));
      }
   while (t<t1-0.5*dt);

   *this=curve;

   valid=TRUE;
   }

// get bounding box
void minicurve::getbbox(miniv3d &bboxmin,miniv3d &bboxmax)
   {
   validate();

   bboxmin=this->bboxmin;
   bboxmax=this->bboxmax;
   }

// get bounding sphere
void minicurve::getbsphere(miniv3d &center,double &radius2)
   {
   validate();

   center=0.5*(bboxmin+bboxmax);
   radius2=(0.5*(bboxmax-bboxmin)).getlength2();
   }

// serialization
ministring minicurve::to_string()
   {
   unsigned int i;

   ministring info("minicurve");

   validate();

   info.append("(");

   info.append_double(curve_start);
   info.append(",");
   info.append_double(curve_stop);
   info.append(",");
   info.append_double(curve_map_start);
   info.append(",");
   info.append_double(curve_map_stop);
   info.append(",");
   info.append_double(curve_repeat_start);
   info.append(",");
   info.append_double(curve_repeat_stop);

   info.append(",[");

   for (i=0; i<getsize(); i++)
      {
      info.append(get(i).to_string());
      if (i+1<getsize()) info.append(",");
      }

   info.append("]");

   info.append(")");

   return(info);
   }

// deserialization
void minicurve::from_string(ministring &info)
   {
   if (info.startswith("minicurve"))
      {
      info=info.tail("minicurve(");

      curve_start=info.prefix(",").value();
      info=info.tail(",");
      curve_stop=info.prefix(",").value();
      info=info.tail(",");
      curve_map_start=info.prefix(",").value();
      info=info.tail(",");
      curve_map_stop=info.prefix(",").value();
      info=info.tail(",");
      curve_repeat_start=info.prefix(",").value();
      info=info.tail(",");
      curve_repeat_stop=info.prefix(")").value();
      info=info.tail(",[");

      while (!info.startswith("]"))
         {
         minimeas meas;

         meas.from_string(info);
         append(meas);

         if (info.startswith(",")) info=info.tail(",");
         else break;
         }

      info=info.tail("]");

      info=info.tail(")");

      valid=FALSE;
      }
   }

// serialize to string list
ministrings minicurve::to_strings()
   {
   unsigned int i;

   ministrings curve;

   ministring info("minicurve");

   validate();

   info.append("(");

   info.append_double(curve_start);
   info.append(",");
   info.append_double(curve_stop);
   info.append(",");
   info.append_double(curve_map_start);
   info.append(",");
   info.append_double(curve_map_stop);
   info.append(",");
   info.append_double(curve_repeat_start);
   info.append(",");
   info.append_double(curve_repeat_stop);

   info.append(")");

   curve.append(info);

   for (i=0; i<getsize(); i++)
      curve.append(get(i).to_string());

   return(curve);
   }

// deserialize from string list
void minicurve::from_strings(ministrings &infos)
   {
   unsigned int line;

   ministring info;

   if (!infos.empty())
      {
      info=infos[0];

      if (info.startswith("minicurve"))
         {
         info=info.tail("minicurve(");

         curve_start=info.prefix(",").value();
         info=info.tail(",");
         curve_stop=info.prefix(",").value();
         info=info.tail(",");
         curve_map_start=info.prefix(",").value();
         info=info.tail(",");
         curve_map_stop=info.prefix(",").value();
         info=info.tail(",");
         curve_repeat_start=info.prefix(",").value();
         info=info.tail(",");
         curve_repeat_stop=info.prefix(")").value();
         info=info.tail(")");

         if (!info.empty()) return;

         line=1;

         while (line<infos.getsize())
            {
            minimeas meas;

            info=infos[line];
            meas.from_string(info);

            if (!info.empty()) return;

            append(meas);

            infos[line].clear();
            }

         infos.clear();

         valid=FALSE;
         }
      }
   }

// compute velocity
double minicurve::compute_velocity(unsigned int i)
   {
   // original value
   if (!isNAN(get(i).velocity)) return(get(i).velocity);

   // forward difference
   if (i==0 || get(i).start)
      if (i+1<getsize())
         return((get(i+1).getpos()-get(i).getpos()).getlength()/(get(i+1).vec.w-get(i).vec.w));
      else
         return(0.0);
   // backward difference
   else if (i==getsize()-1)
      if (i>0)
         return((get(i).getpos()-get(i-1).getpos()).getlength()/(get(i).vec.w-get(i-1).vec.w));
      else
         return(0.0);
   // central difference
   else
      return((get(i+1).getpos()-get(i-1).getpos()).getlength()/(get(i+1).vec.w-get(i-1).vec.w));
   }

// check constraints
BOOLINT minicurve::check_constraints(double d,double dt,
                                     miniv3d p1,miniv3d p2,double v1,double v2,
                                     double a1,double a2)
   {
   // check for minimum accuracy threshold
   if (a2>min_accuracy) return(FALSE);

   // check for maximum acceleration threshold
   if (dabs(v2-v1)/dt>max_acceleration) return(FALSE);

   // check for valid geometric point distance vs. maximum travelled distance
   if (v2*dt*max_tolerance+a1+a2<d) return(FALSE);

   return(TRUE);
   }
