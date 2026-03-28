#include <snenv/snenv.h>

#include <stdio.h>
#include <string.h>

#define TEST_VAR_NAME  "SNENV_TEST_VAR"
#define TEST_VAR_VALUE "hello_world"

#define LARGE_VAR_NAME  "SNENV_LARGE_VAR"
#define LARGE_VAR_SIZE  8192

#define DYNAMIC_VAR_NAME "SNENV_DYNAMIC_VAR"

static void test_env_get_set_unset(void) {
    printf("test_env_get_set_unset...\n");

    sn_env_var_unset(TEST_VAR_NAME);

    const char *value = sn_env_var_get(TEST_VAR_NAME);
    SN_ASSERT(value == NULL);

    /* set */
    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, TEST_VAR_VALUE, true));

    value = sn_env_var_get(TEST_VAR_NAME);

    SN_ASSERT(value != NULL);
    SN_ASSERT(strcmp(value, TEST_VAR_VALUE) == 0);

    /* overwrite=false should NOT overwrite */
    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, "different", false));

    value = sn_env_var_get(TEST_VAR_NAME);

    SN_ASSERT(strcmp(value, TEST_VAR_VALUE) == 0);

    /* overwrite=true should overwrite */
    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, "different", true));

    value = sn_env_var_get(TEST_VAR_NAME);

    SN_ASSERT(strcmp(value, "different") == 0);

    /* unset */
    SN_ASSERT(sn_env_var_unset(TEST_VAR_NAME));

    value = sn_env_var_get(TEST_VAR_NAME);

    SN_ASSERT(value == NULL);
}

static void test_env_iteration(void) {
    printf("test_env_iteration...\n");

    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, TEST_VAR_VALUE, true));

    snEnvVarEntry entry;

    int count = 0;
    int found_test_var = 0;

    while (sn_env_var_read(&entry)) {
        SN_ASSERT(entry.name != NULL);
        SN_ASSERT(entry.value != NULL);

        /* env names must not contain '=' */
        SN_ASSERT(strchr(entry.name, '=') == NULL);

        if (strcmp(entry.name, TEST_VAR_NAME) == 0) {
            found_test_var = 1;
            SN_ASSERT(strcmp(entry.value, TEST_VAR_VALUE) == 0);
        }

        count++;
    }

    SN_ASSERT(count > 0);
    SN_ASSERT(found_test_var);
}

static void test_env_iteration_reset(void) {
    printf("test_env_iteration_reset...\n");

    snEnvVarEntry entry;

    int first_pass = 0;
    int second_pass = 0;

    while (sn_env_var_read(&entry))
        first_pass++;

    while (sn_env_var_read(&entry))
        second_pass++;

    SN_ASSERT(first_pass > 0);
    SN_ASSERT(second_pass > 0);

    /* should be roughly same number of entries */
    SN_ASSERT(first_pass == second_pass);
}

static void test_env_iteration_mutation(void) {
    printf("test_env_iteration_mutation...\n");

    sn_env_var_set(DYNAMIC_VAR_NAME, "initial", true);

    snEnvVarEntry entry;

    int found_dynamic = 0;

    while (sn_env_var_read(&entry)) {
        if (!found_dynamic && strcmp(entry.name, TEST_VAR_NAME) == 0) {
            /* mutate env while iterating */
            SN_ASSERT(sn_env_var_set(DYNAMIC_VAR_NAME, "changed", true));
        }

        if (strcmp(entry.name, DYNAMIC_VAR_NAME) == 0)
            found_dynamic = 1;
    }

    SN_ASSERT(found_dynamic);

    const char *value = sn_env_var_get(DYNAMIC_VAR_NAME);
    SN_ASSERT(strcmp(value, "changed") == 0);
}

static void test_large_env_value(void) {
    printf("test_large_env_value...\n");

    static char large_value[LARGE_VAR_SIZE];

    for (int i = 0; i < LARGE_VAR_SIZE - 1; ++i)
        large_value[i] = 'A' + (i % 26);

    large_value[LARGE_VAR_SIZE - 1] = 0;

    SN_ASSERT(sn_env_var_set(LARGE_VAR_NAME, large_value, true));

    const char *value = sn_env_var_get(LARGE_VAR_NAME);

    SN_ASSERT(value != NULL);
    SN_ASSERT(strcmp(value, large_value) == 0);
}

static void test_process_ids(void) {
    printf("test_process_ids...\n");

    uint64_t pid = sn_env_get_process_id();
    uint64_t ppid = sn_env_get_process_parent_id();

    printf("pid  = %llu\n", (unsigned long long)pid);
    printf("ppid = %llu\n", (unsigned long long)ppid);

    SN_ASSERT(pid > 0);

    /* parent may be 0 in containers */
    SN_ASSERT(ppid >= 0);
}

static void test_exe_path(void) {
    printf("test_exe_path...\n");

    const char *path1 = sn_env_get_exe_path();

    SN_ASSERT(path1 != NULL);
    SN_ASSERT(path1[0] != 0);

    printf("exe path: %s\n", path1);

    /* pointer should remain stable (cached) */
    const char *path2 = sn_env_get_exe_path();

    SN_ASSERT(path1 == path2);
}

static void test_cwd(void) {
    printf("test_cwd...\n");

    const char *cwd1 = sn_env_get_cwd();

    SN_ASSERT(cwd1 != NULL);
    SN_ASSERT(cwd1[0] != 0);

    printf("cwd: %s\n", cwd1);

    const char *cwd2 = sn_env_get_cwd();

    SN_ASSERT(cwd2 != NULL);

    /* pointer may be same static buffer */
    SN_ASSERT(strcmp(cwd1, cwd2) == 0);
}

int main(void) {

    printf("==== SnEnv Tests ====\n");

    test_env_get_set_unset();

    test_env_iteration();

    test_env_iteration_reset();

    test_env_iteration_mutation();

    test_large_env_value();

    test_process_ids();

    test_exe_path();

    test_cwd();

    printf("==== All SnEnv tests passed ====\n");

    return 0;
}
