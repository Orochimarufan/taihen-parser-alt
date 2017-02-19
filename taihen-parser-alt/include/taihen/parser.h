/*
 * parser.c - Parser API for taihen configuration files
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *               2017 Taeyeon Mori
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define TAIHEN_CONFIG_MAX_PATH_SIZE 256

#define TAIHEN_CONFIG_ALL_SECTION "ALL"
#define TAIHEN_CONFIG_ALL_SECTION_LEN 3
#define TAIHEN_CONFIG_KERNEL_SECTION "KERNEL"
#define TAIHEN_CONFIG_KERNEL_SECTION_LEN 6


typedef void (* taihen_config_handler)(const char *module, void *param);

/*!
    \brief Check whether a configuration is valid syntax.
    taihen_config_validate is used to check whether a provided configuration is valid syntax.
    This is useful when used before taihen_config_parse to provide error checking before stream based
    parsing.
    \param input A UTF-8 encoded null-terminated string containing the configuration to check.
    \return non-zero on valid configuration, else zero on invalid.
    \sa taihen_config_parse
 */
int taihen_config_validate(const char *input);

/*!
    \brief taihen_config_parse parses a configuration for contextualised paths.
    taihen_config_parse is used to obtain an ordered stream of the paths appropriate for the section provided.
    Special sections such as ALL and KERNEL will be taken into consideration when generating the stream.
    taihen_config_parse provides no error checking or handling. Use taihen_config_validate before parsing the
    document to avoid errors in parsing.
   \param input     A UTF-8 encoded null-terminated string containing the configuration to parse.
   \param section   A UTF-8 encoded null-terminated string containing the section to base context from.
   \param handler   A taihen_config_handler to receive the stream of paths.
   \param param     A user provided value that is passed to the provided taihen_config_handler.
   \sa taihen_config_validate
 */
void taihen_config_parse(const char *input, const char *section, taihen_config_handler handler, void *param);


#ifdef __cplusplus
}
#endif