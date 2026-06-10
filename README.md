# SnEnv

Cross-platform environment variable and process query library written in C.

Provides get/set/unset for environment variables, iteration over all variables,
process ID queries, executable path and current working directory retrieval.

## API

### Types

```c
typedef struct SnEnvVarEntry {
    const char *name;
    const char *value;
} SnEnvVarEntry;
```

### Functions

| Function | Description |
|----------|-------------|
| `uint64_t sn_env_var_get(const char *name, char *value, uint64_t size)` | Get env var value. Pass NULL/0 to query required size. |
| `bool sn_env_var_set(const char *name, const char *value, bool overwrite)` | Set env var. `overwrite` controls whether existing values are replaced. |
| `bool sn_env_var_unset(const char *name)` | Unset/remove an env var. |
| `bool sn_env_var_read(SnEnvVarEntry *entry)` | Iterate all env vars. Returns false when done; next call restarts. |
| `uint64_t sn_env_get_process_id(void)` | Get current process ID. |
| `uint64_t sn_env_get_process_parent_id(void)` | Get parent process ID. |
| `uint64_t sn_env_get_exe_path(char *path, uint64_t size)` | Get executable path. Pass NULL/0 to query required size. |
| `uint64_t sn_env_get_cwd(char *cwd, uint64_t size)` | Get current working directory. Pass NULL/0 to query required size. |

## Usage

```c
#include <snenv/snenv.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Set and get an environment variable
    sn_env_var_set("MY_VAR", "hello", true);

    uint64_t size = sn_env_var_get("MY_VAR", NULL, 0);
    char *val = malloc(size);
    sn_env_var_get("MY_VAR", val, size);
    printf("MY_VAR = %s\n", val);
    free(val);

    // Process info
    printf("PID = %llu\n", (unsigned long long)sn_env_get_process_id());

    // Current working directory
    size = sn_env_get_cwd(NULL, 0);
    char *cwd = malloc(size);
    sn_env_get_cwd(cwd, size);
    printf("CWD = %s\n", cwd);
    free(cwd);

    sn_env_var_unset("MY_VAR");
    return 0;
}
```

## Adding to your project

```cmake
include(FetchContent)
FetchContent_Declare(snenv
    GIT_REPOSITORY https://github.com/kshku/SnEnv.git
    GIT_TAG main
)
FetchContent_MakeAvailable(snenv)

target_link_libraries(myapp PRIVATE snenv)
```

## Build

```sh
cmake -B build
cmake --build build
```

| Option | Default | Description |
|--------|---------|-------------|
| `SN_ENV_BUILD_SHARED` | `OFF` | Build as shared library |
| `SN_ENV_BUILD_TEST` | `OFF` | Build tests |

## Platform Support

| Platform | Backend |
|----------|---------|
| Linux | `getenv`/`setenv`/`unsetenv`, `procfs` for exe path |
| macOS | `getenv`/`setenv`/`unsetenv`, `_NSGetExecutablePath` |
| Windows | `GetEnvironmentVariableA`/`SetEnvironmentVariableA`, `GetModuleFileNameA` |

## Dependencies

- **SnCore** — fetched automatically via FetchContent
