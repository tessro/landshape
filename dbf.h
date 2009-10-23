#ifndef SHAPE_H
#define SHAPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define DBF_HEADER_PREFIX_SIZE 32
#define DBF_FIELD_DESCRIPTOR_SIZE 32

#define MAX(a,b) ((a) > (b) ? (a) : (b))

#if !defined(uchar)
typedef unsigned char uchar;
#endif

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
  char type;
  
  union {
    int number;
    char string[256];
  } data;
} dbf_record_t;

void parse_dbf_header_prefix(dbfinfo_t *info, const uchar *buf);
void parse_dbf_field_descriptor(dbf_fieldinfo_t* fi, const uchar *buf);

void read_dbf_header(dbfinfo_t *info, FILE *f);

void dump_dbf_header(dbfinfo_t* info);

#ifdef __cplusplus
}
#endif

#endif
