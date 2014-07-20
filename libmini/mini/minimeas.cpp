// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "minimeas.h"

// serialization
ministring minimeas::to_string() const
   {
   ministring info("minimeas");

   ministring desc,meta;

   desc=get_description();
   desc.substitute("\"","");

   meta=get_metadata();

   info.append("(");
   info.append(minicoord::to_string());
   info.append(",");
   info.append_double(accuracy);
   info.append(",");
   info.append_double(velocity);
   info.append(",");
   info.append_double(heading);
   info.append(",");
   info.append_double(inclination);
   info.append(",");
   info.append_uint(start);
   info.append(",");
   info.append_double(power);
   info.append(",");
   info.append_double(frequency);
   info.append(",");
   info.append_double(heartbeat);
   info.append(",");
   info.append("\""+desc+"\"");
   info.append(",");
   info.append("\""+meta+"\"");
   info.append(")");

   return(info);
   }

// deserialization
void minimeas::from_string(ministring &info)
   {
   ministring desc,meta;

   if (info.startswith("minimeas"))
      {
      info=info.tail("minimeas(");
      minicoord::from_string(info);
      info=info.tail(",");
      accuracy=info.prefix(",").value();
      info=info.tail(",");
      velocity=info.prefix(",").value();
      info=info.tail(",");
      heading=info.prefix(",").value();
      info=info.tail(",");
      inclination=info.prefix(",").value();
      info=info.tail(",");
      start=info.prefix(",").value_uint();
      info=info.tail(",");
      power=info.prefix(",").value();
      info=info.tail(",");
      frequency=info.prefix(",").value();
      info=info.tail(",");
      heartbeat=info.prefix(",").value();
      info=info.tail(",\"");
      desc=info.prefix("\",");
      info=info.tail(",\"");
      meta=info.prefix("\")");
      info=info.tail("\")");

      set_description(desc);
      set_metadata(meta);
      }
   }
