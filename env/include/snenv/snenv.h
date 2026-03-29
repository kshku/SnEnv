#pragma once

#include "snenv/defines.h"
#include "snenv/api.h"

/**
 * @struct snEnvVarEntry
 * @brief A struct representing environment variable and value pair.
 */
typedef struct snEnvVarEntry {
    const char *name; /**< Name of variable */
    const char *value; /**< Value of variable */
} snEnvVarEntry;

/**
 * @brief Get the value of an environment variable.
 *
 * @param name The name of environment variable.
 * @param value Buffer to store the value
 * @param size The size of the buffer
 *
 * @return Returns number of bytes written (0 if not found).
 *
 * @note If value or size is NULL, returns number of bytes required.
 */
SN_API uint64_t sn_env_var_get(const char *name, char *value, uint64_t size);

/**
 * @brief Set environment variable.
 *
 * @param name The name of variable.
 * @param value The variable value.
 * @param overwrite Should overwrite existing value.
 *
 * @return Returns true on success.
 */
SN_API bool sn_env_var_set(const char *name, const char *value, bool overwrite);

/**
 * @brief Unset environment variable.
 *
 * @param name Variable name.
 * 
 * @return Returns true on success.
 */
SN_API bool sn_env_var_unset(const char *name);

/**
 * @brief Iterate through environment variables.
 *
 * Once all the entries are iterated (after returning false), from next call onwards, starts from beginning.
 *
 * @return Returns true if more entries are there, false if iteration completed.
 *
 * @note Contents of entry are valid only upto next call to sn_env_var_read.
 *
 * @note Not thread safe, and iteration state is global.
 */
SN_API bool sn_env_var_read(snEnvVarEntry *entry);

/**
 * @brief Get the process id.
 *
 * @return Returns process id.
 */
SN_API uint64_t sn_env_get_process_id(void);

/**
 * @brief Get the parent process id.
 *
 * @return Returns process id.
 */
SN_API uint64_t sn_env_get_process_parent_id(void);

/**
 * @brief Get absolute executable path.
 *
 * @param path Buffer to store path
 * @param size Size of the buffer
 *
 * @return Returns number of bytes written.
 *
 * @note If path is NULL or size is 0, returns required size.
 */
SN_API uint64_t sn_env_get_exe_path(char *path, uint64_t size);

/**
 * @brief Get current working directory.
 *
 * @param cwd Buffer to store cwd
 * @param size Size of the buffer
 *
 * @return Returns number of bytes written.
 *
 * @note If cwd is NULL or size is 0, returns required size.
 */
SN_API uint64_t sn_env_get_cwd(char *cwd, uint64_t size);

// TODO: Arg processing
