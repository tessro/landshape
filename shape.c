#include "shape.h"

void free_polygon(polygon_t *shp)
{
  free(shp->parts);
  free(shp->points);
  free(shp);
}

void dump_polygon(const polygon_t *shp)
{
  printf("id=%i parts=%i points=%i\n", shp->record_no, shp->num_parts, shp->num_points);
  
  int i;
  printf("points: [\n");
  for (i = 0; i < shp->num_points; i++) {
    printf("  (%f, %f) \n", shp->points[i].x, shp->points[i].y);
  }
  printf("]\n");
}
