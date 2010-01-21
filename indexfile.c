#include "shapefile.h"

int ls_read_indexfile(ls_indexfile_t *idxfile, FILE* f)
{
  char buf[SHAPEFILE_FILE_HEADER_LENGTH];
  
  fread(buf, sizeof(uchar), SHAPEFILE_FILE_HEADER_LENGTH, f);
  ls_parse_shapefile_header(&idxfile->info, buf);
  
  idxfile->num_records = (idxfile->info.byte_length - SHAPEFILE_FILE_HEADER_LENGTH) / SHAPEFILE_INDEX_RECORD_LENGTH;
  
  idxfile->index = malloc(idxfile->num_records * sizeof(ls_index_record_t));
  
  int i;
  uint temp[2];
  ls_index_record_t *rec;
  for (i = 0; i < idxfile->num_records; i++) {
    rec = idxfile->index + i;
    
    fread(&temp, sizeof(uchar), SHAPEFILE_INDEX_RECORD_LENGTH, f);
    rec->offset = 2*SWAP_ENDIAN_32(temp[0]);
    rec->length = 2*SWAP_ENDIAN_32(temp[1]);
  }
}

void ls_free_indexfile_contents(ls_indexfile_t *idxfile)
{
  free(idxfile->index);
}
