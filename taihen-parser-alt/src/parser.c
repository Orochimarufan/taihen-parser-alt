/*
 * parser.c - parser algorithm for taihen configuration files
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *               2017 Taeyeon Mori
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <taihen/lexer.h>
#include <taihen/parser.h>

#ifndef NO_STRING
#include <string.h>
#endif


// Helpers
#ifdef NO_STRING
static inline int memcmp(const char *a, const char *b, unsigned size)
{
    while (size--) {
        if (a[size] != b[size])
            return 1;
    }
    return 0;
}

static inline unsigned strlen(const char *a)
{
    int len = 0;
    while (*a[len])
        ++len;
    return len;
}

static inline void memcpy(void *dest, const void *src, unsigned len)
{
    while(len--)
        dst[len] = src[len];
}
#endif

static inline int strings_equal(const char *a, unsigned a_len, const char *b, unsigned b_len)
{
    return (a_len == b_len) && !memcmp(a, b, a_len);
}


// Parser
void taihen_config_parse(const char *input, const char *section, taihen_config_handler handler, void *param)
{
    taihen_config_lexer ctx;
    taihen_config_init_lexer(&ctx, input);

    int halt_flag = 0;
    int match = 0;

    int section_len = strlen(section);
    int is_kernel = strings_equal(section, section_len, 
                                  TAIHEN_CONFIG_KERNEL_SECTION,
                                  TAIHEN_CONFIG_KERNEL_SECTION_LEN);

    char path[TAIHEN_CONFIG_MAX_PATH_SIZE + 1];
    int len;

    while (taihen_config_lex(&ctx) > 0)
    {
        switch (ctx.token)
        {
        case CONFIG_SECTION_TOKEN:
            // Check if last section makes us stop
            if (match && halt_flag)
                return;
            // Reset flags for new section
            match = halt_flag = 0;
            break;
        case CONFIG_SECTION_HALT_TOKEN:
            // Set halt flag
            halt_flag = 1;
            break;
        case CONFIG_SECTION_NAME_TOKEN:
            // Check if section matches
            // Unless this is matching kernel modules,
            // also include entries in ALL
            len = ctx.end - ctx.start;
            match = (!is_kernel && strings_equal(ctx.start, len,
                            TAIHEN_CONFIG_ALL_SECTION,
                            TAIHEN_CONFIG_ALL_SECTION_LEN)) ||
                    strings_equal(ctx.start, len, section, section_len);
            break;
        case CONFIG_PATH_TOKEN:
            // Only matching
            if (!match)
                break;
            // Check size, null-terminate the path in a buffer and
            // fire the callback
            len = ctx.end - ctx.start;
            if (len <= TAIHEN_CONFIG_MAX_PATH_SIZE)
            {
                memcpy(path, ctx.start, len);
                path[len] = '\0';
                handler(path, param);
            }
            // We have no way of reporting an error,
            // so we just ignore the module.
            break;
        // All other token types shouldn't survive _lex()
        default:
            break;
        }
    }
}


// Validator
// UTF-8 validation 1:1 from original implementation
static inline int is_continuation_byte(char b)
{
    return ((b & 0xC0) == 0x80);
}

static inline int check_continuation_bytes(const char *start, const char *end, int len)
{
    if ((end - start) < len)
    {
        return 0;
    }

    for (int i = 0; i < len; ++i)
    {
        if (!is_continuation_byte(start[i]))
        {
            return 0;
        }
    }

    return 1;
}

static int check_utf8_sequence(const char *str, const char *end, unsigned char mask, unsigned char lead, int cont_len)
{
    if ((*str & mask) == lead)
    {
        if (!check_continuation_bytes(str+1, end, cont_len))
        {
            return -1;
        }

        return 1;
    }

    return 0;
}

static int check_utf8(const char *str, const char *end)
{
    struct
    {
        unsigned char mask;
        unsigned char lead;
        unsigned char cont_len;
    } utf8_lut[4] =
    {
        { 0x80, 0x00, 0 }, // U+0000 -> U+007F, 0xxxxxx
        { 0xE0, 0xC0, 1 }, // U+0080 -> U+07FF, 110xxxxx
        { 0xF0, 0xE0, 2 }, // U+0800 -> U+FFFF, 1110xxxx
        { 0xF8, 0xF0, 3 }, // U+10000 -> U+10FFFF, 11110xxx
    };

    while (str < end)
    {
        int i = 0;

        for (i = 0; i < 4; ++i)
        {
            int res = check_utf8_sequence(str, end, utf8_lut[i].mask, utf8_lut[i].lead, utf8_lut[i].cont_len);

            // check if valid sequence but incorrect contiunation
            if (res < 0)
            {
                return 0;
            }

            // check if valid sequence
            if (res > 0)
            {
                str += utf8_lut[i].cont_len+1;
                break;
            }
        }

        // check if we had no valid sequences
        if (i == 4)
        {
            return 0;
        }
    }

    return 1;
}

int taihen_config_validate(const char *input)
{
    taihen_config_lexer ctx;
    taihen_config_init_lexer(&ctx, input);

    int have_section = 0;
    int lex_result = 0;

    while ((lex_result = taihen_config_lex(&ctx)) > 0)
    {
        switch (ctx.token)
        {
        case CONFIG_PATH_TOKEN:
            // paths must belong to a section
            if (!have_section)
                return 0;

            // Check size constraint
            if (ctx.end - ctx.start > TAIHEN_CONFIG_MAX_PATH_SIZE)
                return 0;

        case CONFIG_SECTION_NAME_TOKEN:
            // ensure we actually have a string
            if (ctx.end == ctx.start)
                return 0;

            // validate it is UTF-8
            if (!check_utf8(ctx.start, ctx.end))
                return 0;

            have_section = 1; // Only reachable from PATH after check already passed
            break;

        // ignore these, nothing to check
        case CONFIG_SECTION_HALT_TOKEN:
        //case CONFIG_COMMENT_TOKEN: // Already eliminated by _lex()
        case CONFIG_SECTION_TOKEN:
        case CONFIG_END_TOKEN:
            break;

        // unexpected tokens, invalid document
        default:
            return 0;
        }
    }

    return (lex_result == 0);
}
