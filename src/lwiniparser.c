/**
 * @file 
 * lwiniparser.c
 * 
 * @brief 
 * 
 * @author hacper (git@hacperme.com)
 * @date 23/07/2023
 * 
 * =================================================================================
 * Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.  All Rights Reserved.
 * Quectel Wireless Solution Proprietary and Confidential.
 * =================================================================================
 * 
 * =================================================================================
 * 		EDIT HISTORY FOR MODULE
 * This section contains comments describing changes made to the module.
 * WHEN		WHO				WHAT, WHERE, WHY
 * ------------		-------		--------------------------------------------------
 * 23/07/2023		hacper		create
 * =================================================================================
 */

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "lwiniparser.h"

typedef enum
{
    LWINI_PARSE_TYPE_UNKNOWN = 0,
    LWINI_PARSE_TYPE_COMMENT,
    LWINI_PARSE_TYPE_EMPTY,
    LWINI_PARSE_TYPE_SECTION,
    LWINI_PARSE_TYPE_KEY
} lwini_parse_type_t;

static int lwini_read_line(const char *buffer, uint32_t len, char **line)
{
    int ret = -1;
    char *p = NULL;
    uint32_t line_len = 0;
    char *tmp_line = NULL;

    if (buffer == NULL || len == 0 || line == NULL)
    {
        LWINI_LOG_PRINTF("buffer is null or len is 0\n");
        goto exit;
    }

    p = strchr(buffer, '\n');
    if (p == NULL)
    {
        
        if(buffer[len] == '\0')
        {
            line_len = len;
        }
        else
        {
            LWINI_LOG_PRINTF("not found \\n\n");
            goto exit;
        }
        
    }
    else
    {
        p++;
        // todo 处理续行符 多行的情况
        line_len = p - buffer;
    }

    if(line_len == 0)
    {
        *line = NULL;
        ret = -1;
        goto exit;
    }

    tmp_line = (char *)malloc(line_len + 1);
    if (tmp_line == NULL)
    {
        LWINI_LOG_PRINTF("malloc error\n");
        goto exit;
    }

    memcpy(tmp_line, buffer, line_len);
    tmp_line[line_len] = '\0';

    *line = tmp_line;

    ret = line_len;

exit:
    return ret;
}

static int lwini_strip_line(char *line)
{
    char *s_start = NULL;
    char *s_end = NULL;
    
    int line_len = 0;

    if(line == NULL)
    {
        LWINI_LOG_PRINTF("line is null\n");
        return -1;
    }

    // 去除文本前面和后面的空格
    line_len = strlen(line);
    s_start = line;
    s_end = line + line_len;

    while (isspace((int)*s_start) && s_start < s_end)
    {
        s_start++;
    }

    while (s_end > s_start)
    {
        if (isspace((int)*(s_end - 1)))
        {
            s_end--;
        }
        else
        {
            break;
        }
    }
    *s_end = '\0';

    memmove(line, s_start, s_end - s_start + 1);

    return 0;
}

static int lwini_parse_line(const char *line, lwini_parse_type_t *type, char **section, char **key, char **value)
{
    int ret = 0;
    int line_len = 0;
    char *new_line = NULL;
    char *pos = NULL;

    if (line == NULL || type == NULL || section == NULL || key == NULL || value == NULL)
    {
        LWINI_LOG_PRINTF("line is null or type is null or section is null or key is null or value is null\n");
        ret = -1;
        goto exit;
    }

    *section = NULL;
    *key = NULL;
    *value = NULL;

    new_line = strdup(line);
    if (new_line == NULL)
    {
        LWINI_LOG_PRINTF("strdup error\n");
        ret = -1;
        goto exit;
    }

    lwini_strip_line(new_line);

    // remwoe comment
    pos = strchr(new_line, ';');
    if (pos != NULL)
    {
        *pos = '\0';
    }

    pos = strchr(new_line, '#');
    if (pos != NULL)
    {
        *pos = '\0';
    }


    line_len = strlen(new_line);
    if (line_len == 0 || new_line[0] == '\r' || new_line[0] == '\n')
    {
        *type = LWINI_PARSE_TYPE_EMPTY;
        goto exit;
    }

    if (new_line[0] == '#' || new_line[0] == ';')
    {
        *type = LWINI_PARSE_TYPE_COMMENT;
        goto exit;
    }

    if (new_line[0] == '[')
    {
        pos = strchr(new_line, ']');
        if (pos == NULL)
        {
            LWINI_LOG_PRINTF("not found ]\n");
            ret = -1;
            goto exit;
        }
        *pos = '\0';
        *section = strdup(new_line + 1);
        lwini_strip_line(*section);
        *type = LWINI_PARSE_TYPE_SECTION;
        goto exit;
    }

    pos = strchr(new_line, '=');
    if (pos == NULL)
    {
        LWINI_LOG_PRINTF("not found =\n");
        ret = -1;
        goto exit;
    }
    *pos = '\0';

    if (*(pos + 1) == '\0' || (*(pos + 1) == '\r' || *(pos + 1) == '\n') || *(pos + 1) == '#' || *(pos + 1) == ';')
    {
        *key = strdup(new_line);
        lwini_strip_line(*key);
        *value = strdup("");
        *type = LWINI_PARSE_TYPE_KEY;
        goto exit;
    }
    else
    {
        *key = strdup(new_line);
        lwini_strip_line(*key);
        pos = pos + 1;
        char *tmp_s = pos;
        while (*(pos) != '\0')
        {
            // 去除行尾的结束符或注释
            if (*(pos) == '\r' || *(pos) == '\n' || *(pos) == ';' || *(pos) == '#')
            {
                *pos = '\0';
                break;
            }
            pos++;
        }
        *value = strdup(tmp_s);
        lwini_strip_line(*value);
        *type = LWINI_PARSE_TYPE_KEY;
        goto exit;
    }

    ret = 0;

exit:
    if (new_line != NULL)
    {
        free(new_line);
    }
    return ret;
}

