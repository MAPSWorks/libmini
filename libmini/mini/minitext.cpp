// (c) by Stefan Roettger

#include "minibase.h"

#include "minirgb.h"
#include "miniOGL.h"

#include "minitext.h"

namespace minitext {

float CONFIGURE_ZSCALE=0.95f; // must be 1.0f for orthographic projections

void drawsymbol(float hue,float sat,float val,float alpha,const char *symbol)
   {
   float rgb[3];

   float px,py,lx,ly;

   BOOLINT draw=TRUE;

   hsv2rgb(hue,sat,val,rgb);

   color(rgb[0],rgb[1],rgb[2],alpha);

   px=py=0.0f;

   while (*symbol!='\0')
      {
      lx=px;
      ly=py;

      switch (*symbol++)
         {
         case 'u': draw=FALSE; break;
         case 'd': draw=TRUE; break;
         case 'n': py+=1.0f; break;
         case 's': py-=1.0f; break;
         case 'e': px+=1.0f; break;
         case 'w': px-=1.0f; break;
         case 'N': py+=1.0f; px+=1.0f; break;
         case 'S': py-=1.0f; px-=1.0f; break;
         case 'E': px+=1.0f; py-=1.0f; break;
         case 'W': px-=1.0f; py+=1.0f; break;
         }

      if (draw)
         if (px!=lx || py!=ly) renderline(lx,ly,0.0f,px,py,0.0f);
      }
   }

void drawletter(float hue,float sat,float val,float alpha,char letter)
   {
   mtxpush();
   mtxscale(1.0f/4,1.0f/6,0.0f);

   switch (toupper(letter))
      {
      // letters occupy a 7x4 grid
      case 'A': drawsymbol(hue,sat,val,alpha,"nnnnnNeEsssssunnndwww"); break;
      case 'B': drawsymbol(hue,sat,val,alpha,"nnnnnneeEsSwwueedEsSww"); break;
      case 'C': drawsymbol(hue,sat,val,alpha,"ueeNdSwWnnnnNeE"); break;
      case 'D': drawsymbol(hue,sat,val,alpha,"nnnnnneeEssssSww"); break;
      case 'E': drawsymbol(hue,sat,val,alpha,"ueeedwwwnnnnnneeeussswdww"); break;
      case 'F': drawsymbol(hue,sat,val,alpha,"nnnnnneeeussswdww"); break;
      case 'G': drawsymbol(hue,sat,val,alpha,"unnneedessSwWnnnnNeE"); break;
      case 'H': drawsymbol(hue,sat,val,alpha,"nnnnnnueeedssssssunnndwww"); break;
      case 'I': drawsymbol(hue,sat,val,alpha,"uedeeuwdnnnnnneuwdw"); break;
      case 'J': drawsymbol(hue,sat,val,alpha,"undEeNnnnnnwww"); break;
      case 'K': drawsymbol(hue,sat,val,alpha,"nnnnnnusssdNNNuSSSdEEE"); break;
      case 'L': drawsymbol(hue,sat,val,alpha,"ueeedwwwnnnnnn"); break;
      case 'M': drawsymbol(hue,sat,val,alpha,"nnnnnnEeNssssss"); break;
      case 'N': drawsymbol(hue,sat,val,alpha,"nnnnnnusdEEEssunndnnnn"); break;
      case 'O': drawsymbol(hue,sat,val,alpha,"uedWnnnnNeEssssSw"); break;
      case 'P': drawsymbol(hue,sat,val,alpha,"nnnnnneeEsSww"); break;
      case 'Q': drawsymbol(hue,sat,val,alpha,"uedWnnnnNeEssssSwuNdE"); break;
      case 'R': drawsymbol(hue,sat,val,alpha,"nnnnnneeEsSwwuedEEs"); break;
      case 'S': drawsymbol(hue,sat,val,alpha,"undEeNnWwWnNeE"); break;
      case 'T': drawsymbol(hue,sat,val,alpha,"ueednnnnnnwwueedee"); break;
      case 'U': drawsymbol(hue,sat,val,alpha,"unnnnnndsssssEeNnnnnn"); break;
      case 'V': drawsymbol(hue,sat,val,alpha,"unnnnnndsssssENNnnnn"); break;
      case 'W': drawsymbol(hue,sat,val,alpha,"unnnnnndssssssNeEnnnnnn"); break;
      case 'X': drawsymbol(hue,sat,val,alpha,"nnNNNnuwwwdsEEEss"); break;
      case 'Y': drawsymbol(hue,sat,val,alpha,"ueednnWWnnueeedsssS"); break;
      case 'Z': drawsymbol(hue,sat,val,alpha,"ueeendswwwnnNNNnwwws"); break;
      case '_': drawsymbol(hue,sat,val,alpha,"eee"); break;
      case '0': drawsymbol(hue,sat,val,alpha,"uedWnnnnNeEssssSwuNNdWWW"); break;
      case '1': drawsymbol(hue,sat,val,alpha,"ueednnnnnnSS"); break;
      case '2': drawsymbol(hue,sat,val,alpha,"ueeedwwwNNNnnWwSs"); break;
      case '3': drawsymbol(hue,sat,val,alpha,"undEeNnWwuedNnWwS"); break;
      case '4': drawsymbol(hue,sat,val,alpha,"ueednnnnnnuwwdssseee"); break;
      case '5': drawsymbol(hue,sat,val,alpha,"undEeNnWwwnnneee"); break;
      case '6': drawsymbol(hue,sat,val,alpha,"unndNeEsSwWnnnnNeE"); break;
      case '7': drawsymbol(hue,sat,val,alpha,"ueednnnnnnwwusssedee"); break;
      case '8': drawsymbol(hue,sat,val,alpha,"uedWnNWnNeEsSwuedEsSw"); break;
      case '9': drawsymbol(hue,sat,val,alpha,"undEeNnnnnWwSsEee"); break;
      case '.': drawsymbol(hue,sat,val,alpha,"uednesw"); break;
      case ',': drawsymbol(hue,sat,val,alpha,"uedN"); break;
      case ':': drawsymbol(hue,sat,val,alpha,"unedneswunndnesw"); break;
      case ';': drawsymbol(hue,sat,val,alpha,"uneedwnesSunnndnesw"); break;
      case '?': drawsymbol(hue,sat,val,alpha,"ueednnnNnWwS"); break;
      case '!': drawsymbol(hue,sat,val,alpha,"ueednundnnnn"); break;
      case '|': drawsymbol(hue,sat,val,alpha,"ueednnnnnn"); break;
      case '-': drawsymbol(hue,sat,val,alpha,"unnnedee"); break;
      case '+': drawsymbol(hue,sat,val,alpha,"unnnedeeunwdss"); break;
      case '*': drawsymbol(hue,sat,val,alpha,"unnedNNuwwdEE"); break;
      case '/': drawsymbol(hue,sat,val,alpha,"undNNN"); break;
      case '>': drawsymbol(hue,sat,val,alpha,"unedNNWW"); break;
      case '<': drawsymbol(hue,sat,val,alpha,"uneeedWWNN"); break;
      case '=': drawsymbol(hue,sat,val,alpha,"unnedeeunndww"); break;
      case '(': drawsymbol(hue,sat,val,alpha,"ueedWnnnnN"); break;
      case ')': drawsymbol(hue,sat,val,alpha,"uedNnnnnW"); break;
      case '[': drawsymbol(hue,sat,val,alpha,"ueedwnnnnnne"); break;
      case ']': drawsymbol(hue,sat,val,alpha,"uedennnnnnw"); break;
      case '{': drawsymbol(hue,sat,val,alpha,"ueedwnnWNnne"); break;
      case '}': drawsymbol(hue,sat,val,alpha,"udennNWnnw"); break;
      case '$': drawsymbol(hue,sat,val,alpha,"unndEeNWwWNeEuWndssssssuwdnnnnnn"); break;
      case '%': drawsymbol(hue,sat,val,alpha,"undNNNusssdswneuwwnnndnwse"); break;
      case '#': drawsymbol(hue,sat,val,alpha,"unndeeeunndwwwuNdssssuednnnn"); break;
      case '&': drawsymbol(hue,sat,val,alpha,"ueeedWWWnnNEssSSseNN"); break;
      case '@': drawsymbol(hue,sat,val,alpha,"ueeendSwWnnnnNeEssswwnnee"); break;
      case '^': drawsymbol(hue,sat,val,alpha,"uennnndNE"); break;
      case '~': drawsymbol(hue,sat,val,alpha,"unnnndNEN"); break;
      case '"': drawsymbol(hue,sat,val,alpha,"uennnndnueds"); break;
      case '\'': drawsymbol(hue,sat,val,alpha,"uennnndn"); break;
      }

   mtxpop();
   }

void drawline(float x1,float y1,float x2,float y2,
              float hue,float sat,float val,float alpha)
   {
   float rgb[3];

   hsv2rgb(hue,sat,val,rgb);

   color(rgb[0],rgb[1],rgb[2],alpha);
   renderline(x1,y1,0.0f,x2,y2,0.0f);
   }

void drawlineRGBA(float x1,float y1,float x2,float y2,
                  float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);
   renderline(x1,y1,0.0f,x2,y2,0.0f);
   }

