// (c) by Stefan Roettger, licensed under GPL 2+

#include "objectlist.h"

Objects::Objects()
   : minikeyval<Object *>()
   {}

Objects::~Objects()
   {}

unsigned int Objects::key_number=0;

ministring Objects::newkey()
   {
   ministring key;

   key="object_nr";
   key.append_uint(++key_number);
   key+="_";
   key.append_random(10);

   return(key);
   }

int Objects::add(const ministring &key,Object *obj,const ministring &tag)
   {
   int errorcode=OBJECT_FAILURE;

   MINILOG("adding object with key=" + key + " and tag=" + tag);

   if (minikeyval<Object *>::add(key,obj,tag))
      {
      errorcode=obj->initGFX();

      if (errorcode==OBJECT_SUCCESS)
         {
         MINILOG((ministring)" object extent: " + obj->get_center());
         MINILOG((ministring)" object size: " + obj->get_radius());
         }
      else if (errorcode==OBJECT_NOT_REFERENCED)
         MINILOG((ministring)" object is not geo-referenced");
      else
         {
         minikeyval<Object *>::remove(key);

         MINILOG((ministring)" object failed to initialize");
         }
      }
   else
      MINILOG((ministring)" object failed to be added");

   return(errorcode);
   }

int Objects::add(const ministring &key,Object *obj,const ministrings &tags)
   {
   int errorcode=OBJECT_FAILURE;

   MINILOG("adding object with key=" + key + " and tags=" + tags.to_string());

   if (minikeyval<Object *>::add(key,obj,tags))
      {
      errorcode=obj->initGFX();

      if (errorcode==OBJECT_SUCCESS)
         {
         MINILOG((ministring)" object extent: " + obj->get_center());
         MINILOG((ministring)" object size: " + obj->get_radius());
         }
      else if (errorcode==OBJECT_NOT_REFERENCED)
         MINILOG((ministring)" object is not geo-referenced");
      else
         {
         minikeyval<Object *>::remove(key);

         MINILOG((ministring)" object failed to initialize");
         }
      }
   else
      MINILOG((ministring)" object failed to be added");

   return(errorcode);
   }

unsigned int Objects::get_num() const
   {return(minikeyval<Object *>::get_pairs());}

Object *Objects::idx(unsigned int i)
   {
   Object **obj=minikeyval<Object *>::idx(i);
   return(obj?*obj:NULL);
   }

Object *Objects::get(const ministring &key)
   {
   Object **obj=minikeyval<Object *>::get(key);
   return(obj?*obj:NULL);
   }

ministrings *Objects::get_tags(const ministring &key)
   {return(minikeyval<Object *>::get_tags(key));}

void Objects::add_tag(const ministring &key,const ministring &tag)
   {
   MINILOG("adding tag to object with key=" + key + " and tag=" + tag);
   minikeyval<Object *>::tag(key,tag);
   }

void Objects::add_tags(const ministring &key,const ministrings &tags)
   {
   MINILOG("adding tags to object with key=" + key + " and tags=" + tags.to_string(","));
   minikeyval<Object *>::tag(key,tags);
   }

void Objects::remove_tag(const ministring &key,const ministring &tag)
   {
   MINILOG("removing tag from object with key=" + key + " and tag=" + tag);
   minikeyval<Object *>::untag(key,tag);
   }

BOOLINT Objects::has_tag(const ministring &key,const ministring &tag)
   {return(minikeyval<Object *>::has_tag(key,tag));}

ministrings Objects::list()
   {return(minikeyval<Object *>::get_items());}

ministrings Objects::list(const ministring &tag)
   {return(minikeyval<Object *>::get_items(tag));}

ministrings Objects::list(const ministrings &tags)
   {return(minikeyval<Object *>::get_items(tags));}

ministrings Objects::list_wo(const ministring &tag)
   {return(minikeyval<Object *>::get_items_wo(tag));}

ministrings Objects::list_wo(const ministrings &tags)
   {return(minikeyval<Object *>::get_items_wo(tags));}

void Objects::set_repo(const ministring &repo)
   {
   unsigned int i;

   MINILOG("change repository to " + repo);

   for (i=0; i<get_num(); i++)
      {
      ministring path=idx(i)->get_full_name();

      idx(i)->set_relative_path(repo);
      idx(i)->set_relative_name(path);
      }
   }

void Objects::show(const ministring &key,BOOLINT yes)
   {
   Object *object=get(key);
   if (object!=NULL) object->show(yes);
   }

void Objects::focus(const ministring &key)
   {
   Object *object=get(key);
   if (object!=NULL) object->focus();
   }

void Objects::mark(const ministring &key,BOOLINT yes)
   {
   Object *object=get(key);

   if (object!=NULL)
      {
      Object_extents *extent=dynamic_cast<Object_extents *>(object);
      if (extent!=NULL) extent->mark(yes);
      }
   }

void Objects::move(const ministring &key,const minicoord &pos0,const minicoord &pos1)
   {
   Object *object=get(key);

   if (object!=NULL)
      {
      Object_extents *extent=dynamic_cast<Object_extents *>(object);

      if (extent!=NULL)
         {
         extent->move(pos0,pos1);
         extent->updateGFX();
         }
      }
   }

void Objects::rotate(const ministring &key,const minicoord &pos0,const minicoord &pos1)
   {
   Object *object=get(key);

   if (object!=NULL)
      {
      Object_extents *extent=dynamic_cast<Object_extents *>(object);

      if (extent!=NULL)
         {
         extent->rotate(pos0,pos1);
         extent->updateGFX();
         }
      }
   }

void Objects::scale_ds(const ministring &key,const minicoord &pos0,const minicoord &pos1)
   {
   Object *object=get(key);

   if (object!=NULL)
      {
      Object_extents *extent=dynamic_cast<Object_extents *>(object);

      if (extent!=NULL)
         {
         extent->scale_ds(pos0,pos1);
         extent->updateGFX();
         }
      }
   }

void Objects::scale_dt(const ministring &key,const minicoord &pos0,const minicoord &pos1)
   {
   Object *object=get(key);

   if (object!=NULL)
      {
      Object_extents *extent=dynamic_cast<Object_extents *>(object);

      if (extent!=NULL)
         {
         extent->scale_dt(pos0,pos1);
         extent->updateGFX();
         }
      }
   }

void Objects::remove(const ministring &key)
   {
   MINILOG("removing object with key=" + key);

   Object *obj=get(key);

   if (obj)
      {
      minikeyval<Object *>::remove(key);
      obj->exitGFX();
      delete obj;
      }
   }

void Objects::remove(const ministrings &keys)
   {
   unsigned int i;

   for (i=0; i<keys.getsize(); i++)
      remove(keys[i]);
   }

void Objects::clear()
   {
   unsigned int i;

   for (i=0; i<get_num(); i++)
      {
      Object *obj=idx(i);
      obj->exitGFX();
      delete obj;
      }
   }
