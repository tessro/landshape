#include "shape.h"

void free_shape_contents(shape_t *shp)
{
  free(shp->parts);
  free(shp->part_types);
  free(shp->points);
}

void dump_shape(const shape_t *shp)
{
  printf("id=%i parts=%i points=%i\n", shp->record_no, shp->num_parts, shp->num_points);
  
  int i;
  printf("points: [\n");
  for (i = 0; i < shp->num_points; i++) {
    printf("  (%f, %f) \n", shp->points[i].x, shp->points[i].y);
  }
  printf("]\n");
}
