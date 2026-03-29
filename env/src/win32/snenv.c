#include "snenv/snenv.h"

#if defined(SN_OS_WINDOWS)

#include <windows.h>
#include <tlhelp32.h>

uint64_t sn_env_var_get(const char *name, char *value, uint64_t size) {
    char buffer[40000] = {0};
    DWORD len = GetEnvironmentVariableA(name, buffer, SN_ARRAY_LENGTH(buffer));
    if (len == 0) {
        SN_ASSERT(GetLastError() == ERROR_ENVVAR_NOT_FOUND);
        return 0;
    }

    uint64_t i = 0;
    if (!value || !size) {
        for (i = 0; buffer[i]; ++i);
        return i + 1;
    }

    for (i = 0; i < size && buffer[i]; ++i) value[i] = buffer[i];

    if (i == size) value[i - 1] = 0;
    else value[i] = 0;

    return i;
}

bool sn_env_var_set(const char *name, const char *value, bool overwrite) {
    if (overwrite || sn_env_var_get(name, NULL, 0) == 0) return SetEnvironmentVariableA(name, value);
    // Var exists and value is not changed.
    return true;
}

bool sn_env_var_unset(const char *name) {
    return SetEnvironmentVariableA(name, NULL);
}

bool sn_env_var_read(snEnvVarEntry *entry) {
    // We have no way of calling FreeEnvironmentStrings()!
    static bool first_env = true;

    static LPTCH env = NULL;
    static LPTSTR var = NULL;

    if (first_env) {
        first_env = false;
        env = GetEnvironmentStrings();
        var = (LPTSTR)env;
    }

    SN_ASSERT(env);
    *entry = (snEnvVarEntry){0};

    if (!*var) {
        FreeEnvironmentStrings(env);
        first_env = true;
        env = NULL;
        var = NULL;
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

    return true;
}

uint64_t sn_env_get_process_id(void) {
    return (uint64_t)GetCurrentProcessId();
}

uint64_t sn_env_get_process_parent_id(void) {
    DWORD current_pid = GetCurrentProcessId();
    PROCESSENTRY32 pe = {0};

    // Take a snapshot of all Windows processes
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    SN_ASSERT(h != INVALID_HANDLE_VALUE);

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

uint64_t sn_env_get_exe_path(char *path, uint64_t size) {
    char buffer[MAX_PATH] = {0};

    DWORD result = GetModuleFileNameA(NULL, buffer, SN_ARRAY_LENGTH(buffer));
    if (result == 0 || result == SN_ARRAY_LENGTH(buffer)) return 0;

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
    char buffer[MAX_PATH] = {0};

    DWORD len = GetCurrentDirectoryA(SN_ARRAY_LENGTH(buffer), buffer);
    if (len == 0 || len > SN_ARRAY_LENGTH(buffer)) return 0;

    uint64_t i = 0;

    if (!cwd || !size) {
        for (i = 0; buffer[i]; ++i);
        return i + 1; // include one byte for NULL
    }

    for (i = 0; i < size && buffer[i]; ++i) cwd[i] = buffer[i];

    if (i == size) cwd[i - 1] = 0;
    else cwd[i] = 0;

    return i;
}

#endif
