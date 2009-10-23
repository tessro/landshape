#include "shape.h"

void ls_free_shape_contents(ls_shape_t *shp)
{
  free(shp->parts);
  free(shp->part_types);
  free(shp->points);
}

void ls_dump_shape(const ls_shape_t *shp)
{
  printf("id=%i parts=%i points=%i\n", shp->record_no, shp->num_parts, shp->num_points);
  
  int i;
  printf("points: [\n");
  for (i = 0; i < shp->num_points; i++) {
    printf("  (%f, %f) \n", shp->points[i].x, shp->points[i].y);
  }
  printf("]\n");
}
