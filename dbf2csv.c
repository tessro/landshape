#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define DBF_HEADER_PREFIX_SIZE 32
#define DBF_FIELD_DESCRIPTOR_SIZE 32

#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef unsigned char uchar;

typedef struct dbfinfo {
  uchar version;
  int updated_on;
  int num_records;
  ushort header_size, record_size;
  int num_fields;
  
  struct dbf_fieldinfo *fields;
} dbfinfo_t;

typedef struct dbf_fieldinfo {
  char name[11];
  char type;
  short length;
  char decimals;
} dbf_fieldinfo_t;

typedef struct dbf_record {
  union {
    int number;
    char* character;
  } data;
} dbf_record_t;

void parse_dbf_header_prefix(dbfinfo_t *info, const uchar *buf)
{
  info->version = buf[0];
  info->updated_on = (1900+buf[1])*10000 + buf[2] * 100 + buf[3];
  memcpy(&info->num_records, buf + 4, sizeof(int) + 2*sizeof(ushort));
  info->num_fields = (info->header_size - DBF_HEADER_PREFIX_SIZE) / DBF_FIELD_DESCRIPTOR_SIZE;
}

void parse_dbf_field_descriptor(dbf_fieldinfo_t* fi, const uchar *buf)
{
  memcpy(&fi->name, buf, 12*sizeof(char));
  
  fi->length = buf[16];
  fi->decimals = buf[17];
}

void read_dbf_header(dbfinfo_t *info, FILE *f)
{
  uchar buf[MAX(DBF_HEADER_PREFIX_SIZE, DBF_FIELD_DESCRIPTOR_SIZE)];
  
  fread(&buf, sizeof(uchar), DBF_HEADER_PREFIX_SIZE, f);
  
  parse_dbf_header_prefix(info, buf);
  
  info->fields = malloc(info->num_fields * sizeof(dbf_fieldinfo_t));
  
  int i;
  for (i = 0; i < info->num_fields; i++) {
    fread(&buf, sizeof(uchar), DBF_FIELD_DESCRIPTOR_SIZE, f);
    parse_dbf_field_descriptor(info->fields + i, buf);
  }
}

void dump_dbf_header(dbfinfo_t* info)
{
  printf("dbf version\t: %i\n", info->version);
  printf("last update\t: %i\n", info->updated_on);
  printf("records\t\t: %i\n", info->num_records);
  printf("fields\t\t: %i\n", info->num_fields);
  printf("header size\t: %i\n", info->header_size);
  printf("record size\t: %i\n", info->record_size);
  
  if (info->fields) {
    int i;
    for (i = 0; i < info->num_fields; i++) {
      printf("\n>> %s\n", info->fields[i].name);
      printf("type\t: %c\n", info->fields[i].type);
      printf("length\t: %i\n", info->fields[i].length);
    }
  }
}

int main(int argc, char** argv)
{
  assert(argc > 0);
  
  if (argc != 2) {
    printf("usage: %s <dbf-file>\n", argv[0]);
    exit(1);
  }
  
  FILE *f;
  
  if (NULL == (f = fopen(argv[1], "r"))) {
    perror("Failed opening database");
    return -1;
  }
  
  dbfinfo_t d;
  bzero(&d, sizeof(dbfinfo_t));
  
  read_dbf_header(&d, f);
  
  
  char *buf = malloc(d.record_size * sizeof(char));
  char *field_ptr, *field_end, *field_start;
  
  /* advance past header terminator */
  fseek(f, 1, SEEK_CUR);
  
  int record_idx, field_idx;
  for (field_idx = 0; field_idx < d.num_fields; field_idx++) {
    printf(d.fields[field_idx].name);
  
    if (field_idx == d.num_fields - 1) {
      printf("\n");
    } else {
      printf(",");
    }
  }
  
  for (record_idx = 0; record_idx < d.num_records; record_idx++) {
    fread(buf, sizeof(char), d.record_size, f);
      field_ptr = buf + 1; /* advance past delete flag */
    
    for (field_idx = 0; field_idx < d.num_fields; field_idx++) {
      char fbuf[256];
      bzero(fbuf, 256);
      memcpy(fbuf, field_ptr, d.fields[field_idx].length);
      
      /* strip starting whitespace */
      field_start = fbuf;
      while (field_start < (fbuf + d.fields[field_idx].length) && isspace(*field_start)) field_start++;
      
      /* strip ending whitespace */
      field_end = fbuf + d.fields[field_idx].length - 1;
      while (field_end > fbuf && isspace(*field_end)) field_end--;
      *(field_end + 1) = '\0';
      
      printf("\"%s\"", field_start);
      
      if (field_idx == d.num_fields - 1) {
        printf("\n");
      } else {
        printf(",");
      }
      
      field_ptr += d.fields[field_idx].length;
    }
  }
  
  free(buf);
  
  fclose(f);
  
  free(d.fields);
  
  return 0;
}
