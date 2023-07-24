/**
 * @file 
 * lwiniparser.h
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

#ifndef __LWINIPARSER_H__
#define __LWINIPARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LWINI_PARSER_VERSION "1.0.0"

#define LWINI_LOG_PRINTF(fmt, ...) printf("[%s][%d]"fmt"\r\n",__FUNCTION__, __LINE__,##__VA_ARGS__)

typedef enum
{
    LWINI_TYPE_SECTION = 0,
    LWINI_TYPE_KEY
}lwini_type_t;


typedef struct lwini_s
{
    struct lwini_s *next; /*next section*/
    struct lwini_s *child; /*key and values*/
    lwini_type_t type; /*section or key*/
    char *key; /*section or key name*/
    char *value; /*value*/
}lwini_t;


lwini_t *lwini_parse(const char *buffer, uint32_t len);

int lwini_destroy(lwini_t *ini);

lwini_t *lwini_get_section(lwini_t *ini, const char *section);

lwini_t *lwini_get_value(lwini_t *ini, const char *key);

lwini_t *lwini_get_value_by_section(lwini_t *ini, const char *section, const char *key);

uint32_t lwini_get_size(lwini_t *ini);

uint32_t lwini_get_section_num(lwini_t *ini);

uint32_t lwini_get_key_num(lwini_t *section);

lwini_t *lwini_create_section(const char *section);

int  lwini_add_section(lwini_t **ini, lwini_t *section);

int lwini_add_value(lwini_t *section, const char *key, const char *value);

int lwini_add_value_by_section(lwini_t *ini, const char *section, const char *key, const char *value);

int lwini_remove_section(lwini_t *ini, const char *section);

int lwini_remove_value(lwini_t *ini, const char *key);

int lwini_remove_value_by_section(lwini_t *ini, const char *section, const char *key);

int lwini_format(lwini_t *ini, char *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* __LWINIPARSER_H__ */
