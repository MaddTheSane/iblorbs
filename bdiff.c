/* Blorb Merge
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "short.h"
#include "index.h"

static char Merge_Mode=0;
/* The version and boilerplate messages.  The string is formatted
 * with the assumption  that it will be printed with the program name,
 * compile date, and version string
 */
#define VERSION ".1b"
#define BOILER "%s: Blorb Diff (%s)\nVersion %s by L. Ross Raszewski\n"
// Maximum number of chunks we'll allow
// The program name.  DJGPP munges this, so we rebuild it here
static char *MyName;

static int cmpchunk(FILE *f1, int o1, FILE *f2, int o2)
{
  void *a;
  void *b;
  int as, bs, c;
  fseek(f1,o1,SEEK_SET);
  as=ReadChunk(f1, &a);
  fseek(f2,o2,SEEK_SET);
  bs=ReadChunk(f2, &b);
  c=(as-bs);
  if (c==0)
   c=memcmp(a,b,as);
//  else printf("Chunk sizes differ %d vs %d.\n",as,bs);
  free(a);
  free(b);
//  if (c) printf("Chunk contents differ.\n");
  return c;
}
/*
  1. Read both RIdx chunks
  2. Compose new RIdx
  3. For each chunk in the new RIdx, write it to the chunk file.
  4. Scan both files to build 'other chunks' index
  5. For each other chunk, if it is in the other chunks index, add it to
     the chunk file.
  6. Write the IFRS and RIdx to the outfile
  7. Copy the chunk file into the outfile
*/


static INdx *diffINdx(INdx *i1, INdx *i2)
{
 INdx *p=i1;
 INdx *pp;
 INdx *out=NULL;
 /*
  foreach chunk in i2
   if chunk notin i1 or chunks not identical
    add chunk to output index
 */
 for(p=i2;p;p=p->next)
 {
  for(pp=i1;pp;pp=pp->next)
   if (pp->len == p->len &&
       memcmp(pp->type,p->type,4)==0 &&
       memcmp(pp->data,p->data,p->len)==0)
       break;
   if (!pp)
   {
        printf("Chunk %c%c%c%c added or changed.\n",
           p->type[0],
           p->type[1],
           p->type[2],
           p->type[3]);
    pp=(INdx *) malloc(sizeof(INdx));
    memcpy(pp->type,p->type,4);
    pp->len=p->len;
    pp->data=malloc(p->len);
    memcpy(pp->data,p->data,p->len);
    pp->next=out;
    out=pp;
   }
  }
return out;
}


int BuildRIdx(FILE *f1, FILE *f2, RIdx **out, INdx **idx)
{
 RIdx *i1;
 RIdx *i2;
 RIdx *indx;

 int l1, l2, l, i,j;
 INdx *a,*b;
 int ct=0;
 printf("Reading the first RIdx chunk...\n");
 l1=ReadRIdx(f1,&i1);
 printf("Reading the second RIdx chunk...\n");
 l2=ReadRIdx(f2,&i2);
 *out=(RIdx *)malloc(l2*sizeof(RIdx));
 indx=*out;
 printf("Indexing other chunks in the first file...\n");
 a=buildINdx(f1,i1,l1);
 printf("Indexing other chunks in the second file...\n");
 b=buildINdx(f2,i2,l2);
 printf("Comparing other chunks...\n");
 *idx=diffINdx(a,b);
 printf("Comparing resources...\n");
 for(i=0;i<l2;i++)
 {
  for(j=0;j<l1;j++)
   if (memcmp(i2[i].use,i1[j].use,4)==0 &&
       memcmp(i2[i].res,i1[j].res,4)==0 &&
       cmpchunk(f1,i1[j].offset,f2,i2[i].offset)==0)
         break;
   if (j>=l1)
   {
    printf("Resource added or changed. Type: %c%c%c%c. Number: %d.\n",
           i2[i].use[0],
           i2[i].use[1],
           i2[i].use[2],
           i2[i].use[3],
           read_int(i2[i].res));
     indx[ct++]=i2[i];
   }
  }
  free(i1);
  free(i2);
  while(a) { INdx *c=a->next;
             free(a->data);
             free(a);
             a=c;
            }
  while(b) { INdx *c=b->next;
             free(b->data);
             free(b);
             b=c;
            }
  return ct;
}

int main(int argc, char **argv)
{
 FILE *f1, *f2, *f3;
  MyName=ShortProgramName(argv[0]);
 printf(BOILER,MyName,__DATE__,VERSION);
 if (argc < 4) { printf("Usage is %s oldfile newfile outfile\n",MyName); exit(1); }
 f1=fopen(argv[1],"rb");
 f2=fopen(argv[2],"rb");
 f3=fopen(argv[3],"wb");
 mergeFiles(f1,f2,f3);
 return 0;
}





