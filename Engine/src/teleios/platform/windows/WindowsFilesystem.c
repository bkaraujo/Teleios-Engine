#include "teleios/platform/detector.h"
#ifdef TL_PLATFORM_WINDOWS
#include "teleios/platform/windows/common.h"
#include "teleios/filesystem.h"
#include "teleios/core/types.h"
#include "teleios/memory/allocator.h"
#include "teleios/logger/console.h"

TLEXPORT u64 tl_filesystem_get_size(const char* path) {
    WIN32_FILE_ATTRIBUTE_DATA fad = { 0 };
    if (!GetFileAttributesEx(path, GetFileExInfoStandard, &fad)) {
        TLWARN("tl_filesystem_get_size: Failed to GetFileAttributesEx: 0x%x", GetLastError());
        return U64MAX;
    }

    LARGE_INTEGER size = { 0 };
    size.HighPart = fad.nFileSizeHigh;
    size.LowPart = fad.nFileSizeLow;

    return size.QuadPart;
}

static b8 tl_filesystem_read(TLFile* file) {
    HANDLE handle = CreateFile(file->path,      // file to open
        GENERIC_READ,                           // open for reading
        FILE_SHARE_READ,                        // share for reading
        NULL,                                   // default security
        OPEN_EXISTING,                          // existing file only
        FILE_ATTRIBUTE_NORMAL,                  // normal file
        NULL);                                  // no attr. template

    if (handle == INVALID_HANDLE_VALUE) {
        TLWARN("tl_filesystem_read: Failed to open \"%s\": 0x%x", file->path, GetLastError());
        return false;
    }

    if (!ReadFile(handle, file->payload.raw, (DWORD)file->size, NULL, NULL)) {
        CloseHandle(handle);
        TLWARN("tl_filesystem_read: Failed to read \"%s\": 0x%x", file->path, GetLastError());
        return false;
    }

    return true;
}

TLEXPORT const TLFile* tl_filesystem_read_raw(const char* path) {
    u64 file_size = tl_filesystem_get_size(path);
    if (file_size == U64MAX) {
        TLWARN("tl_filesystem_read_raw: Failed get file \"%s\" size", path);
        return NULL;
    }

    TLFile* file = tl_memory_halloc(TL_MEMORY_TYPE_FILE, sizeof(TLFile));
    file->path = path;
    *((u64*)&file->size) = file_size;
    file->payload.raw = tl_memory_halloc(TL_MEMORY_TYPE_FILE, file->size);

    if (!tl_filesystem_read(file)) {
        TLERROR("tl_filesystem_read_raw: Failed to read file \"%s\"", path);
        tl_memory_hfree(TL_MEMORY_TYPE_FILE, file->payload.raw, file->size);
        tl_memory_hfree(TL_MEMORY_TYPE_FILE, file, sizeof(TLFile));
        return NULL;
    }

    return file;
}

TLEXPORT const TLFile* tl_filesystem_read_string(const char* path) {
    u64 file_size = tl_filesystem_get_size(path);
    if (file_size == U64MAX) {
        TLWARN("tl_filesystem_read_string: Failed get file \"%s\" size", path);
        return NULL;
    }

    TLFile* file = tl_memory_halloc(TL_MEMORY_TYPE_FILE, sizeof(TLFile));
    file->path = path;
    *((u64*)&file->size) = file_size;
    file->payload.raw = tl_memory_halloc(TL_MEMORY_TYPE_FILE, file->size + 1);

    if (!tl_filesystem_read(file)) {
        TLERROR("tl_filesystem_read_string: Failed to read file \"%s\"", path);
        tl_memory_hfree(TL_MEMORY_TYPE_FILE, file->payload.raw, file->size);
        tl_memory_hfree(TL_MEMORY_TYPE_FILE, file, sizeof(TLFile));
        return NULL;
    }

    *((u64*)&file->size) = file_size + 1;
    return file;
}

TLEXPORT void tl_filesystem_free(const TLFile* file) {
    if (file == NULL) return;

    tl_memory_hfree(TL_MEMORY_TYPE_FILE, file->payload.raw, file->size);
    tl_memory_hfree(TL_MEMORY_TYPE_FILE, (void*)file, sizeof(TLFile));
}
#endif // TL_PLATFORM_WINDOWS