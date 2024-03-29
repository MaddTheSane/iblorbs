/* BLC: The Blorb Packager
 * V .4b by L. Ross Raszewski
 * Copyright 2000 by L. Ross Raszewski, but freely distributable.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "short.h"

#define BUF_SIZE 4096
static void *my_realloc(void *buf, size_t size)
{
 buf=realloc(buf,size);
 if (!buf)
  {
        fprintf(stderr,"Error: Memory exceeded!\n");
        exit(2);
   }
   return buf;
}

static void *my_malloc(int size)
{
 void *buf=malloc(size);
 if (!buf)
  {
        fprintf(stderr,"Error: Memory exceeded!\n");
        exit(2);
   }
   return buf;
}
/* The version and boilerplate messages.  The string is formatted
 * with the assumption  that it will be printed with the program name,
 * compile date, and version string
 */
#define VERSION ".4b"
#define BOILER "%s: Blorb Packager (%s)\nVersion %s by L. Ross Raszewski\n"
// Maximum number of chunks we'll allow
#define MAX_BLORB 1024
// The program name.  DJGPP munges this, so we rebuild it here
static char *MyName;
// The current line in the blc control file
static int Line=0;

/* The blorb chunk types. In addition to the chunk data, it holds the index
 * information if needed
 */
struct Chunk {
        char Type[5];
        char Use[5];
        int Res;
        unsigned long Length;
        char *Data;
        };

//! Write_int writes an integer to a file in blorb format 
static void write_int(FILE *f, unsigned int v)
{
  unsigned char vv[4] = {(v>>24)&0xFF, (v>>16)&0xFF, (v>>8)&0xFF, v&0xFF};
  
  fwrite(vv,1,4,f);
}

/*! str_long writes a long to a string, in a format suitable to later
 * using with write_id
 */
static void str_long(char *f, unsigned int v)
{
  unsigned char v1=v&0xFF,
    v2=(v>>8)&0xFF,
    v3=(v>>16)&0xFF,
    v4=(v>>24)&0xFF;
  
  f[0]=v4;
  f[1]=v3;
  f[2]=v2;
  f[3]=v1;
}

/*! str_long writes a long to a string, in a format suitable to later
 * using with write_id
 */
static void str_short(char *f, unsigned int v)
{
  unsigned char v1=v&0xFF,
    v2=(v>>8)&0xFF;
  
  f[0]=v2;
  f[1]=v1;
}

/*! write_id writes a string to a file as a blorb ID string (4 bytes, space
 * padded)
 */
static void write_id(FILE *f, unsigned char *s)
{
  int i;
  unsigned char sp=' ';
  
  for (i=0; i<strlen(s); i++)
    fwrite(&s[i],1,1,f);
  for (;i<4;i++)
    fwrite(&sp, 1,1,f);
}

//! str_id writes a blorb identifier to a string
static void str_id(char *f, unsigned char *s)
{
  int i;
  unsigned char sp=' ';
  
  for (i=0; i<strlen(s); i++)
   f[i]=s[i];
  for (;i<4;i++)
   f[i]=sp;
}



//! ReadChunk reads one entry from a blc control file and loads a chunk from it
static struct Chunk *ReadChunk(FILE *f)
{
 // Malloc ourselves a new chunk
 struct Chunk *N=(struct Chunk *)my_malloc(sizeof(struct Chunk));
 char *Buffer=NULL; int buflen=0; int current=0; int c;
 // Read in the BLC line
 fscanf(f,"%s %d %s ",N->Use,&(N->Res),N->Type);
 // Abort if anything went wrong
 if (feof(f)){ free (N); return NULL; }
 // Read in the rest of the line to the buffer
 while(1){
  c=fgetc(f);
  if (c==EOF || c=='\n') break;
  if (current==buflen)
  {
   buflen=(buflen+1)*2;
   Buffer=(char *)my_realloc(Buffer,buflen);
  }
  Buffer[current++]=c;
  };
 // If it's not a resource, the data is the line data.
 if (strcmp(N->Use,"0")==0) { N->Length=current; N->Data=Buffer; }
 else if (strcmp(N->Use,"1")==0) { Buffer[current]=0;
                                   N->Data=(char *)my_malloc(2);
                                   str_short(N->Data,atoi(Buffer));
                                   free(Buffer);
                                   N->Length=2; N->Use[0]='0';}
 else
 {
  // Otherwise, we load the file from disk
  FILE *in; char *buf; int bsize=BUF_SIZE;
  Buffer[current]=0;
  if (N->Use[0]=='2') N->Use[0]='0';
  in=fopen(Buffer,"rb");
  if (!in) { printf("%s: Error: %d: Can't open file %s\n",MyName,Line,Buffer);
             free(Buffer);
             free(N);
             return NULL;
            }
   free(Buffer);
   fseek(in,0,SEEK_END);
   N->Length=ftell(in);
   Buffer=(char *)my_malloc(N->Length);
   fseek(in,0,SEEK_SET);
   if ( (fread(Buffer,1,N->Length,in))!=N->Length)
    fprintf(stderr,"Couldn't read the file. What's up with that?\n");


/*
 Older, slower version.
   Buffer=(char *)my_malloc(BUF_SIZE);

   buf=Buffer;
   // Load in the file
   while(fread(buf,1,BUF_SIZE,in)>0)
   { bsize+=BUF_SIZE;
     Buffer=(char *)my_realloc(Buffer,bsize);
     buf=Buffer+bsize-BUF_SIZE;
   }
  // And calculate the chunk size
   fseek(in,0,SEEK_END);
   N->Length=ftell(in);


*/
   N->Data=Buffer;
   fclose(in);
   }
  return N;
}