lwini_t *lwini_parse(const char *buffer, uint32_t len)
{
    lwini_t *ini = NULL;
    lwini_t *tmp_section = NULL;
    char *line = NULL;
    uint32_t read_len = 0;
    int ret = -1;

    lwini_parse_type_t type = LWINI_PARSE_TYPE_UNKNOWN;
    char *section = NULL;
    char *key = NULL;
    char *value = NULL;
    char *last_section = NULL;

    if (buffer == NULL || len == 0)
    {
        LWINI_LOG_PRINTF("buffer is null or len is 0\n");
        goto exit;
    }

    while (read_len < len)
    {
        ret = lwini_read_line(buffer + read_len, len - read_len, &line);
        if (ret < 0)
        {
            LWINI_LOG_PRINTF("read line error\n");
            goto parse_err;
        }
        read_len += ret;
        // read a line

        // parse a line
        type = LWINI_PARSE_TYPE_UNKNOWN;
        if (lwini_parse_line(line, &type, &section, &key, &value) != 0)
        {
            LWINI_LOG_PRINTF("parse line error\n");
            goto parse_err;
        }

        switch (type)
        {
        case LWINI_PARSE_TYPE_COMMENT:
        case LWINI_PARSE_TYPE_EMPTY:
            break;
        case LWINI_PARSE_TYPE_SECTION:
            tmp_section = lwini_create_section(section);
            lwini_add_section(&ini, tmp_section);
            if (last_section)
            {
                free(last_section);
            }
            last_section = strdup(section);
            free(section);
            section = NULL;
            break;
        case LWINI_PARSE_TYPE_KEY:
            if (last_section == NULL)
            {
                LWINI_LOG_PRINTF("key must be in section\n");
                goto parse_err;
            }
            lwini_add_value_by_section(ini, last_section, key, value);
            if (key)
            {
                free(key);
                key = NULL;
            }

            if(value)
            {
                free(value);
                value = NULL;
            }
            break;
        default:
        parse_err:
            if (section) free(section);
            
            if (key) free(key);
            
            if (value) free(value);
            
            if(ini) lwini_destroy(ini), ini = NULL;
            
            if(line) free(line), line = NULL;
            
            goto exit;
        }

        free(line);
        line = NULL;
    }

exit:
    if (last_section)
    {
        free(last_section);
    }
    return ini;
}

int lwini_destroy(lwini_t *ini)
{
    int ret = 0;
    lwini_t *section = NULL;
    lwini_t *value = NULL;
    lwini_t *tmp = NULL;

    if (ini == NULL)
    {
        LWINI_LOG_PRINTF("ini is null\n");
        goto exit;
    }

    section = ini;

    while (section != NULL)
    {
        value = section->child;
        while (value != NULL)
        {
            if (value->key)
            {
                free(value->key);
            }
            if (value->value)
            {
                free(value->value);
            }
            tmp = value;
            value = value->child;
            free(tmp);
        }
        if (section->key)
        {
            free(section->key);
        }
        if (section->value)
        {
            free(section->value);
        }
        tmp = section;
        section = section->next;
        free(tmp);
    }

exit:
    return ret;
}

lwini_t *lwini_get_section(lwini_t *ini, const char *section)
{
    lwini_t *ret = NULL;
    lwini_t *tmp = NULL;

    if (ini == NULL || section == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null\n");
        goto exit;
    }

    tmp = ini;
    while (tmp != NULL)
    {
        if (tmp->type == LWINI_TYPE_SECTION && strcmp(tmp->key, section) == 0)
        {
            ret = tmp;
            break;
        }
        tmp = tmp->next;
    }

exit:

    return ret;
}

