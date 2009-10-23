#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dbf.h"


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
  
  /* advance past header terminator */
  fseek(f, 1, SEEK_CUR);
  
  printf("RECORD_NO,");
  
  int record_idx, field_idx;
  for (field_idx = 0; field_idx < d.num_fields; field_idx++) {
    printf(d.fields[field_idx].name);
  
    if (field_idx == d.num_fields - 1) {
      printf("\n");
    } else {
      printf(",");
    }
  }
  
  char *buf = malloc(d.record_size * sizeof(char));
  char *field_ptr, *field_start, *field_end;
  dbf_record_t *r;
  
  for (record_idx = 0; record_idx < d.num_records; record_idx++) {
    fread(buf, sizeof(char), d.record_size, f);
    field_ptr = buf + 1; /* advance past delete flag */
    
    printf("%i,", record_idx);
    
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
      
      //strcpy(&r->data.string, )
      
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
