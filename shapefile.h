#ifndef SHAPEFILE_H
#define SHAPEFILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "shape.h"

#define SHAPEFILE_FILE_HEADER_LENGTH 100
#define SHAPEFILE_RECORD_HEADER_LENGTH 8

#define SWAP_ENDIAN_32(x)       \
  (unsigned long)               \
  (((x) >> 24) |                \
   (((x) >> 8) & 0x0000FF00) |  \
   (((x) << 8) & 0x00FF0000) |  \
   ((x) << 24))

typedef struct shapefileinfo {
  int version;
  int type;
  int byte_length;
  
  box4_t bounds;
} shapefileinfo_t;

typedef struct shapefile {
  shapefileinfo_t info;
  
  shape_t *head;
} shapefile_t;

int ls_read_shapefile(shapefile_t *sf, FILE* f);

void ls_parse_file_header(shapefileinfo_t *s, const uchar *buf);
void ls_dump_file_header(const shapefileinfo_t *s);

void ls_parse_shape(int type, shape_t *shp, const uchar *buf, const uint size);

void ls_free_shapefile_contents(shapefile_t *sf);

#ifdef __cplusplus
}
#endif

#endif
