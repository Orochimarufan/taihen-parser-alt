/*
 * lexer.c - Tokenisation algorithm for taihen configuration files
 *
 * Copyright (C) 2016 David "Davee" Morgan
 *               2017 Taeyeon Mori
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include <taihen/lexer.h>

#define COMMENT_CHAR '#'
#define SECTION_CHAR '*'
#define SECTION_HALT_CHAR '!'


int taihen_config_init_lexer(taihen_config_lexer *lex, const char *input)
{
    lex->start = lex->end = input;
    lex->token = CONFIG_START_TOKEN;
    return 0;
}

static inline const char *until_eol(const char *end, int comments)
{
    // Search for newline or comment
    while (*end != '\r' && *end != '\n' && (!comments || *end != COMMENT_CHAR))
        end++;

    return end;
}

static inline const char *until_eol_trim(const char *end)
{
    // Find end of line
    end = until_eol(end, 1);

    // Backtrack until we encounter non-whitespace.
    do
        end--;
    while ((*end == ' ' || *end == '\t' || *end == '\v' || *end == '\f'));

    return ++end;
}

taihen_config_lexer_token taihen_config_next_token(taihen_config_lexer *lex)
{
    // Begin where last one ended
    const char *end = lex->end;
    lex->start = end++;

    // Check next character
    switch(*lex->start)
    {
    case COMMENT_CHAR:
        lex->token = CONFIG_COMMENT_TOKEN;
        end = until_eol(end, 0);
        break;
    case SECTION_CHAR:
        lex->token = CONFIG_SECTION_TOKEN;
        break;
    case SECTION_HALT_CHAR:
        lex->token = CONFIG_SECTION_HALT_TOKEN;
        break;
    case '\r':
    case '\n':
        lex->token = CONFIG_NEWLINE_TOKEN;
        while (*end == '\r' || *end == '\n')
            ++end;
        break;
    case ' ':
    case '\t':
    case '\v':
    case '\f':
        lex->token = CONFIG_WHITESPACE_TOKEN;
        while (*end == ' ' || *end == '\t' || *end == '\v' || *end == '\f')
            end++;
        break;
    case '\0':
        lex->token = CONFIG_END_TOKEN;
        break;
    default:
        lex->token = CONFIG_TEXT_TOKEN;
        end = until_eol_trim(end);
        break;
    }
    
    lex->end = end;
    return lex->token;
}

int taihen_config_lex(taihen_config_lexer *lex)
{
    taihen_config_lexer_token last_token = lex->token;
    taihen_config_lexer_token this_token;

    // Discard comments, newlines, whitespace
    do
        this_token = taihen_config_next_token(lex);
    while (this_token == CONFIG_WHITESPACE_TOKEN ||
            this_token == CONFIG_COMMENT_TOKEN ||
            this_token == CONFIG_NEWLINE_TOKEN);

    // Check grammar
    switch (last_token)
    {
    case CONFIG_START_TOKEN:
    case CONFIG_SECTION_NAME_TOKEN:
    case CONFIG_PATH_TOKEN:
        switch (this_token)
        {
        case CONFIG_TEXT_TOKEN:
            lex->token = CONFIG_PATH_TOKEN;
        case CONFIG_SECTION_TOKEN:
            return 1;
        case CONFIG_END_TOKEN:
            return 0;
        default:
            return -1;
        }
    case CONFIG_SECTION_TOKEN:
        switch (this_token)
        {
        case CONFIG_TEXT_TOKEN:
            lex->token = CONFIG_SECTION_NAME_TOKEN;
        case CONFIG_SECTION_HALT_TOKEN:
            return 1;
        default:
            return -1;
        }
    case CONFIG_SECTION_HALT_TOKEN:
        switch (this_token)
        {
        case CONFIG_TEXT_TOKEN:
            lex->token = CONFIG_SECTION_NAME_TOKEN;
            return 1;
        default:
            return -1;
        }
    default:
        return -1;
    }
}
