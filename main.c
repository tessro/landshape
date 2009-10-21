#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "shapefile.h"

#define BUF_SIZE 100

int main(int argc, char** argv)
{
  assert(argc > 0);
  
  if (argc == 1) {
    printf("usage: %s <shapefile>\n", argv[0]);
    exit(1);
  }
  
  FILE *f;
  
  if (NULL == (f = fopen(argv[1], "r"))) {
    perror("Failed opening shapefile");
    return -1;
  }
  
  shapefile_t sf;
  bzero(&sf, sizeof(shapefile_t));
  
  if (!read_shapefile(&sf, f)) {
    printf("Couldn't read shapefile\n");
    exit (1);
  }
  
  fclose(f);
  
  
  
  free_shapefile_contents(&sf);
  
  return 0;
}
