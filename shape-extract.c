#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "shapefile.h"

#define OUTPUT_KML  1
#define OUTPUT_JSON 2

void extract_shape(const uint type, ls_shape_t *s, const ls_index_record_t *idx, FILE *f);
void write_shape_kml(const ls_shape_t *sptr, FILE *out, const uint height, const char *style);
void write_shape_json(const ls_shape_t *sptr, FILE *out, const uint height, const char *style);

int main(int argc, char** argv)
{
  int idx = 0, height = 100;
  char fetch_bounds = 0;
  char use_stdout = 1;
  char output = OUTPUT_KML;
  char *style = NULL;
  char c;
  
  while ((c = getopt(argc, argv, "t:n:h:s:b")) != -1) {
    switch(c) {
      case 'n':
        idx = atoi(optarg);
        break;
      case 'h':
        height = atoi(optarg);
        break;
      case 's':
        style = malloc(strlen(optarg));
        strcpy(style, optarg);
        break;
      case 't':
        if(0 == strcmp(optarg, "json")) {
          output = OUTPUT_JSON;
        } else if (0 == strcmp(optarg, "kml")) {
          output = OUTPUT_KML;
        } else {
          printf("unknown output type: %s\n", optarg);
          exit(1);
        }
        break;
      case 'b':
        fetch_bounds = 1;
        break;
      case '?':
        return 1;
    }
  }
  
  int argc_remaining = argc - optind;
  
  if (argc_remaining < 2 || argc_remaining > 3) {
    printf("usage: %s [-n <shape-index>] <indexfile> <mainfile> [<kml-file>]\n", argv[0]);
    exit(1);
  }
  
  if (argc_remaining == 4) {
    use_stdout = 0;
  }
  
  FILE *fidx, *fmain;
  
  if(NULL == (fidx = fopen(argv[optind], "r"))) {
    perror("Failed opening index");
    exit(1);
  }
  
  ls_indexfile_t indexfile;
  bzero(&indexfile, sizeof(ls_indexfile_t));
  
  if (!ls_read_indexfile(&indexfile, fidx)) {
    printf("Couldn't read index\n");
    exit (1);
  }
  
  fclose(fidx);
  
  if(NULL == (fmain = fopen(argv[optind+1], "r"))) {
    perror("Failed opening main file");
    exit(1);
  }
  
  ls_shape_t s;
  bzero(&s, sizeof(ls_shape_t));
  extract_shape(indexfile.info.type, &s, &indexfile.index[idx], fmain);
  s.record_no = idx;
  
  fclose(fmain);
  
  FILE *out;
  
  if (use_stdout) {
    out = stdout;
  } else {
    out = fopen(argv[optind+2], "w");
  }

  if (fetch_bounds) {
    printf("%f,%f,%f,%f\n", s.bounds.xmin, s.bounds.ymin, s.bounds.xmax, s.bounds.ymax);
  } else {
    if (output == OUTPUT_KML)       write_shape_kml(&s, out, height, style);
    else if (output == OUTPUT_JSON) write_shape_json(&s, out, height, style);  
  }
  
  if (!use_stdout) {
    fclose(out);  
  }
  
  free(style);
  ls_free_shape_contents(&s);
  ls_free_indexfile_contents(&indexfile);
  
  return 0;
}

void extract_shape(const uint type, ls_shape_t *s, const ls_index_record_t *idx, FILE *f)
{
  uchar *shpbuf = malloc(idx->length);
  
  fseek(f, idx->offset + SHAPEFILE_RECORD_HEADER_LENGTH, SEEK_SET);
  fread(shpbuf, sizeof(uchar), idx->length, f);
  
  ls_parse_shape(type, s, shpbuf, idx->length);
  
  free(shpbuf);
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

void write_shape_kml(const ls_shape_t *sptr, FILE *out, const uint height, const char *style)
{
  fputs("  <Placemark>\n", out);
  fprintf(out, "    <name>%i</name>\n", sptr->record_no);
  fprintf(out, "    <styleUrl>%s</styleUrl>\n", style);
  fputs("    <MultiGeometry>\n", out);
  
  int part_idx, point_idx, part_end_idx, orig_len, new_len;
  point2_t* simple_pts;
  
  simple_pts = malloc(sptr->num_points * sizeof(point2_t));
  
  for(part_idx = 0; part_idx < sptr->num_parts; part_idx++) {
    if (part_idx == sptr->num_parts - 1) {
      part_end_idx = sptr->num_points - 1;
    } else {
      part_end_idx = sptr->parts[part_idx+1] - 1;
    }

    orig_len = part_end_idx - sptr->parts[part_idx] + 1;
    new_len = dp_simplify(simple_pts,                           \
                          sptr->points + sptr->parts[part_idx], \
                          orig_len, 0.01);

    fputs("      <Polygon>\n", out);
    fputs("        <extrude>1</extrude>\n", out);
    fputs("        <altitudeMode>relativeToGround</altitudeMode>\n", out);
    fputs("        <outerBoundaryIs>\n", out);
    fputs("          <LinearRing>\n", out);
    fputs("            <coordinates>\n", out);
    
    for (point_idx = 0; point_idx < new_len; point_idx++) {
      fprintf(out, "              %f,%f,%i\n", simple_pts[point_idx].x, simple_pts[point_idx].y, height);
    }
    
    fputs("            </coordinates>\n", out);
    fputs("          </LinearRing>\n", out);
    fputs("        </outerBoundaryIs>\n", out);
    fputs("      </Polygon>\n", out);
    
  }
  
  fputs("    </MultiGeometry>\n", out);
  fputs("  </Placemark>\n", out);
}


void write_shape_json(const ls_shape_t *sptr, FILE *out, const uint height, const char *style)
{
  fputs("{\n  'Placemark': {\n", out);
  fprintf(out, "    'name': \"%i\",\n", sptr->record_no);
  fprintf(out, "    'styleUrl': \"%s\",\n", style);
  fputs("    'MultiGeometry': [\n", out);
  
  int part_idx, point_idx, part_end_idx, orig_len, new_len;
  point2_t* simple_pts;
  
  simple_pts = malloc(sptr->num_points * sizeof(point2_t));
  
  for(part_idx = 0; part_idx < sptr->num_parts; part_idx++) {
    if (part_idx == sptr->num_parts - 1) {
      part_end_idx = sptr->num_points - 1;
    } else {
      part_end_idx = sptr->parts[part_idx+1] - 1;
    }

    orig_len = part_end_idx - sptr->parts[part_idx] + 1;
    new_len = dp_simplify(simple_pts,                           \
                          sptr->points + sptr->parts[part_idx], \
                          orig_len, 0.01);

    fputs("      {\n", out);
    fputs("        'type': \"polygon\",\n", out);
    fputs("        'extrude': 1,\n", out);
    fputs("        'altitudeMode': \"relativeToGround\",\n", out);
    fputs("        'outerBoundaryIs': {\n", out);
    fputs("          'LinearRing': {\n", out);
    fputs("            'coordinates': [\n", out);
    
    for (point_idx = 0; point_idx < new_len; point_idx++) {
      fprintf(out, "              [%f, %f, %i],\n", simple_pts[point_idx].x, simple_pts[point_idx].y, height);
    }
    
    fputs("            ]\n", out);
    fputs("          }\n", out);
    fputs("        }\n", out);
    fputs("      },\n", out);
    
  }
  
  fputs("    ]\n", out);
  fputs("  }\n}\n", out);
}





