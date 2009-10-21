#include "shapefile.h"

/* internal functions */
void parse_polygon(shape_t *shp, const uchar *buf, const uint size);

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

int read_shapefile(shapefile_t *sf, FILE* f)
{
  uchar buf[SHAPEFILE_FILE_HEADER_LENGTH+1];
  
  if (SHAPEFILE_FILE_HEADER_LENGTH != fread(buf, sizeof(char), SHAPEFILE_FILE_HEADER_LENGTH, f)) {
    printf("Invalid header\n");
    return -1;
  }

  parse_file_header(&sf->info, buf);
  
  int record_no, record_length;
  uchar *shp_buf;
  shape_t *shp, *head;
  
  head = shp = malloc(sizeof(shape_t));
  bzero(shp, sizeof(shape_t));
  
  while (fread(buf, sizeof(uchar), SHAPEFILE_RECORD_HEADER_LENGTH, f)) {
    record_no     = SWAP_ENDIAN_32(*(uint*)buf);
    record_length = 2*SWAP_ENDIAN_32(*(uint*)(buf + 4)); /* arrives in words */
    
    shp_buf = malloc(record_length);
    
    if (record_length != fread(shp_buf, sizeof(uchar), record_length, f)) {
      printf("Invalid shape\n");
      free(shp_buf);
      free_shape_contents(head);
      free(head);
      return -1;
    }
    
    shp->next = malloc(sizeof(shape_t));
    shp = shp->next;
    bzero(shp, sizeof(shape_t));
    
    shp->record_no = record_no;
    parse_shape(sf->info.type, shp, shp_buf, record_length);
    
    free(shp_buf);
  }
  
  /* cut off the temporary head */
  shp = head;
  head = head->next;
  free(shp);
  
  sf->head = head;
  
  return 1;
}

void free_shapefile_contents(shapefile_t *sf)
{
  shape_t *tmp;
  
  while (tmp = sf->head) {
    sf->head = sf->head->next;
    free(tmp);
  }
}

void parse_shape(int type, shape_t *shp, const uchar *buf, const uint size)
{
  bzero(shp, sizeof(shp));
  shp->type = type;
  
  switch(type) {
  case SHAPE_TYPE_POLYGON:
    parse_polygon(shp, buf, size);
    break;
  default:
    printf("Invalid shape.\n");
    exit(1);
  }
}

void parse_polygon(shape_t *shp, const uchar *buf, const uint size)
{
  shp->type = SHAPE_TYPE_POLYGON;
  
  /* read the bounding box, num_parts and num_points */
  memcpy(&shp->bounds, buf+4, 4*sizeof(double) + 2*sizeof(int));
  
  shp->parts = malloc(shp->num_parts * sizeof(int));
  shp->points = malloc(shp->num_points * sizeof(point2_t));
  
  memcpy(shp->parts, buf+44, shp->num_parts * sizeof(int));
  memcpy(shp->points, buf+44 + shp->num_parts * sizeof(int), shp->num_points * sizeof(point2_t));
}
