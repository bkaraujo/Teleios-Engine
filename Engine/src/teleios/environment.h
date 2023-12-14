#ifndef TELEIOS_ENVIRONMENT_H

#include "teleios/types.h"

typedef enum {
    TL_ENV_A,
    TL_ENV_B
} TLEnvironmentVariables;

b8 tl_environment_initialize(void);
b8 tl_environment_terminate(void);

b8 tl_environment_set(const TLEnvironmentVariables key, const void* value);
b8 tl_environment_remove(const TLEnvironmentVariables key);
const void* tl_environment_get(const TLEnvironmentVariables key);

#endif // TELEIOS_ENVIRONMENT_H
