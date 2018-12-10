#include "index.h"
#include <string.h>
#include <stdlib.h>

//! Write_int writes an integer to a file in blorb format 
static void write_int(FILE *f, unsigned int v)
{
  unsigned char v1=v&0xFF,
    v2=(v>>8)&0xFF,
    v3=(v>>16)&0xFF,
    v4=(v>>24)&0xFF;
  
  fwrite(&v4,1,1,f);
  fwrite(&v3,1,1,f);
  fwrite(&v2,1,1,f);
  fwrite(&v1,1,1,f);
}

unsigned int read_int(unsigned char *from)
{
 unsigned int l;
 
 l = ((unsigned int) from[0]) << 24;
 l |= ((unsigned int) from[1]) << 16;
 l |= ((unsigned int) from[2]) << 8;
 l |= ((unsigned int) from[3]);
 return l;
}

unsigned int ReadChunk(FILE *from, void **to)
// ReadChunk reads one entry from a blorb and loads a chunk from it
{
 unsigned int l;
 unsigned char v[4];
 unsigned long pp;
 pp=ftell(from);
 fseek(from,4,SEEK_CUR);
 fread(v,4,1,from);
 l=read_int(v);
 fseek(from,pp,SEEK_SET);
 l+=8;
 if (l%2) l++;
 *to=malloc(l);
 fread(*to,l,1,from);
 return l;
}

int ReadRIdx(FILE *from, RIdx **arr)
{
 void *chnk;
 RIdx *indx;
 int i,n,l;
 fseek(from,12,SEEK_SET);
 i=ReadChunk(from,&chnk);
 if (memcmp(chnk,"RIdx",4)) { printf("Error: RIdx chunk not found!\n"); exit(1); }
 n=read_int(chnk+8);
 indx=(RIdx *)malloc(sizeof(RIdx)*n);
 for(i=0,l=12;i<n;i++,l+=12)
 {
  unsigned char buf[4];
  memcpy(indx[i].use,chnk+l,4);
  memcpy(indx[i].res,chnk+l+4,4);
  memcpy(buf,chnk+l+8,4);
  indx[i].offset=read_int(buf);
  indx[i].f=from;
 }
 *arr=indx;
 free(chnk);
 return n;
}

INdx *buildINdx(FILE *from, RIdx *idx, int n)
{
 char type[4];
 char buf[4];
 int pos, skip, len, i;
 INdx *head=NULL;
 fseek(from,24+(12*n),SEEK_SET);
 while(!feof(from) && !ferror(from))
 {
 skip=0;
 pos=ftell(from);
 if (feof(from)) break;
 for(i=0;i<n;i++) if (pos==idx[i].offset) skip=1;
 fread(type,1,4,from);
 if (feof(from)) break;
 if (skip==0)
  {
   INdx *t=(INdx *) malloc(sizeof(INdx));
   
   t->next=head;
   head=t;
   fseek(from,pos,SEEK_SET);
   t->len=ReadChunk(from,&(t->data));
   t->deleted=0;
   memcpy(t->type,type,4); 
  }
  else
  {
   fread(buf,1,4,from);
   len=read_int(buf);
   if (len %2) len++;
   fseek(from,len,SEEK_CUR);
  }
 }
 rewind(from);
 return head;
}

static void writeResources(RIdx *indx, int n, FILE *out)
{
 int i;
 void *chunk;
 int l;
 fseek(out,24+(12*n),SEEK_SET);
 for(i=0;i<n;i++)
 {
  fseek(indx[i].f,indx[i].offset,SEEK_SET);
  l=ReadChunk(indx[i].f,&chunk);
  indx[i].offset=ftell(out);
  fwrite(chunk,1,l,out);
  free(chunk);
 }
}

static void writeRIdx(FILE *out, RIdx *indx, int n)
{
  int i;
  fseek(out,12,SEEK_SET);
  fwrite("RIdx",1,4,out);
  write_int(out,4+(12*n));
  write_int(out,n);
  for(i=0;i<n;i++)
  {
   fwrite(indx[i].use,1,4,out);
   fwrite(indx[i].res,1,4,out);
   write_int(out,indx[i].offset);
  }
}

void mergeFiles(FILE *f1, FILE *f2, FILE *out)
{
 RIdx *ridx;
 INdx *indx;
 int n;

 n=BuildRIdx(f1,f2,&ridx,&indx);
printf("Writing the resource chunks...\n");
 writeResources(ridx,n,out);
printf("Writing the RIdx chunk...\n");
 writeRIdx(out,ridx,n);
 fseek(out,0,SEEK_END);
printf("Writing the other chunks...\n");
 while(indx)
 {
  INdx *ii=indx;
  fwrite(indx->data,indx->len,1,out);
  free(indx->data);
  indx=indx->next;
  free(ii);
 }
 n=ftell(out);
 fseek(out,0,SEEK_SET);
 fwrite("FORM",1,4,out);
 write_int(out, n-8);
 fwrite("IFRS",1,4,out);
}
