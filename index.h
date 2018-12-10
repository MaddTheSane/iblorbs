#include <stdio.h>

//! Write_int writes an integer to a file in blorb format 
extern void write_int(FILE *f, unsigned int v);
extern unsigned int read_int(unsigned char *from);
//! ReadChunk reads one entry from a blorb and loads a chunk from it
extern unsigned int ReadChunk(FILE *from, void **to);

typedef struct {
  char use[4];
  char res[4];
  unsigned int offset;
  FILE *f;
} RIdx;
typedef struct idx_t
 {
  char type[4];
  int len;
  void *data;
  char deleted;
  struct idx_t *next;
 } INdx;

extern int ReadRIdx(FILE *from, RIdx **arr);
extern INdx *buildINdx(FILE *from, RIdx *idx, int n);
extern void mergeFiles(FILE *f1, FILE *f2, FILE *out);

//Declared in the main file
extern int BuildRIdx(FILE *f1, FILE *f2, RIdx **out, INdx **idx);
