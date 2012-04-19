// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef OBJECTS_H
#define OBJECTS_H

#include <mini/mini_generic.h>
#include <mini/mini_tileset.h>
#include <mini/mini_object.h>

class Viewer;

//! object (base class)
class Object
   {
   public:

   Object(const ministring &name="",const ministring &repo="");
   virtual ~Object();

   ministring repository; // data repository
   ministring filename; // data file in repository

   minicoord get_coord() {return(coord);}
   double get_radius() {return(radius);}

   virtual BOOLINT initGFX() = 0;
   virtual void exitGFX() = 0;

   virtual void focus() {}

   protected:

   minicoord coord; // object barycenter
   double radius; // object radius
   };

//! tileset object
class Object_tileset: public Object
   {
   public:

   Object_tileset(const ministring &name="",const ministring &repo="",
                  Viewer *viewer=NULL);

   virtual ~Object_tileset();

   virtual BOOLINT initGFX();
   virtual void exitGFX();

   virtual void focus();

   protected:

   Viewer *tileset_viewer;
   minilayer *tileset_layer;
   };

//! object container
class Objects: public minikeyval<Object *>
   {
   public:

   //! default constructor
   Objects();

   //! default destructor
   ~Objects();

   BOOLINT add(const ministring &key,Object *obj,const ministring &tag);
   BOOLINT add(const ministring &key,Object *obj,const ministrings &tags);
   unsigned int get_num();
   Object *get(unsigned int i);
   Object *get(const ministring &key);
   ministrings *get_tags(const ministring &key);
   ministrings list();
   ministrings list(const ministring &tag);
   ministrings list(const ministrings &tags);
   void remove(const ministring &key);
   void clear();
   };

#endif
