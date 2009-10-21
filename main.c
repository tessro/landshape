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
    exit(1);
  }
  
  uchar buf[BUF_SIZE+1];
  
  if (SHAPEFILE_FILE_HEADER_LENGTH != fread(buf, sizeof(char), SHAPEFILE_FILE_HEADER_LENGTH, f)) {
    printf("Invalid header\n");
    exit(1);
  }

  shapefileinfo_t s;
  
  parse_file_header(&s, buf);
  
  int record_no, record_length;
  uchar *shp_buf;
  polygon_t *poly, *poly_head;
  
  poly_head = poly = malloc(sizeof(polygon_t));
  bzero(poly, sizeof(polygon_t));
  
  while (fread(buf, sizeof(uchar), SHAPEFILE_RECORD_HEADER_LENGTH, f)) {
    record_no     = SWAP_ENDIAN_32(*(uint*)buf);
    record_length = 2*SWAP_ENDIAN_32(*(uint*)(buf + 4)); /* arrives in words */
    
    shp_buf = malloc(record_length);
    
    if (record_length != fread(shp_buf, sizeof(uchar), record_length, f)) {
      printf("Invalid shape");
      free(shp_buf);
      free_polygon(poly_head);
      exit(1);
    }
    
    poly->next = malloc(sizeof(polygon_t));
    poly = poly->next;
    bzero(poly, sizeof(polygon_t));
    
    poly->record_no = record_no;
    parse_polygon(poly, shp_buf, record_length);
    
    free(shp_buf); 
  }
  
  fclose(f);
  
  /* cut off the temporary head */
  poly = poly_head;
  poly_head = poly_head->next;
  free_polygon(poly);
  
  /* free the list */
  while (poly = poly_head) {
    poly_head = poly_head->next;
    free_polygon(poly);
  }
  
  return 0;
}