//! Array of all chunks
static struct Chunk *Blorb[MAX_BLORB];
//! Number of chunks in this file
static int Chunks=1;
//! Number of chunks we need to index
static int IndexEntries=0;
//! Offsets of index entries
static unsigned long int *Index;

//! BuildIndex builds the index chunk for a blorb file, loading all other chunks
static void BuildIndex(FILE *f)
{
 int i,n=0; char *dp;
 Blorb[0]=(struct Chunk *)my_malloc(sizeof(struct Chunk));
 // Write the chunk type
 strcpy(Blorb[0]->Type,"RIdx");
 strcpy(Blorb[0]->Use,"0"); 
 // Load all the chunks
 while(!feof(f))
 { Line++;
   Blorb[Chunks]=ReadChunk(f);
   if(Blorb[Chunks]) {
    // Find out how many resources there are
    if (strcmp(Blorb[Chunks]->Use,"0")) n++;
    Chunks++;
    }
 }

 // Write the length of the resource index chunk, and allocate its data space
 Blorb[0]->Length=(12*n)+4;
 Blorb[0]->Data=(char *)my_malloc(Blorb[0]->Length);
 Index=(unsigned long *)my_malloc(n*sizeof(unsigned long));
 // The first thing in the data chunk is the number of entries
 str_long(Blorb[0]->Data,n);
 // Now, scroll through the chunks, noting each one in the index chunk
 dp=Blorb[0]->Data+4;
 for(i=1;i<Chunks;i++)
  if (strcmp(Blorb[i]->Use,"0"))
  {
   str_id(dp,Blorb[i]->Use);
   dp+=4;
   str_long(dp,Blorb[i]->Res);
   dp+=4;
   Index[IndexEntries++]=(dp-(Blorb[0]->Data))+20;
   dp+=4;
  }
}

//! Write_Chunk writes one chunk to a file
static void Write_Chunk(FILE *Out,struct Chunk *C)
{
int z=0;
/* AIFF files are themselves chunks, so we just write their data, not
 * their other info.
 */
if (strcmp(C->Type,"FORM")){
 write_id(Out,C->Type);
 write_int(Out,C->Length);
 }
 fwrite(C->Data,1,C->Length,Out);
 // Pad chunks of odd length
 if (C->Length%2) fwrite(&z,1,1,Out);
}

//! WriteBlorb generates a blorb from a BLC file
static void WriteBlorb(FILE *F,FILE *Out)
{
 int n=0,i;
 // Write the IFF header
 write_id(Out,"FORM");
 write_id(Out,"latr");  // We'll find this out at the end
 write_id(Out,"IFRS");
 // Build the index chunk
 BuildIndex(F);
 for(i=0;i<Chunks;i++)
 {
  // Write the index entries into the index chunk
  if (strcmp(Blorb[i]->Use,"0"))
  {
   long t=ftell(Out);
   fseek(Out,Index[n++],SEEK_SET);
   write_int(Out,t);
   fseek(Out,t,SEEK_SET);
  }
  // Write the chunk to the file
  Write_Chunk(Out,Blorb[i]);
  // Delete the chunk
  free(Blorb[i]->Data);
  free(Blorb[i]);
 }
 // We don't need the index offsets any more either.
 free(Index);
 // Size of the data section of the blorb file
 n=ftell(Out)-8;
 fseek(Out,4,SEEK_SET);
 // Write that to the file
 write_int(Out,n);
}
int main(int argc, char **argv)
{
 FILE *in; 
 FILE *out;
 char *OutName;
 MyName=ShortProgramName(argv[0]);
 printf(BOILER,MyName,__DATE__,VERSION);
 // Print error usage
 if (argc<2)
 { printf("%s: Error: usage: Usage is one of:\n\
 %s in-file out-file\n\
 %s out-file\n",MyName,MyName,MyName);
   exit(1);
 }
 // 1 argument: use standard input
 if (argc==2)
 { in=stdin;
   OutName=argv[1];
 }
 else // two arguments: open a blc file.
 {
 in=fopen(argv[1],"r");
 OutName=argv[2];
 if (!in)
 {
  printf("%s: Error: (before compilation): Can't open Blorb Control File %s\n",
  MyName,argv[1]);
  exit(1);
 }
 }
 // Open output
 out=fopen(OutName,"wb");
 if (!out)
 {
  printf("%s: Error: (before compilation): Can't open Blorb Output File %s\n",
  MyName,OutName);
  exit(1);
 }
 // Generate the blorb
 WriteBlorb(in,out);
 // Close the files, delete MyName
 fclose(in);
 fclose(out);
 free(MyName);
 return 0;
}
