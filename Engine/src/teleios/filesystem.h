#ifndef TELEIOS_FILESYSTEM_FILES_H
#define TELEIOS_FILESYSTEM_FILES_H

#include "teleios/defines.h"
#include "teleios/types.h"

TLAPI const u64 tl_filesyste_file_size(const char* path);
TLAPI const TLFile* tl_filesystem_file_tochar(const char* path);
TLAPI const TLFile* tl_filesystem_file_tou32(const char* path);
TLAPI void tl_filesystem_file_free(const TLFile* file);

#endif // TELEIOS_FILESYSTEM_FILES_H
