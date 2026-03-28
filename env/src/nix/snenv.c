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

const char *sn_env_var_get(const char *name) {
    // Use secure_getenv()?
    return (const char *)getenv(name);
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
    for (i = 0; i < SN_ARRAY_LENGTH(buffer) && *env[i]; ++i) {
        if (*env[i] == '=') {
            buffer[i] = 0;
            entry->value = &buffer[i + 1];
        } else {
            buffer[i] = *env[i];
        }
    }
    // If assertion fails, buffer size is not enough
    SN_ASSERT(*env[i] == 0);
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

const char *sn_env_get_exe_path(void) {
    static char buffer[PATH_MAX] = {0};
    if (buffer[0]) return (const char *)buffer;
#if defined(SN_OS_LINUX)
    ssize_t count = readlink("/proc/self/exe", buffer, SN_ARRAY_LENGTH(buffer) - 1);
    if (count < 0) {
        buffer[0] = 0;
        return NULL;
    }
    buffer[count] = 0;
#else
    char raw_path[PATH_MAX] = {0};
    uint32_t raw_path_size = SN_ARRAY_LENGTH(raw_path);
    if (_NSGetExecutablePath(raw_path, &raw_path_size) != 0) return NULL;
    if (realpath(raw_path, buffer) == NULL) {
        buffer[0] = 0;
        return NULL;
    }
#endif
    return buffer;
}

const char *sn_env_get_cwd(void) {
    static char buffer[PATH_MAX];
    return getcwd(buffer, SN_ARRAY_LENGTH(buffer));
}

#endif
