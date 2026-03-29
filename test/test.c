#include <snenv/snenv.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEST_VAR_NAME  "SNENV_TEST_VAR"
#define TEST_VAR_VALUE "hello_world"

#define LARGE_VAR_NAME  "SNENV_LARGE_VAR"
#define LARGE_VAR_SIZE  8192

#define DYNAMIC_VAR_NAME "SNENV_DYNAMIC_VAR"

static uint64_t env_get_alloc(const char *name, char **out)
{
    uint64_t size = sn_env_var_get(name, NULL, 0);

    if (!size)
    {
        *out = NULL;
        return 0;
    }

    *out = malloc(size);

    return sn_env_var_get(name, *out, size);
}

static void test_env_get_set_unset(void)
{
    printf("test_env_get_set_unset...\n");

    sn_env_var_unset(TEST_VAR_NAME);

    char *value = NULL;

    SN_ASSERT(env_get_alloc(TEST_VAR_NAME, &value) == 0);
    SN_ASSERT(value == NULL);

    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, TEST_VAR_VALUE, true));

    env_get_alloc(TEST_VAR_NAME, &value);

    SN_ASSERT(value);
    SN_ASSERT(strcmp(value, TEST_VAR_VALUE) == 0);

    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, "different", false));

    free(value);
    env_get_alloc(TEST_VAR_NAME, &value);

    SN_ASSERT(strcmp(value, TEST_VAR_VALUE) == 0);

    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, "different", true));

    free(value);
    env_get_alloc(TEST_VAR_NAME, &value);

    SN_ASSERT(strcmp(value, "different") == 0);

    SN_ASSERT(sn_env_var_unset(TEST_VAR_NAME));

    free(value);

    SN_ASSERT(env_get_alloc(TEST_VAR_NAME, &value) == 0);

    printf("ok\n");
}

static void test_env_iteration(void)
{
    printf("test_env_iteration...\n");

    SN_ASSERT(sn_env_var_set(TEST_VAR_NAME, TEST_VAR_VALUE, true));

    snEnvVarEntry entry;

    int count = 0;
    int found_test_var = 0;

    while (sn_env_var_read(&entry))
    {
        SN_ASSERT(entry.name);
        SN_ASSERT(entry.value);

        SN_ASSERT(strchr(entry.name, '=') == NULL);

        if (strcmp(entry.name, TEST_VAR_NAME) == 0)
        {
            found_test_var = 1;
            SN_ASSERT(strcmp(entry.value, TEST_VAR_VALUE) == 0);
        }

        count++;
    }

    SN_ASSERT(count > 0);
    SN_ASSERT(found_test_var);

    printf("entries: %d\n", count);
}

static void test_env_iteration_reset(void)
{
    printf("test_env_iteration_reset...\n");

    snEnvVarEntry entry;

    int first = 0;
    int second = 0;

    while (sn_env_var_read(&entry))
        first++;

    while (sn_env_var_read(&entry))
        second++;

    SN_ASSERT(first > 0);
    SN_ASSERT(second > 0);

    SN_ASSERT(first == second);

    printf("count: %d\n", first);
}

static void test_large_env_value(void)
{
    printf("test_large_env_value...\n");

    char large[LARGE_VAR_SIZE];

    for (int i = 0; i < LARGE_VAR_SIZE - 1; i++)
        large[i] = 'A' + (i % 26);

    large[LARGE_VAR_SIZE - 1] = 0;

    SN_ASSERT(sn_env_var_set(LARGE_VAR_NAME, large, true));

    char *value;

    env_get_alloc(LARGE_VAR_NAME, &value);

    SN_ASSERT(value);
    SN_ASSERT(strcmp(value, large) == 0);

    free(value);
}

static void test_process_ids(void)
{
    printf("test_process_ids...\n");

    uint64_t pid = sn_env_get_process_id();
    uint64_t ppid = sn_env_get_process_parent_id();

    printf("pid=%llu ppid=%llu\n",
        (unsigned long long)pid,
        (unsigned long long)ppid);

    SN_ASSERT(pid > 0);
}

static void test_exe_path(void)
{
    printf("test_exe_path...\n");

    uint64_t needed =
        sn_env_get_exe_path(NULL, 0);

    SN_ASSERT(needed > 0);

    char *buf = malloc(needed);

    uint64_t written =
        sn_env_get_exe_path(buf, needed);

    SN_ASSERT(written > 0);
    SN_ASSERT(buf[0]);

    printf("exe=%s\n", buf);

    free(buf);
}

static void test_cwd(void)
{
    printf("test_cwd...\n");

    uint64_t needed =
        sn_env_get_cwd(NULL, 0);

    SN_ASSERT(needed > 0);

    char *buf = malloc(needed);

    uint64_t written =
        sn_env_get_cwd(buf, needed);

    SN_ASSERT(written > 0);

    printf("cwd=%s\n", buf);

    free(buf);
}

int main(void)
{
    printf("==== SnEnv Tests ====\n");

    test_env_get_set_unset();

    test_env_iteration();

    test_env_iteration_reset();

    test_large_env_value();

    test_process_ids();

    test_exe_path();

    test_cwd();

    printf("==== All SnEnv tests passed ====\n");

    return 0;
}
