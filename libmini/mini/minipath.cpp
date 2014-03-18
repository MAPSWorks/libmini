// (c) by Stefan Roettger

#include "minipath.h"

// serialization
ministrings minipath::to_csv() const
   {
   unsigned int i;

   ministrings csv;

   csv.append("\"name\",\"activity\",\"description\"");
   csv.append("\""+name+"\",\""+activity+"\",\""+description+"\"");

   csv.append("");

   csv.append("\"segment\",\"point\",\"lat\",\"lon\",\"height\",\"heading\",\"accuracy\",\"velocity\",\"time\"");

   for (i=0; i<getsize(); i++)
      {
      ministring line;
      minimeas meas;

      meas=get(i);
      meas.convert2llh();

      // segment
      line.append("\"");
      line.append_uint(meas.segment);
      line.append("\"");

      // point
      line.append("\"");
      line.append_uint(i);
      line.append("\"");

      // lat
      line.append(",\"");
      line.append_double(meas.vec.y/3600);
      line.append("\"");

      // lon
      line.append(",\"");
      line.append_double(meas.vec.x/3600);
      line.append("\"");

      // height
      line.append(",\"");
      line.append_double(meas.vec.z);
      line.append("\"");

      // heading
      line.append(",\"");
      line.append_double(meas.heading);
      line.append("\"");

      // accuracy
      line.append(",\"");
      line.append_double(meas.accuracy);
      line.append("\"");

      // velocity
      line.append(",\"");
      line.append_double(meas.velocity);
      line.append("\"");

      // time
      line.append(",\"");
      line.append_double(meas.vec.w);
      line.append("\"");

      csv.append(line);
      }

   return(csv);
   }

// deserialization
void minipath::from_csv(ministrings &csv)
   {
   unsigned int i,j;

   ministrings values;

   if (csv.getsize()<2) return;

   values.from_string(csv[0],",");

   if (values.getsize()<3) return;

   values.from_string(csv[1],",");

   if (values.getsize()<3) return;

   name=values[0].tail("\"").head("\"");
   activity=values[1].tail("\"").head("\"");
   description=values[2].tail("\"").head("\"");

   for (i=2; i<csv.getsize(); i++)
      if (!csv[i].empty()) break;

   values.from_string(csv[i],",");

   if (values.getsize()<9) return;

   for (i++; i<csv.getsize(); i++)
      if (!csv[i].empty())
         {
         minimeas meas;

         values.from_string(csv[i],",");

         if (values.getsize()<9) return;

         for (j=0; j<values.getsize(); j++)
            values[j]=values[j].tail("\"").head("\"");

         meas.set_llh(values[2].value(),values[3].value(),values[4].value(),values[8].value());

         meas.accuracy=values[6].value();
         meas.velocity=values[7].value();
         meas.heading=values[5].value();

         meas.segment=values[0].value_uint();

         append(meas);

         csv[i].clear();
         }

   csv.clear();
   }

BOOLINT minipath::load(ministring filename)
   {
   BOOLINT success;

   ministrings path;
   ministring info;

   path.load(filename);

   if (path.empty()) return(FALSE);

   info=path[0];

   if (info.startswith("minicurve")) success=read_curve_format(path);
   else if (info.startswith("\"")) success=read_csv_format(path);
   else if (info.startswith("<?xml")) success=read_gpx_format(path);
   else if (info=="[track]") success=read_trk_format(path);
   else success=FALSE;

   return(success);
   }

void minipath::save(ministring filename)
   {to_strings().save(filename);}

BOOLINT minipath::read_curve_format(ministrings &curve)
   {
   from_strings(curve);

   return(curve.empty());
   }

BOOLINT minipath::read_csv_format(ministrings &csv)
   {
   from_csv(csv);

   return(csv.empty());
   }

BOOLINT minipath::read_gpx_format(ministrings &gpx)
   {
   if (gpx.empty()) return(FALSE);

   return(FALSE);
   }

BOOLINT minipath::read_trk_format(ministrings &trk)
   {
   unsigned int i,j;

   unsigned int segment=1;

   if (trk.empty()) return(FALSE);

   if (trk[0]=="[track]")
      for (i=1; i<trk.getsize(); i++)
         if (trk[i]=="--start--")
            {
            for (j=i+1; j<trk.getsize(); j++)
               {
               ministring line=trk[j];

               if (line.startswith("("))
                  {
                  double lat,lon,elev,time;

                  line=line.tail("(");
                  lat=line.prefix(",").value();
                  line=line.tail(",");
                  lon=line.prefix(",").value();
                  line=line.tail(",");
                  elev=line.prefix(",").value();
                  line=line.tail(",");
                  time=line.prefix(",").value();
                  line=line.tail(")");

                  minicoord coord(miniv4d(lon*3600,lat*3600,elev,time),minicoord::MINICOORD_LLH);

                  ministring desc;

                  if (line.startswith(";"))
                     {
                     line=line.tail(";");

                     if (line=="#CMDNewSegment") segment++;
                     else desc=line;

                     line.clear();
                     }

                  minimeas meas(coord,0.0,0.0,0.0,segment);
                  meas.set_description(desc);

                  append(meas);
                  }

               if (!line.empty()) return(FALSE);
               }

            return(TRUE);
            }

   return(FALSE);
   }