lwini_t *lwini_get_value(lwini_t *ini, const char *key)
{
    lwini_t *ret = NULL;
    lwini_t *tmp = NULL;

    if (ini == NULL || key == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or key is null\n");
        goto exit;
    }

    if (ini->type != LWINI_TYPE_SECTION)
    {
        goto exit;
    }

    tmp = ini->child;

    while (tmp != NULL)
    {
        if (tmp->type == LWINI_TYPE_KEY && strcmp(tmp->key, key) == 0)
        {
            ret = tmp;
            break;
        }
        tmp = tmp->child;
    }

exit:

    return ret;
}

lwini_t *lwini_get_value_by_section(lwini_t *ini, const char *section, const char *key)
{
    lwini_t *ret = NULL;
    lwini_t *tmp = NULL;

    if (ini == NULL || section == NULL || key == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null or key is null\n");
        goto exit;
    }

    tmp = lwini_get_section(ini, section);
    if (tmp == NULL)
    {
        goto exit;
    }

    ret = lwini_get_value(tmp, key);

exit:

    return ret;
}

uint32_t lwini_get_size(lwini_t *ini)
{
    uint32_t ret = 0;
    lwini_t *section = NULL;
    lwini_t *value = NULL;

    if (ini == NULL)
    {
        LWINI_LOG_PRINTF("ini is null\n");
        goto exit;
    }

    section = ini;

    while (section != NULL)
    {
        ret += 1 + 1 + strlen(section->key) + 1 + 1; // [section]\r\n
        value = section->child;
        while (value != NULL)
        {
            ret += strlen(value->key) + 1; // key=value\r\n
            if (value->value)
            {
                ret += strlen(value->value) + 1 + 1;
            }
            else
            {
                ret += 1 + 1 + 1;
            }

            value = value->child;
        }
        section = section->next;
    }

    ret += 1; // \0

exit:

    return ret;
}

uint32_t lwini_get_section_num(lwini_t *ini)
{
    uint32_t ret = 0;
    lwini_t *section = NULL;

    if (ini == NULL)
    {
        LWINI_LOG_PRINTF("ini is null\n");
        goto exit;
    }

    section = ini;

    while (section != NULL)
    {
        if (section->type == LWINI_TYPE_SECTION)
        {
            ret++;
        }
        section = section->next;
    }

exit:

    return ret;
}

uint32_t lwini_get_key_num(lwini_t *section)
{
    uint32_t ret = 0;
    lwini_t *value = NULL;

    if (section == NULL)
    {
        LWINI_LOG_PRINTF("section is null\n");
        goto exit;
    }

    value = section->child;

    while (value != NULL)
    {
        if (value->type == LWINI_TYPE_KEY)
        {
            ret++;
        }
        value = value->child;
    }

exit:

    return ret;
}

lwini_t *lwini_create_section(const char *section)
{
    lwini_t *ret = NULL;

    if (section == NULL)
    {
        LWINI_LOG_PRINTF("section is null\n");
        goto exit;
    }

    ret = (lwini_t *)malloc(sizeof(lwini_t));
    if (ret == NULL)
    {
        LWINI_LOG_PRINTF("malloc error\n");
        goto exit;
    }

    memset(ret, 0, sizeof(lwini_t));

    ret->type = LWINI_TYPE_SECTION;
    ret->key = strdup(section);

exit:

    return ret;
}

int lwini_add_section(lwini_t **ini, lwini_t *section)
{
    int ret = -1;
    lwini_t *tmp = NULL;

    if (ini == NULL || section == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null\n");
        goto exit;
    }

    if (*ini == NULL)
    {
        *ini = section;
        ret = 0;
        goto exit;
    }

    tmp = *ini;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }

    tmp->next = section;

    ret = 0;

exit:

    return ret;
}

