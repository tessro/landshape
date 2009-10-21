#include "shapefile.h"

void dump_file_header(const shapefileinfo_t *s)
{
  printf("version=%i type=%i size=%i\n", s->version, s->type, s->byte_length);
  printf("Shape bounds: x in [%f, %f]; y in [%f, %f]\n", s->bounds.xmin, s->bounds.xmax, s->bounds.ymin, s->bounds.ymax);
}

void parse_file_header(shapefileinfo_t *s, const uchar *buf)
{
  s->version      = *((int*)buf + 7);
  s->type         = *((int*)buf + 8);
  s->byte_length  = 2*SWAP_ENDIAN_32(*(uint*)(buf + 24)); /* arrives in words */
  
  /* read the bounding box */
  memcpy(&s->bounds, buf+36, 8 * sizeof(double));
  
  assert("Bounding box mismatch" && s->bounds.xmin < s->bounds.xmax && s->bounds.ymin < s->bounds.ymax);
}

void parse_polygon(polygon_t *shp, const uchar *buf, const uint size)
{
  shp->type = SHAPE_TYPE_POLYGON;
  
  /* read the bounding box, num_parts and num_points */
  memcpy(&shp->bounds, buf+4, 4*sizeof(double) + 2*sizeof(int));
  
  shp->parts = malloc(shp->num_parts * sizeof(int));
  shp->points = malloc(shp->num_points * sizeof(point_t));
  
  memcpy(shp->parts, buf+44, shp->num_parts * sizeof(int));
  memcpy(shp->points, buf+44 + shp->num_parts * sizeof(int), shp->num_points * sizeof(point_t));
}
