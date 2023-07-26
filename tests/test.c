#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lwiniparser.h"

#define TEST_ASSERT_EQU_BASE(equ,exp,act,fmt) \
    do { \
        test_count++; \
        if (equ) { \
            test_passed++; \
        } else { \
            printf("Test assert: %s:%d:expected " fmt ", but got " fmt "\r\n", \
                __func__, __LINE__,exp, act); \
        } \
    } while (0)

#define TEST_ASSERT_EQU_INT(exp,act) TEST_ASSERT_EQU_BASE((exp) == (act),exp,act,"%d")
#define TEST_ASSERT_EQU_TRUE(exp) TEST_ASSERT_EQU_BASE((exp) != 0,"true","false","%s")
#define TEST_ASSERT_EQU_FALSE(exp) TEST_ASSERT_EQU_BASE((exp) == 0,"false","true","%s")
#define TEST_ASSERT_EQU_STR(exp,act) TEST_ASSERT_EQU_BASE(strcmp(exp,act) == 0,exp,act,"%s")



static int test_return = 0;

static int test_count = 0;
static int test_passed = 0;

static void test_parser_empty(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }

    test_data = "\r\n\r\n";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }

    test_data = ";";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }

    test_data = "\r\n;\r\n";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }

    test_data = "\n\n;";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }

    test_data = "    \n   \n   ;    ";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }
}


static void test_parser_empty_key(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "[testsection]";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini != NULL);

    if(ini)
    {
        lwini_t *section = NULL;
        section = lwini_get_section(ini, "testsection");
        TEST_ASSERT_EQU_TRUE(section != NULL);
        if(section)
        {
            TEST_ASSERT_EQU_INT(0, lwini_get_key_num(section));
            TEST_ASSERT_EQU_STR("testsection", section->key);
        }
    }

    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }
}

static void test_parser_empty_value(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "[testsection]\r\nkey1=\r\nkey2=;=123 ";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini != NULL);

    if(ini)
    {
        lwini_t *section = NULL;
        lwini_t *key = NULL;
        section = lwini_get_section(ini, "testsection");
        TEST_ASSERT_EQU_TRUE(section != NULL);
        if(section)
        {
            TEST_ASSERT_EQU_INT(2, lwini_get_key_num(section));
            TEST_ASSERT_EQU_STR("testsection", section->key);
            key = lwini_get_value(section, "key1");
            TEST_ASSERT_EQU_TRUE(key != NULL);
            if(key)
            {
                TEST_ASSERT_EQU_STR("key1", key->key);
                TEST_ASSERT_EQU_STR("", key->value);
            }
            key = lwini_get_value(section, "key2");
            TEST_ASSERT_EQU_TRUE(key != NULL);
            if(key)
            {
                TEST_ASSERT_EQU_STR("key2", key->key);
                TEST_ASSERT_EQU_STR("", key->value);
            }
        }
    }

    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }
}

static void test_parser_spaced(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "    \r\n  [blanks]  \r\n  \r\n  \r\n  a=1\r\n  b=2;\r\n  c=3; comment\r\n \
    d=4# comment\r\n  [   spaced   ]\r\n  a=1\r\n  b=2;\r\n  c=3; comment\r\n  d = 4# co]";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini != NULL);
    if(ini)
    {
        lwini_t *key = NULL;

        key = lwini_get_value_by_section(ini, "blanks", "d");
        TEST_ASSERT_EQU_TRUE(key != NULL);
        if(key)
        {
            TEST_ASSERT_EQU_STR("d", key->key);
            TEST_ASSERT_EQU_STR("4", key->value);
        }

        key = lwini_get_value_by_section(ini, "spaced", "d");
        TEST_ASSERT_EQU_TRUE(key != NULL);
        if(key)
        {
            TEST_ASSERT_EQU_STR("d", key->key);
            TEST_ASSERT_EQU_STR("4", key->value);
        }

    }

    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }
}

static void test_parser_long_key(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "# Stress testing buffers for overflows\r\n\
[long]\r\n\
# Shitload key size\r\n\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa=1";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini != NULL);
    
    if(ini)
    {
        lwini_destroy(ini);
        ini = NULL;
    }
}

static void test_parser_normal(void)
{
    test_parser_empty();
    test_parser_empty_key();
    test_parser_empty_value();
    test_parser_spaced();
    test_parser_long_key();
}

static void test_parser_abnormal(void)
{
    lwini_t *ini = NULL;
    char *test_data = NULL;
    test_data = "test";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
    
    test_data = "test[";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "test]";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "test[]";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "\r\n[ test ]\r\n key";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "\r\n[ test ]\r\n key ;=123\r\n";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "\r\n[ test ]\r\n keyhff ie \r\n key =123\r\n";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);

    test_data = "[tes;tsec]\r\nkey=123\r\n";
    ini = lwini_parse(test_data, strlen(test_data));
    TEST_ASSERT_EQU_TRUE(ini == NULL);
}


static void test_parser(void)
{
    test_parser_normal();
    // test abnormal
    test_parser_abnormal();
}

int main(int argc, char *argv[])
{
    test_parser();
    // printf test report
    printf("test count:%d, test passed:%d, %3.2f%%\r\n", test_count, test_passed, test_count == 0? 0 : (float)test_passed / test_count * 100.0f);
    return test_return;
}