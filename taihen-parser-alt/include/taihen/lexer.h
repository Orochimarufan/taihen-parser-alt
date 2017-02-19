/*
 * lexer.h - Lexer API for taihen configuration files
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

typedef enum
{
    CONFIG_START_TOKEN,
    CONFIG_END_TOKEN,
    CONFIG_COMMENT_TOKEN, ///< taihen_config_next_token only
    CONFIG_SECTION_TOKEN,
    CONFIG_SECTION_HALT_TOKEN,
    CONFIG_SECTION_NAME_TOKEN, ///< taihen_config_lex only
    CONFIG_PATH_TOKEN, ///< taihen_config_lex only
    CONFIG_TEXT_TOKEN, ///< taihen_config_next_token only
    CONFIG_NEWLINE_TOKEN, ///< taihen_config_next_token only
    CONFIG_WHITESPACE_TOKEN ///< taihen_config_next_token only
} taihen_config_lexer_token;

typedef struct
{
    taihen_config_lexer_token token;
    const char *start;
    const char *end;
} taihen_config_lexer;

/*!
    \brief Initialise or reset lexer context.
    taihen_config_init_lexer will init/reset the provided taihen_config_lexer and assign the
    provided input to the context.
    \param ctx      A non-null pointer to a context to initialise or reset.
    \param input    A non-null UTF-8 encoded null-terminated string to tokenise.
    \return zero on success, < 0 on error.
 */
int taihen_config_init_lexer(taihen_config_lexer *ctx, const char *input);

/*!
    \brief Retrieve the next raw lexer token
    taihen_config_next_token will give you the next raw token on the stream. It doesn't
    do any validation whatsoever. The source file (including Whitespace and comments)
    can be perfectly reproduced from this token stream. This function will never fail,
    but sensible output depends on sensible input.
    \note Some token types are only available in this function's token stream
    \param ctx A non-null pointer to an initialised lexer context
    \return The next token type (also available as ctx->token) for quick access
    \sa taihen_config_init_lexer
    \sa taihen_config_lex
 */
taihen_config_lexer_token taihen_config_next_token(taihen_config_lexer *ctx);

/*!
    \brief Retrieve the next lexer token.
    taihen_config_lex provides a more processed token stream, with whitespace and comments
    eliminated. It also runs a very basic grammar check on the token transitions, as well
    as adding context to CONFIG_TEXT_TOKENs by turning them into CONFIG_SECTION_NAME_TOKENs 
    and CONFIG_PATH_TOKENs.
    \param ctx  A non-null pointer to an initialised context.
    \return 0 if there are no further tokens, > 0 if there are further tokens else < 0 on error.
    \sa taihen_config_init_lexer
    \sa taihen_config_next_token
 */
int taihen_config_lex(taihen_config_lexer *ctx);

#ifdef __cplusplus
}
#endif
