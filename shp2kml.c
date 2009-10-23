#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "mainfile.h"

#define BUF_SIZE 100

void write_kml(ls_mainfile_t *sf, FILE *out);

int main(int argc, char** argv)
{
  assert(argc > 0);
  
  if (argc != 3) {
    printf("usage: %s <shapefile> <kmlfile>\n", argv[0]);
    exit(1);
  }
  
  FILE *f;
  
  if (NULL == (f = fopen(argv[1], "r"))) {
    perror("Failed opening shapefile");
    return -1;
  }
  
  ls_mainfile_t sf;
  bzero(&sf, sizeof(ls_mainfile_t));
  
  printf("Loading shapefile...");
  
  if (!ls_read_mainfile(&sf, f)) {
    printf("Couldn't read mainfile\n");
    exit (1);
  }
  
  printf("done.\n");
  
  fclose(f);
  
  if (NULL == (f = fopen(argv[2], "w"))) {
    perror("Failed opening output file");
    return -1;
  }
  
  write_kml(&sf, f);
  fclose(f);
  
  ls_free_mainfile_contents(&sf);
  
  return 0;
}

double orthogonal_distance(const point2_t *p, const point2_t* l1, const point2_t* l2)
{
  double d;
  
  if (l1->x == l2->x && l1->y == l2->y) {
    d = sqrt(pow(p->x - l1->x, 2) + pow(p->y - l1->y, 2));
  } else {
    d = fabs((l2->x - l1->x) * (l1->y - p->y) - (l1->x - p->x) * (l2->y - l1->y)) \
          / sqrt(pow(l2->x - l1->x, 2) + pow(l2->y - l1->y, 2));
  }

  return d;
}

/* Ramer-Douglas-Peucker simplification algorithm               */
/* (Pseudocode borrowed from Wikipedia)                         */
/* http://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm */
int dp_simplify(point2_t* pts_out, const point2_t* pts_in, uint len, double epsilon)
{
  double d, dmax = 0.0;
  int i, index = 0;
  
  /* find the point with the maximum distance */
  for (i = 1; i < len - 1; i++) {
    d = orthogonal_distance(pts_in + i, pts_in, pts_in + len - 1);
    if (d > dmax) {
      index = i;
      dmax = d;
    }
  }
  
  /* if max distance is greater than epsilon, recursively simplify */
  if (dmax >= epsilon) {
    int len1 = index;
    int len2 = len - index;
    int pts1, pts2;
    
    point2_t *res1 = malloc(len1 * sizeof(point2_t));
    point2_t *res2 = malloc(len2 * sizeof(point2_t));

    pts1 = dp_simplify(res1, pts_in,         len1, epsilon);

    pts2 = dp_simplify(res2, pts_in + index, len2, epsilon);

    memcpy(pts_out,            res1, (pts1-1)*sizeof(point2_t));
    memcpy(pts_out + pts1 - 1, res2, pts2*sizeof(point2_t));

    free(res1);
    free(res2);

    return pts1 + pts2 - 1;
  } else {
    pts_out[0] = pts_in[0];
    
    if (len > 1) pts_out[1] = pts_in[len-1];
    
    return 2;
  }
}

void write_kml(ls_mainfile_t *sf, FILE *out)
{
  fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", out);
  fputs("<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n", out);
  
  ls_shape_t *sptr = sf->head;
  int part_idx, point_idx, part_end_idx, orig_len, new_len;
  point2_t* simple_pts;
  
  fputs("<Document>\n", out);
  for(; sptr; sptr = sptr->next) {
    fputs("  <Placemark>\n", out);
    fprintf(out, "    <name>%i</name>\n", sptr->record_no);
    fputs("    <MultiGeometry>\n", out);
    
    simple_pts = malloc(sptr->num_points * sizeof(point2_t));
    
    for(part_idx = 0; part_idx < sptr->num_parts; part_idx++) {
      if (part_idx == sptr->num_parts - 1) {
        part_end_idx = sptr->num_points - 1;
      } else {
        part_end_idx = sptr->parts[part_idx+1] - 1;
      }

      printf("Simplifying record %i part %i...", sptr->record_no, part_idx);
      orig_len = part_end_idx - sptr->parts[part_idx] + 1;
      new_len = dp_simplify(simple_pts,                           \
                            sptr->points + sptr->parts[part_idx], \
                            orig_len, 0.01);
      printf("done. (%i%%) [old=%i new=%i]\n", 100 * new_len / orig_len, orig_len, new_len);

      fputs("      <Polygon>\n", out);
      fputs("        <extrude>1</extrude>\n", out);
      fputs("        <altitudeMode>relativeToGround</altitudeMode>\n", out);
      fputs("        <outerBoundaryIs>\n", out);
      fputs("          <LinearRing>\n", out);
      fputs("            <coordinates>\n", out);
      
      for (point_idx = 0; point_idx < new_len; point_idx++) {
        fprintf(out, "              %f,%f,%i\n", simple_pts[point_idx].x, simple_pts[point_idx].y, 1000);
      }
      
      fputs("            </coordinates>\n", out);
      fputs("          </LinearRing>\n", out);
      fputs("        </outerBoundaryIs>\n", out);
      fputs("      </Polygon>\n", out);
      
    }
    
    fputs("    </MultiGeometry>\n", out);
    fputs("  </Placemark>\n", out);
  }
  fputs("</Document>\n", out);
    
  fputs("</kml>\n", out);
}
