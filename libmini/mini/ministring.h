// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINISTRING_H
#define MINISTRING_H

#include "minibase.h"
#include "minidyna.h"
#include "minisort.h"

#include "miniio.h"

//! string base class
typedef minidyna<char,16> ministring_base;

//! dynamic string
class ministring: public ministring_base
   {
   public:

   //! default constructor
   ministring(): ministring_base()
      {cstr=NULL;}

   //! copy constructor
   ministring(const ministring_base &a): ministring_base(a)
      {cstr=NULL;}

   //! copy constructor
   ministring(const ministring &a): ministring_base(a)
      {cstr=NULL;}

   //! constructor with copy from c-string
   ministring(const char *str): ministring_base()
      {
      unsigned int i,l;

      cstr=NULL;

      if (str!=NULL) l=strlen(str);
      else l=0;

      setsize(l);
      for (i=0; i<l; i++) set(i,str[i]);
      }

   //! constructor with initialization from floating-point value
   ministring(double v): ministring_base()
      {
      static const int len=32;
      static char str[len];

      if (isNAN(v)) strcpy(str,"NAN");
      else if (dabs(v-dtrc(v+0.5))<1E-7) snprintf(str,len,"%g",v);
      else snprintf(str,len,"%.7f",v);

      cstr=NULL;

      *this=ministring(str);
      }

   //! destructor
   ~ministring()
      {if (cstr!=NULL) free(cstr);}

   //! string length
   unsigned int length() const
      {return(getsize());}

   //! append char
   void append(const char c)
      {ministring_base::append(c);}

   //! append string
   void append(const ministring_base &a)
      {ministring_base::append(a);}

   //! append from c-string
   void append(const char *str)
      {ministring_base::append(ministring(str));}

   //! append floating-point value
   void append(double v)
      {append(ministring(v));}

   //! append double value
   void append_float(float v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%.7e",v);

      append(str);
      }

   //! append double value
   void append_double(double v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%.16e",v);

      append(str);
      }

   //! append integer value
   void append_int(int v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%d",v);

      append(str);
      }

   //! append unsigned integer value
   void append_uint(unsigned int v)
      {
      static const int len=32;
      static char str[len];

      snprintf(str,len,"%u",v);

      append(str);
      }

   //! check for existing sub-string and return first occurring index
   BOOLINT find(const ministring_base &sub,unsigned int &idx,
                unsigned int start=0) const
      {
      unsigned int i;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      for (idx=start; idx+s<=SIZE; idx++)
         {
         found=TRUE;

         for (i=0; i<s; i++)
            if (get(idx+i)!=sub[i])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

   //! check for existing sub-c-string and return first occurring index
   BOOLINT find(const char *sub,unsigned int &idx,unsigned int start=0) const
      {return(find(ministring(sub),idx,start));}

   //! check for existing sub-string in reverse order and return first occurring index
   BOOLINT findr(const ministring_base &sub,unsigned int &idx,
                 unsigned int start=0) const
      {
      unsigned int i,j;

      unsigned int s;

      BOOLINT found;

      s=sub.getsize();

      idx=0;

      if (s==0) return(FALSE);

      if (start<s-1) start=s-1;

      for (i=start; i<SIZE; i++)
         {
         idx=SIZE-1-i;

         found=TRUE;

         for (j=0; j<s; j++)
            if (get(idx+j)!=sub[j])
               {
               found=FALSE;
               break;
               }

         if (found) return(TRUE);
         }

      return(FALSE);
      }

   //! check for existing sub-c-string in reverse order and return first occurring index
   BOOLINT findr(const char *sub,unsigned int &idx,unsigned int start=0) const
      {return(findr(ministring(sub),idx,start));}

   //! check for existing sub-string
   BOOLINT contains(const ministring_base &sub)
      {
      unsigned int idx;
      return(find(sub,idx));
      }

   //! check for existing sub-c-string
   BOOLINT contains(const char *sub)
      {
      unsigned int idx;
      return(find(ministring(sub),idx));
      }

   //! check for existing sub-string and return remaining tail
   ministring tail(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!find(sub,idx)) return(*this);

      for (i=idx+sub.getsize(); i<SIZE; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string and return remaining tail
   ministring tail(const char *sub) const
      {return(tail(ministring(sub)));}

   //! check for existing sub-string in reverse order and return remaining suffix
   ministring suffix(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!findr(sub,idx)) return(*this);

      for (i=idx+sub.getsize(); i<SIZE; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string in reverse order and return remaining suffix
   ministring suffix(const char *sub) const
      {return(suffix(ministring(sub)));}

   //! check for existing sub-string in reverse order and return remaining head
   ministring head(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!findr(sub,idx)) return(*this);

      for (i=0; i<idx; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string in reverse order and return remaining head
   ministring head(const char *sub) const
      {return(head(ministring(sub)));}

   //! check for existing sub-string and return remaining prefix
   ministring prefix(const ministring_base &sub) const
      {
      unsigned int i;

      unsigned int idx;

      ministring rem;

      if (!find(sub,idx)) return(*this);

      for (i=0; i<idx; i++) rem.append(get(i));

      return(rem);
      }

   //! check for existing sub-c-string and return remaining prefix
   ministring prefix(const char *sub) const
      {return(prefix(ministring(sub)));}

   //! check for prefix
   BOOLINT startswith(const ministring_base &prefix) const
      {
      unsigned int idx;

      if (find(prefix,idx))
         if (idx==0) return(TRUE);

      return(FALSE);
      }

   //! check for prefix
   BOOLINT startswith(const char *prefix) const
      {return(startswith(ministring(prefix)));}

   //! check for suffix
   BOOLINT endswith(const ministring_base &suffix) const
      {
      unsigned int idx;

      if (findr(suffix,idx))
         if (idx==getsize()-suffix.getsize()) return(TRUE);

      return(FALSE);
      }

   //! check for suffix
   BOOLINT endswith(const char *suffix) const
      {return(endswith(ministring(suffix)));}

   //! substitute sub-strings
   void substitute(const ministring_base &sub,const ministring_base &with)
      {
      unsigned int i,j;
      unsigned int l,s,w;

      BOOLINT found;

      l=getsize();

      s=sub.getsize();
      w=with.getsize();

      if (s==0) return;

      for (i=0; i+s<=l;)
         {
         found=TRUE;

         for (j=0; j<s; j++)
            if (get(i+j)!=sub[j])
               {
               found=FALSE;
               break;
               }

         if (found)
            {
            if (s>w)
               for (j=i+s; j<l; j++) set(j-s+w,get(j));

            setsize(l-s+w);

            if (s<w)
               for (j=l-1; j>=i+s; j--) set(j-s+w,get(j));

            for (j=0; j<w; j++) set(i+j,with[j]);

            l=getsize();
            i+=w;
            }
         else i++;
         }
      }

   //! substitute c-string
   void substitute(const ministring_base &sub,const char *with)
      {substitute(sub,ministring(with));}

   //! substitute c-strings
   void substitute(const char *sub,const char *with)
      {substitute(ministring(sub),ministring(with));}

   //! copy to c-string
   const char *c_str()
      {
      unsigned int i,l;

      if (cstr!=NULL) free(cstr);

      l=getsize();

      if ((cstr=(char *)malloc(l+1))==NULL) MEMERROR();

      for (i=0; i<l; i++) cstr[i]=get(i);
      cstr[l]='\0';

      return(cstr);
      }

   //! conversion to double value
   double value()
      {
      double v;

      if (sscanf(c_str(),"%lg",&v)!=1) v=NAN;

      return(v);
      }

   //! assignment operator
   ministring& operator = (const ministring &a)
      {
      ministring_base::copy(a);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   //! add operator (concatenate strings)
   ministring &operator += (const ministring &a)
      {
      append(a);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   //! add operator (concatenate c-string)
   ministring &operator += (const char *c)
      {
      append(c);

      if (cstr!=NULL) free(cstr);
      cstr=NULL;

      return(*this);
      }

   private:

   char *cstr;
   };

//! cmp operator
inline int operator == (const ministring &a,const ministring &b)
   {return(ministring_base(a)==ministring_base(b));}

//! neq operator
inline int operator != (const ministring &a,const ministring &b)
   {return(ministring_base(a)!=ministring_base(b));}

//! less than operator (alpha-numerical comparison)
inline int operator < (const ministring &a,const ministring &b)
   {
   unsigned int i;

   unsigned int s;

   s=a.getsize();

   if (s<b.getsize()) return(1);
   if (s>b.getsize()) return(0);

   for (i=0; i<s; i++)
      {
      if (a[i]<b[i]) return(1);
      if (b[i]<a[i]) return(0);
      }

   return(0);
   }

//! add operator (concatenate strings)
inline ministring operator + (const ministring &a,const ministring &b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate c-string)
inline ministring operator + (const ministring &a,const char *b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate c-string)
inline ministring operator + (const char *a,const ministring &b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! add operator (concatenate floating-point value)
inline ministring operator + (const ministring &a,double b)
   {
   ministring str(a);

   str.append(b);

   return(str);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const ministring &a)
   {
   unsigned int i;

   char c;

   out << "\"";

   for (i=0; i<a.getsize(); i++)
      {
      c=a[i];

      if (c=='\n' || c=='\r' || c=='\t') out << ' ';
      else out << c;
      }

   out << "\"";

   return(out);
   }

//! string list
class ministrings: public minidyna<ministring>
   {
   public:

   //! default constructor
   ministrings()
      : minidyna<ministring>()
      {}

   //! copy constructor
   ministrings(const ministrings &strs)
      : minidyna<ministring>(strs)
      {}

   //! custom constructor
   ministrings(const ministring &str)
      : minidyna<ministring>(str)
      {}

   //! concatenate string list
   ministring to_string(ministring separator="/") const
      {
      unsigned int i;

      ministring str;

      for (i=0; i<getsize(); i++)
         {
         str += get(i);
         if (i<getsize()-1) str += separator;
         }

      return(str);
      }

   //! deconcatenate string
   void from_string(const ministring &str,ministring separator="/")
      {
      unsigned int left,right;
      ministring sub;

      left=0;

      while (str.find(separator,right,left))
         {
         sub=str.range(left,right-1);
         append(sub);

         left=right+separator.length();
         }

      if (left<str.length())
         {
         sub=str.range(left,str.length()-1);
         append(sub);
         }
      }

   //! serialize string list
   ministring serialize() const
      {return(to_string("\n"));}

   //! deserialize string
   void deserialize(const ministring &str)
      {from_string(str,"\n");}

   //! save to file
   void save(ministring filename) const
      {writestring(filename.c_str(),serialize().c_str());}

   //! load from file
   void load(ministring filename)
      {deserialize(readstring(filename.c_str()));}

   //! add operator (string concatenation)
   ministrings &operator += (const ministring &a)
      {
      append(a);
      return(*this);
      }

   //! add operator (string list concatenation)
   ministrings &operator += (const ministrings &a)
      {
      append(a);
      return(*this);
      }

   //! sub operator (string removal)
   ministrings &operator -= (const ministring &a)
      {
      unsigned int i;

      for (i=getsize(); i>0; i--)
         if (get(i-1)==a) remove(i-1);

      return(*this);
      }

   //! sub operator (string list removal)
   ministrings &operator -= (const ministrings &a)
      {
      unsigned int i,j;

      for (j=0; j<a.getsize(); j++)
         for (i=getsize(); i>0; i--)
            if (get(i-1)==a[j]) remove(i-1);

      return(*this);
      }

   };

//! cmp operator (compare string lists)
inline int operator == (const ministrings &a,const ministrings &b)
   {
   unsigned int i;

   if (a.getsize()!=b.getsize()) return(0);

   for (i=0; i<a.getsize(); i++)
      if (a[i]!=b[i]) return(0);

   return(1);
   }

//! less than operator (alpha-numerical comparison of string lists)
inline int operator < (const ministrings &a,const ministrings &b)
   {
   unsigned int i;

   ministrings s1(a),s2(b);
   unsigned int s;

   shellsort<ministring>(s1);
   shellsort<ministring>(s2);

   s=s1.getsize();

   if (s<s2.getsize()) return(1);
   if (s>s2.getsize()) return(0);

   for (i=0; i<s; i++)
      {
      if (s1[i]<s2[i]) return(1);
      if (s2[i]<s1[i]) return(0);
      }

   return(0);
   }

//! add operator (string concatenation)
inline ministrings operator + (const ministrings &a,const ministring &b)
   {
   ministrings strs(a);

   strs.append(b);

   return(strs);
   }

//! add operator (string list concatenation)
inline ministrings operator + (const ministrings &a,const ministrings &b)
   {
   ministrings strs(a);

   strs.append(b);

   return(strs);
   }

//! sub operator (string removal)
inline ministrings operator - (const ministrings &a,const ministring &b)
   {
   unsigned int i;

   ministrings strs(a);

   for (i=strs.getsize(); i>0; i--)
      if (strs[i-1]==b) strs.remove(i-1);

   return(strs);
   }

//! sub operator (string list removal)
inline ministrings operator - (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   ministrings strs(a);

   for (j=0; j<b.getsize(); j++)
      for (i=strs.getsize(); i>0; i--)
         if (strs[i-1]==b[j]) strs.remove(i-1);

   return(strs);
   }

//! div operator (string inclusion)
inline int operator / (const ministring &a,const ministrings &b)
   {
   unsigned int i;

   for (i=0; i<b.getsize(); i++)
      if (a==b[i]) return(1);

   return(0);
   }

//! div operator (string list inclusion)
inline int operator / (const ministrings &a,const ministrings &b)
   {
   unsigned int i,j;

   BOOLINT inclusion;

   for (i=0; i<a.getsize(); i++)
      {
      inclusion=FALSE;

      for (j=0; j<b.getsize(); j++)
         if (a[i]==b[j])
            {
            inclusion=TRUE;
            break;
            }

      if (!inclusion) return(0);
      }

   return(1);
   }

//! stream output
inline std::ostream& operator << (std::ostream &out,const ministrings &a)
   {
   unsigned int i;

   for (i=0; i<a.getsize(); i++)
      {
      out << a[i];
      if (i<a.getsize()-1) out << ";";
      }

   return(out);
   }

#endif