int lwini_add_value(lwini_t *section, const char *key, const char *value)
{
    int ret = -1;
    lwini_t *tmp = NULL;
    lwini_t *new_value = NULL;

    if (section == NULL || key == NULL || value == NULL)
    {
        LWINI_LOG_PRINTF("section is null or key is null or value is null\n");
        goto exit;
    }

    if (section->type != LWINI_TYPE_SECTION)
    {
        LWINI_LOG_PRINTF("section type is not LWINI_TYPE_SECTION\n");
        goto exit;
    }

    new_value = (lwini_t *)malloc(sizeof(lwini_t));
    if (new_value == NULL)
    {
        LWINI_LOG_PRINTF("malloc error\n");
        goto exit;
    }

    memset(new_value, 0, sizeof(lwini_t));

    new_value->type = LWINI_TYPE_KEY;
    new_value->key = strdup(key);
    new_value->value = strdup(value);

    if (section->child == NULL)
    {
        section->child = new_value;
    }
    else
    {
        tmp = section->child;
        while (tmp->child != NULL)
        {
            if (tmp->type == LWINI_TYPE_KEY && strcmp(tmp->key, key) == 0)
            {
                if (tmp->value)
                {
                    free(tmp->value);
                }
                tmp->value = strdup(value);
                free(new_value->key);
                free(new_value->value);
                free(new_value);
                ret = 0;
                goto exit;
            }
            tmp = tmp->child;
        }
        tmp->child = new_value;
    }

    ret = 0;

exit:

    return ret;
}

int lwini_add_value_by_section(lwini_t *ini, const char *section, const char *key, const char *value)
{
    int ret = -1;
    lwini_t *tmp = NULL;

    if (ini == NULL || section == NULL || key == NULL || value == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null or key is null or value is null\n");
        goto exit;
    }

    tmp = lwini_get_section(ini, section);
    if (tmp == NULL)
    {
        tmp = lwini_create_section(section);
        lwini_add_section(&ini, tmp);
    }

    ret = lwini_add_value(tmp, key, value);

exit:

    return ret;
}

int lwini_remove_section(lwini_t *ini, const char *section)
{
    int ret = -1;
    lwini_t *tmp = NULL;
    lwini_t *prev = NULL;

    if (ini == NULL || section == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null\n");
        goto exit;
    }

    tmp = ini;
    while (tmp != NULL)
    {
        if (tmp->type == LWINI_TYPE_SECTION && strcmp(tmp->key, section) == 0)
        {
            if (prev == NULL)
            {
                ini = tmp->next;
            }
            else
            {
                prev->next = tmp->next;
            }
            lwini_destroy(tmp);
            ret = 0;
            goto exit;
        }
        prev = tmp;
        tmp = tmp->next;
    }

exit:

    return ret;
}

int lwini_remove_value(lwini_t *section, const char *key)
{
    int ret = -1;
    lwini_t *tmp = NULL;
    lwini_t *prev = NULL;

    if (section == NULL || key == NULL)
    {
        LWINI_LOG_PRINTF("section is null or key is null\n");
        goto exit;
    }

    if (section->type != LWINI_TYPE_SECTION)
    {
        LWINI_LOG_PRINTF("section type is not LWINI_TYPE_SECTION\n");
        goto exit;
    }

    tmp = section->child;
    while (tmp != NULL)
    {
        if (tmp->type == LWINI_TYPE_KEY && strcmp(tmp->key, key) == 0)
        {
            if (prev == NULL)
            {
                section->child = tmp->child;
            }
            else
            {
                prev->child = tmp->child;
            }
            if (tmp->key)
            {
                free(tmp->key);
            }
            if (tmp->value)
            {
                free(tmp->value);
            }
            free(tmp);
            ret = 0;
            goto exit;
        }
        prev = tmp;
        tmp = tmp->child;
    }

exit:

    return ret;
}

int lwini_remove_value_by_section(lwini_t *ini, const char *section, const char *key)
{
    int ret = -1;
    lwini_t *tmp = NULL;

    if (ini == NULL || section == NULL || key == NULL)
    {
        LWINI_LOG_PRINTF("ini is null or section is null or key is null\n");
        goto exit;
    }

    tmp = lwini_get_section(ini, section);
    if (tmp == NULL)
    {
        LWINI_LOG_PRINTF("section is not found\n");
        goto exit;
    }

    ret = lwini_remove_value(tmp, key);

exit:

    return ret;
}

int lwini_format(lwini_t *ini, char *buffer, uint32_t len)
{
    int ret = -1;
    lwini_t *section = NULL;
    lwini_t *value = NULL;
    uint32_t size = 0;

    if (ini == NULL || buffer == NULL || len == 0)
    {
        LWINI_LOG_PRINTF("ini is null or buffer is null or len is 0\n");
        goto exit;
    }

    if (len < lwini_get_size(ini))
    {
        LWINI_LOG_PRINTF("buffer is too small\n");
        goto exit;
    }

    section = ini;

    while (section != NULL)
    {
        size += snprintf(buffer + size, len - size, "[%s]\r\n", section->key);
        value = section->child;
        while (value != NULL)
        {
            size += snprintf(buffer + size, len - size, "%s=%s\r\n", value->key, value->value == NULL ? "" : value->value);
            value = value->child;
        }
        section = section->next;
    }

    ret = 0;

exit:

    return ret;
}

#ifdef __cplusplus
}
#endif