void drawquad(float x,float y,float width,float height,
              float hue,float sat,float val,float alpha)
   {
   float rgb[3];

   hsv2rgb(hue,sat,val,rgb);

   color(rgb[0],rgb[1],rgb[2],alpha);

   beginfans();
   beginfan();
   fanvertex(x,y,0.0f);
   fanvertex(x+width,y,0.0f);
   fanvertex(x+width,y+height,0.0f);
   fanvertex(x,y+height,0.0f);
   endfans();
   }

void drawquadRGBA(float x,float y,float width,float height,
                  float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);

   beginfans();
   beginfan();
   fanvertex(x,y,0.0f);
   fanvertex(x+width,y,0.0f);
   fanvertex(x+width,y+height,0.0f);
   fanvertex(x,y+height,0.0f);
   endfans();
   }

void drawframe(float x,float y,float width,float height,
               float hue,float sat,float val,float alpha)
   {
   float rgb1[3],rgb2[3];

   hsv2rgb(hue,sat,fmax(val-0.25f,0.0f),rgb1);
   hsv2rgb(hue,sat,fmin(val+0.25f,1.0f),rgb2);

   color(rgb1[0],rgb1[1],rgb1[2],alpha);
   renderline(x,y,0.0f,x+width,y,0.0f);
   renderline(x+width,y,0.0f,x+width,y+height,0.0f);

   color(rgb2[0],rgb2[1],rgb2[2],alpha);
   renderline(x+width,y+height,0.0f,x,y+height,0.0f);
   renderline(x,y+height,0.0f,x,y,0.0f);
   }

