#include "shapefile.h"

void ls_dump_shapefile_header(const ls_shapefileinfo_t *s)
{
  printf("version=%i type=%i size=%i\n", s->version, s->type, s->byte_length);
  printf("Shape bounds: x in [%f, %f]; y in [%f, %f]\n", s->bounds.xmin, s->bounds.xmax, s->bounds.ymin, s->bounds.ymax);
}

void ls_parse_shapefile_header(ls_shapefileinfo_t *s, const uchar *buf)
{
  s->version      = *((int*)buf + 7);
  s->type         = *((int*)buf + 8);
  s->byte_length  = 2*SWAP_ENDIAN_32(*(uint*)(buf + 24)); /* arrives in words */
  
  /* read the bounding box */
  memcpy(&s->bounds, buf+36, 8 * sizeof(double));
  
  assert("Bounding box mismatch" && s->bounds.xmin < s->bounds.xmax && s->bounds.ymin < s->bounds.ymax);
}

