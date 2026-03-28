#include "snenv/snenv.h"

#if defined(SN_OS_WINDOWS)

#include <windows.h>
#include <tlhelp32.h>

const char *sn_env_var_get(const char *name) {
    static char buffer[32767];
    DWORD size = GetEnvironmentVariableA(name, buffer, SN_ARRAY_LENGTH(buffer));
    if (size == 0) {
        SN_ASSERT(GetLastError() == ERROR_ENVVAR_NOT_FOUND);
        return NULL;
    }

    return (const char *)buffer;
}

bool sn_env_var_set(const char *name, const char *value, bool overwrite) {
    if (overwrite || sn_env_var_get(name) == NULL) return SetEnvironmentVariableA(name, value);
    // Var exists and value is not changed.
    return true;
}

bool sn_env_var_unset(const char *name) {
    return SetEnvironmentVariableA(name, NULL);
}

bool sn_env_var_read(snEnvVarEntry *entry) {
    // We have no way of calling FreeEnvironmentStrings()!
    static LPTCH env = GetEnvironmentStrings();
    static LPTSTR var = (LPTSTR)env;
    SN_ASSERT(env);
    *entry = (snEnvVarEntry){0};

    if (!*var) {
        var = (LPTSTR)env;
        return false;
    }

    size_t i = 0;
    static char buffer[40000] = {0};
    entry->name = &buffer[0];
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

    var += i + 1;

    return ture;
}

uint64_t sn_env_get_process_id(void) {
    return (uint64_t)GetCurrentProcessId();
}

uint64_t sn_env_get_process_parent_id(void) {
    uint64_t pid;
    DWORD current_pid = GetCurrentProcessId();
    PROCESSENTRY32 pe = {0};

    // Take a snapshot of all Windows processes
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    SN_ASSERT(h);

    pe.dwSize = sizeof(PROCESSENTRY32);

    // Iterate through the snapshot to find the current process
    if (Process32First(h, &pe)) {
        do {
            if (pe.th32ProcessID == current_pid) {
                CloseHandle(h);
                return (uint64_t)pe.th32ParentProcessID;
            }
        } while(Process32Next(h, &pe));
    }

    SN_SHOULD_NOT_REACH_HERE;
}

const char *sn_env_get_exe_path(void) {
    static char buffer[MAX_PATH] = {0};
    if (buffer[0]) return (const char *)buffer;

    DWORD result = GetModuleFileNameA(NULL, buffer, SN_ARRAY_LENGTH(buffer));
    if (result == 0 || result == SN_ARRAY_LENGTH(buffer)) {
        buffer[0] = 0;
        return NULL;
    }

    return (const char *)buffer;
}

const char *sn_env_get_cwd(void) {
    static char buffer[MAX_PATH] = {0};
    DWORD size = GetCurrentDirectory(SN_ARRAY_LENGTH(buffer), buffer);
    if (result == 0 || result > SN_ARRAY_LENGTH(buffer)) return NULL;
    return (const char *)buffer;
}

#endif
