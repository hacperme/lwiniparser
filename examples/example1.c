#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lwiniparser.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

	const char *test_data = "; ini test data\r\n" \
	"[section1]\r\n" \
	"key1=value1\r\n" \
	"key2=value2\r\n" \
	"\r\n" \
	"[ section2 ]\r\n" \
	"key1=value1\r\n" \
	"  key2=value2\r\n" \
	"key3=346;123vale\r\n" \
	"[section3]\r\n" \
	"[section4]\r\n" \
	"key1=\r\n" \
	"\r\n" \
	"\r\n" \
	"[colors]  \r\n" \
	"foreground = black\r\n" \
	"background = white\r\n" \
	"highlight = yellow\r\n" ;

	
	lwini_t *ini = NULL;
	lwini_t *section = NULL;
	lwini_t *key = NULL;

	ini = lwini_parse(test_data, strlen(test_data));

	if(ini == NULL)
	{
		LWINI_LOG_PRINTF("ini parse failed");
		return -1;
	}

	// section nums
	LWINI_LOG_PRINTF("section nums:%d", lwini_get_section_num(ini));

	section = lwini_get_section(ini, "section1");
	if(section)
	{
		LWINI_LOG_PRINTF("section1");
		key = lwini_get_value(section, "key1");
		if(key)
		{
			LWINI_LOG_PRINTF("key1:%s", key->value);
		}
		key = lwini_get_value(section, "key2");
		if(key)
		{
			LWINI_LOG_PRINTF("key2:%s", key->value);
		}
	}

	section = lwini_get_section(ini, "section2");
	if(section)
	{
		LWINI_LOG_PRINTF("section2");
		key = lwini_get_value(section, "key1");
		if(key)
		{
			LWINI_LOG_PRINTF("key1:%s", key->value);
		}
		key = lwini_get_value(section, "key2");
		if(key)
		{
			LWINI_LOG_PRINTF("key2:%s", key->value);
		}
		key = lwini_get_value(section, "key3");
		if(key)
		{
			LWINI_LOG_PRINTF("key3:%s", key->value);
		}
	}

	section = lwini_get_section(ini, "section3");
	if(section)
	{
		LWINI_LOG_PRINTF("section3");
		key = lwini_get_value(section, "key1");
		if(key)
		{
			LWINI_LOG_PRINTF("key1:%s", key->value);
		}
	}

	section = lwini_get_section(ini, "section4");
	if(section)
	{
		LWINI_LOG_PRINTF("section4");
		key = lwini_get_value(section, "key1");
		if(key)
		{
			LWINI_LOG_PRINTF("key1:%s", key->value);
		}
	}

	section = lwini_get_section(ini, "colors");
	if(section)
	{
		LWINI_LOG_PRINTF("colors");
		// value nums
		LWINI_LOG_PRINTF("value nums:%d", lwini_get_key_num(section));
		key = lwini_get_value(section, "foreground");
		if(key)
		{
			LWINI_LOG_PRINTF("foreground:%s", key->value);
		}
		key = lwini_get_value(section, "background");
		if(key)
		{
			LWINI_LOG_PRINTF("background:%s", key->value);
		}
		key = lwini_get_value(section, "highlight");
		if(key)
		{
			LWINI_LOG_PRINTF("highlight:%s", key->value);
		}
	}

	section = lwini_create_section("section test");
	// add section test
	lwini_add_section(&ini, section);
	// add vaue test
	lwini_add_value(section, "tettt", "12334");
	// add vaue test
	lwini_add_value(section, "tettt345", "12334");

	// size of ini
	int format_size = lwini_get_size(ini);

	char *tmpini = malloc(format_size);
	memset(tmpini, 0, format_size);
	lwini_format(ini, tmpini, format_size);
	LWINI_LOG_PRINTF("format ini:%s", tmpini);

	lwini_destroy(ini);
    free(tmpini);

    return 0;
}