void drawframeRGBA(float x,float y,float width,float height,
                   float r,float g,float b,float alpha)
   {
   color(r,g,b,alpha);
   renderline(x,y,0.0f,x+width,y,0.0f);
   renderline(x+width,y,0.0f,x+width,y+height,0.0f);
   renderline(x+width,y+height,0.0f,x,y+height,0.0f);
   renderline(x,y+height,0.0f,x,y,0.0f);
   }

void drawstring(float width,
                float hue,float sat,float val,float alpha,const char *str,
                float backval,float backalpha)
   {
   const float linefeed=0.2f;

   int c,cmax,l;
   float scale;
   const char *ptr;

   if (str==NULL) return;

   for (c=0,cmax=l=1,ptr=str; *ptr!='\0'; ptr++)
      {
      if (*ptr!='\n') c++;
      else {c=0; l++;}
      if (c>cmax) cmax=c;
      }

   scale=width/cmax;

   if (backalpha>0.0f && CONFIGURE_ZSCALE<1.0f)
      {
      drawquad(0.0f,0.0f,scale*cmax,scale*(l+(l-1)*linefeed),0.0f,0.0f,backval,backalpha);

      mtxproj();
      mtxpush();
      mtxscale(CONFIGURE_ZSCALE,CONFIGURE_ZSCALE,CONFIGURE_ZSCALE); // prevent Z-fighting
      mtxmodel();

      disableZwriting();
      }

   mtxpush();
   mtxscale(scale,scale,0.0f);
   mtxtranslate(0.0f,(l-1)*(1.0f+linefeed),0.0f);
   mtxpush();

   while (*str!='\0')
      {
      if (*str=='\n')
         {
         mtxpop();
         mtxtranslate(0.0f,-(1.0f+linefeed),0.0f);
         mtxpush();
         }
      else
         {
         if (*str>='a' && *str<='z')
            {
            mtxpush();
            mtxtranslate(0.2f,0.0f,0.0f);
            mtxscale(0.6f,0.75f,1.0f);

            drawletter(hue,sat,val,alpha,*str);

            mtxpop();
            }
         else drawletter(hue,sat,val,alpha,*str);

         mtxtranslate(1.0f,0.0f,0.0f);
         }

      str++;
      }

   mtxpop();
   mtxpop();

   if (backalpha>0.0f && CONFIGURE_ZSCALE<1.0f)
      {
      mtxproj();
      mtxpop();
      mtxmodel();

      enableZwriting();
      }
   }

// configuring
void configure_zfight(float zscale) {CONFIGURE_ZSCALE=zscale;}

}
