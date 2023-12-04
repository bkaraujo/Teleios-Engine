#ifndef TELEIOS_FILESYSTEM_FILES_H
#define TELEIOS_FILESYSTEM_FILES_H

#include "teleios/defines.h"
#include "teleios/types.h"

/// <summary>
/// Read a file content do string. The string content must be freed with a call to tl_memory_free(my_string, TL_MEMORY_TYPE_STRING, length);
/// </summary>
/// <param name="path">Absolute file path</param>
/// <returns>Heap allocated result, or NULL otherwise</returns>
const char* tl_filesystem_file_load(const char* path);

const u64 tl_filesyste_file_size(const char* path);

#endif // TELEIOS_FILESYSTEM_FILES_H
