#define _GNU_SOURCE
#include "snenv/snenv.h"

#if defined(SN_OS_LINUX) || defined(SN_OS_MAC)

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h>

#if defined(SN_OS_MAC)
#include <mach-o/dyld.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

uint64_t sn_env_var_get(const char *name, char *value, uint64_t size) {
    // Use secure_getenv()?
    const char *res = (const char *)getenv(name);
    if (!res) return 0;

    uint64_t i = 0;
    if (!value || !size) {
        for (i = 0; res[i]; ++i);
        return i + 1;
    }

    for (i = 0; i < size && res[i]; ++i) value[i] = res[i];

    if (i == size) value[i - 1] = 0;
    else value[i] = 0;

    return i;
}

bool sn_env_var_set(const char *name, const char *value, bool overwrite) {
    return setenv(name, value, (int)overwrite) == 0;
}

bool sn_env_var_unset(const char *name) {
    return unsetenv(name) == 0;
}

bool sn_env_var_read(snEnvVarEntry *entry) {
    extern char **environ;

    static bool first = true;
    static char **env = NULL;

    if (first) {
        first = false;
        env = environ;
    }

    *entry = (snEnvVarEntry){0};

    if (!(*env)) {
        env = environ;
        return false;
    }

    size_t i = 0;
    static char buffer[40000] = {0};
    entry->name = &buffer[0];
    const char *var = *env;
    for (i = 0; i < SN_ARRAY_LENGTH(buffer) && var[i]; ++i) {
        if (var[i] == '=') {
            buffer[i] = 0;
            entry->value = &buffer[i + 1];
        } else {
            buffer[i] = var[i];
        }
    }
    // If assertion fails, buffer size is not enough
    SN_ASSERT(var[i] == 0);
    buffer[i] = 0;

    ++env;

    return true;
}

uint64_t sn_env_get_process_id(void) {
    return (uint64_t)getpid();
}

uint64_t sn_env_get_process_parent_id(void) {
    return (uint64_t)getppid();
}

uint64_t sn_env_get_exe_path(char *path, uint64_t size) {
    char buffer[PATH_MAX] = {0};

#if defined(SN_OS_LINUX)
    ssize_t count = readlink("/proc/self/exe", buffer, SN_ARRAY_LENGTH(buffer) - 1);
    if (count < 0) return 0;

    buffer[count] = 0;
#else
    char raw_path[PATH_MAX] = {0};
    uint32_t raw_path_size = SN_ARRAY_LENGTH(raw_path);
    if (_NSGetExecutablePath(raw_path, &raw_path_size) != 0) return 0;
    if (realpath(raw_path, buffer) == NULL) return 0;
#endif


    uint64_t i = 0;
    if (!path || !size) {
        for (i = 0; buffer[i]; ++i);
        return i + 1; // include one byte for NULL
    }

    for (i = 0; i < size && buffer[i]; ++i) path[i] = buffer[i];

    if (i == size) path[i - 1] = 0;
    else path[i] = 0;

    return i;
}

uint64_t sn_env_get_cwd(char *cwd, uint64_t size) {
    char buffer[PATH_MAX] = {0};
    char *res = getcwd(buffer, SN_ARRAY_LENGTH(buffer));

    if (!res) return 0;

    uint64_t i = 0;
    if (!cwd || !size) {
        for (i = 0; res[i]; ++i);
        return i + 1; // include one byte for NULL
    }

    for (i = 0; i < size && res[i]; ++i) cwd[i] = res[i];

    if (i == size) cwd[i - 1] = 0;
    else cwd[i] = 0;

    return i;
}

#endif
