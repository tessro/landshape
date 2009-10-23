#include "dbf.h"

#define DBF_HEADER_PREFIX_SIZE 32
#define DBF_FIELD_DESCRIPTOR_SIZE 32

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
