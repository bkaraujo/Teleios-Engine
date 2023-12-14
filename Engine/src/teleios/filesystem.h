#ifndef TELEIOS_FILESYSTEM_FILES_H
#define TELEIOS_FILESYSTEM_FILES_H

#include "teleios/defines.h"
#include "teleios/types.h"

typedef enum {
    TL_FILE_TYPE_STRING,
    TL_FILE_TYPE_U32
} TLFileType;

typedef struct {
    const TLFileType type;
    const u64 size;
    const char* path;
    union {
        const void* raw;
        const u32* u32;
        const char* string;
    } payload;
} TLFile;

TLAPI const u64 tl_filesyste_file_size(const char* path);
TLAPI const TLFile* tl_filesystem_file_tochar(const char* path);
TLAPI const TLFile* tl_filesystem_file_tou32(const char* path);
TLAPI void tl_filesystem_file_free(const TLFile* file);

#endif // TELEIOS_FILESYSTEM_FILES_H
