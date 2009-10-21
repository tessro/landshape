#ifndef SHAPE_H
#define SHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define SHAPE_TYPE_NULL         0
#define SHAPE_TYPE_POINT        1
#define SHAPE_TYPE_POLYLINE     3
#define SHAPE_TYPE_POLYGON      5
#define SHAPE_TYPE_MULTIPOINT   8
#define SHAPE_TYPE_POINTZ       11
#define SHAPE_TYPE_POLYLINEZ    13
#define SHAPE_TYPE_POLYGONZ     15
#define SHAPE_TYPE_MULTIPOINTZ  18
#define SHAPE_TYPE_POINTM       21
#define SHAPE_TYPE_POLYLINEM    23
#define SHAPE_TYPE_POLYGONM     25
#define SHAPE_TYPE_MULTIPOINTM  28
#define SHAPE_TYPE_MULTIPATCH   31

typedef unsigned char uchar;

/* IMPORTANT: order matches ESRI shapefile spec, so we read them all at once! */
typedef struct box1 {
  double min;
  double max;
} box1_t;

typedef struct box2 {
  double xmin;
  double ymin;

  double xmax;
  double ymax;
} box2_t;

typedef struct box4 {
  double xmin;
  double ymin;

  double xmax;
  double ymax;
  
  double zmin;
  double zmax;
  
  double mmin;
  double mmax;
} box4_t;

typedef struct point2 {
  double x;
  double y;
} point2_t;

typedef struct point4 {
  double x;
  double y;
  double z;
  double m;
} point4_t;

typedef struct shape {
  struct shape *next;
  
  int record_no;
  int type;
  
  point4_t point;
  
  box2_t bounds;
  
  int num_parts;
  int num_points;
  
  int *parts;
  int *part_types;
  point2_t *points;
  
  box1_t *z_bounds;
  double *z_values;
  
  box1_t *m_bounds;
  double *m_values;
} shape_t;

void free_shape_contents(shape_t *shp);

void dump_shape(const shape_t *shp);

#ifdef __cplusplus
}
#endif


#endif