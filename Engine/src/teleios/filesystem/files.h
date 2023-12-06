#ifndef TELEIOS_FILESYSTEM_FILES_H
#define TELEIOS_FILESYSTEM_FILES_H

#include "teleios/defines.h"
#include "teleios/types.h"

const u64 tl_filesyste_file_size(const char* path);
const char* tl_filesystem_file_tochar(const char* path);
const u32* tl_filesystem_file_tou32(const char* path);

#endif // TELEIOS_FILESYSTEM_FILES_H
