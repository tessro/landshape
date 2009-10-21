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

typedef struct box2 {
  double xmin;
  double ymin;

  double xmax;
  double ymax;
} box2_t;

typedef struct point {
  double x;
  double y;
} point_t;

typedef struct polygon {
  struct polygon *next;
  
  int record_no;
  int type;
  
  box2_t bounds;
  
  int num_parts;
  int num_points;
  
  int *parts;
  point_t *points;
} polygon_t;

void free_polygon(polygon_t *shp);

void dump_polygon(const polygon_t *shp);

#ifdef __cplusplus
}
#endif


#endif