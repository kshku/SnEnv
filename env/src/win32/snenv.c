#include "snenv/snenv.h"

#if defined(SN_OS_WINDOWS)

// clang-format off
    #include <windows.h>
    #include <tlhelp32.h>
    #include <sncore/utf.h>
// clang-format on

uint64_t sn_env_var_get(const char *name, char *value, uint64_t size) {
    wchar_t wname[1024];
    if (sn_utf8_to_utf16(name, wname, SN_ARRAY_LENGTH(wname)) == (size_t)-1) return 0;

    wchar_t wbuffer[32768];
    DWORD len = GetEnvironmentVariableW(wname, wbuffer, SN_ARRAY_LENGTH(wbuffer));
    if (len == 0) {
        SN_ASSERT(GetLastError() == ERROR_ENVVAR_NOT_FOUND);
        return 0;
    }

    size_t needed = sn_utf16_to_utf8(wbuffer, NULL, 0);
    if (needed == (size_t)-1) return 0;

    if (!value || !size) return needed + 1;

    size_t written = sn_utf16_to_utf8(wbuffer, value, size);
    if (written == (size_t)-1) return 0;
    if (written >= size) value[size - 1] = 0;

    return written;
}

bool sn_env_var_set(const char *name, const char *value, bool overwrite) {
    if (overwrite || sn_env_var_get(name, NULL, 0) == 0) {
        wchar_t wname[1024];
        if (sn_utf8_to_utf16(name, wname, SN_ARRAY_LENGTH(wname)) == (size_t)-1) return false;

        if (value) {
            wchar_t wvalue[32768];
            if (sn_utf8_to_utf16(value, wvalue, SN_ARRAY_LENGTH(wvalue)) == (size_t)-1)
                return false;
            return SetEnvironmentVariableW(wname, wvalue);
        }

        return SetEnvironmentVariableW(wname, NULL);
    }
    return true;
}

bool sn_env_var_unset(const char *name) {
    wchar_t wname[1024];
    if (sn_utf8_to_utf16(name, wname, SN_ARRAY_LENGTH(wname)) == (size_t)-1) return false;
    return SetEnvironmentVariableW(wname, NULL);
}

bool sn_env_var_read(SnEnvVarEntry *entry) {
    static bool first_env = true;

    static wchar_t *env = NULL;
    static wchar_t *var = NULL;

    if (first_env) {
        first_env = false;
        env = GetEnvironmentStringsW();
        var = env;
    }

    SN_ASSERT(env);
    *entry = (SnEnvVarEntry){0};

    if (!*var) {
        FreeEnvironmentStringsW(env);
        first_env = true;
        env = NULL;
        var = NULL;
        return false;
    }

    static char buffer[40000] = {0};
    size_t written = sn_utf16_to_utf8(var, buffer, SN_ARRAY_LENGTH(buffer));
    if (written == (size_t)-1) return false;

    entry->name = &buffer[0];
    for (size_t i = 0; i < written; ++i) {
        if (buffer[i] == '=') {
            buffer[i] = 0;
            entry->value = &buffer[i + 1];
            break;
        }
    }

    while (*var) ++var;
    ++var;

    return true;
}

uint64_t sn_env_get_process_id(void) {
    return (uint64_t)GetCurrentProcessId();
}

uint64_t sn_env_get_process_parent_id(void) {
    DWORD current_pid = GetCurrentProcessId();
    PROCESSENTRY32 pe = {0};

    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    SN_ASSERT(h != INVALID_HANDLE_VALUE);

    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(h, &pe)) {
        do {
            if (pe.th32ProcessID == current_pid) {
                CloseHandle(h);
                return (uint64_t)pe.th32ParentProcessID;
            }
        } while (Process32Next(h, &pe));
    }

    SN_SHOULD_NOT_REACH_HERE;
    return 0;
}

uint64_t sn_env_get_exe_path(char *path, uint64_t size) {
    wchar_t wbuffer[MAX_PATH];

    DWORD result = GetModuleFileNameW(NULL, wbuffer, SN_ARRAY_LENGTH(wbuffer));
    if (result == 0 || result == SN_ARRAY_LENGTH(wbuffer)) return 0;

    size_t needed = sn_utf16_to_utf8(wbuffer, NULL, 0);
    if (needed == (size_t)-1) return 0;

    if (!path || !size) return needed + 1;

    size_t written = sn_utf16_to_utf8(wbuffer, path, size);
    if (written == (size_t)-1) return 0;
    if (written >= size) path[size - 1] = 0;

    return written;
}

uint64_t sn_env_get_cwd(char *cwd, uint64_t size) {
    wchar_t wbuffer[MAX_PATH];

    DWORD len = GetCurrentDirectoryW(SN_ARRAY_LENGTH(wbuffer), wbuffer);
    if (len == 0 || len > SN_ARRAY_LENGTH(wbuffer)) return 0;

    size_t needed = sn_utf16_to_utf8(wbuffer, NULL, 0);
    if (needed == (size_t)-1) return 0;

    if (!cwd || !size) return needed + 1;

    size_t written = sn_utf16_to_utf8(wbuffer, cwd, size);
    if (written == (size_t)-1) return 0;
    if (written >= size) cwd[size - 1] = 0;

    return written;
}

#endif
