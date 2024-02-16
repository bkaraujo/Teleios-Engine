#ifndef TELEIOS_FILESYSTEM_H
#define TELEIOS_FILESYSTEM_H

#include "Teleios/Types.h"

TLEXPORT u64 tl_filesystem_get_size(const char* path);

TLEXPORT const TLFile* tl_filesystem_read_raw(const char* path);
TLEXPORT const TLFile* tl_filesystem_read_string(const char* path);
TLEXPORT void tl_filesystem_free(const TLFile* file);

#endif // TELEIOS_FILESYSTEM_H