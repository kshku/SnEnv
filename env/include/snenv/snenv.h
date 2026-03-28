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
 *
 * @return Returns pointer to value (owned by system) or NULL if not found.
 *
 * @note Returned value should not be freed, and is only valid till next call.
 */
SN_API const char *sn_env_var_get(const char *name);

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
 * @return Returns pointer to executable path string.
 *
 * @note Returns NULL if failed to get the string.
 *
 * @note Returned value is only valid till next call.
 */
SN_API const char *sn_env_get_exe_path(void);

/**
 * @brief Get current working directory.
 *
 * @return Returns pointer to cwd string.
 *
 * @note Returns NULL if failed to get the string.
 *
 * @note Returned value is only valid till next call.
 */
SN_API const char *sn_env_get_cwd(void);

// TODO: Arg processing
