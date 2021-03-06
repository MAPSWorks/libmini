// (c) by Stefan Roettger, licensed under LGPL 2.1

#include <mini/minibase.h>

#include <mini/rekbase.h>

int main(int argc,char *argv[])
   {
   if (argc!=3)
      {
      printf("usage: %s <input.rek> <output.raw>\n",argv[0]);
      printf(" input: 8bit or 16bit Fraunhofer volume file format (with 2048 byte header)\n");
      printf(" output: 8bit or 16bit LSB raw volume\n");
      exit(1);
      }

   char *output;

   output=copyREKvolume(argv[1],argv[2]);

   if (output)
      {
      printf("wrote %s\n",output);
      free(output);
      }
   else exit(1);

   return(0);
   }
