#ifndef MAINFILE_H
#define MAINFILE_H

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

typedef struct ls_shapefileinfo {
  int version;
  int type;
  int byte_length;
  
  box4_t bounds;
} ls_shapefileinfo_t;

typedef struct ls_mainfile {
  ls_shapefileinfo_t info;
  
  ls_shape_t *head;
} ls_mainfile_t;

int ls_read_mainfile(ls_mainfile_t *sf, FILE* f);

void ls_parse_shapefile_header(ls_shapefileinfo_t *s, const uchar *buf);
void ls_dump_shapefile_header(const ls_shapefileinfo_t *s);

void ls_parse_shape(int type, ls_shape_t *shp, const uchar *buf, const uint size);

void ls_free_mainfile_contents(ls_mainfile_t *sf);

#ifdef __cplusplus
}
#endif

#endif
