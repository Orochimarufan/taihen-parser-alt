#include <stdio.h>
#include <taihen/lexer.h>
#include <taihen/parser.h>
#include "data.h"

void puts_handler(const char *module, void *x)
{
    printf("\t%s\n", module);
}

const char *token_names[10] = {
    "Start Token",
    "End Token",
    "Comment", ///< taihen_config_next_token only
    "Section Start",
    "Section Halt Flag",
    "Section Name", ///< taihen_config_lex only
    "Module Path", ///< taihen_config_lex only
    "Text", ///< taihen_config_next_token only
    "Newline", ///< taihen_config_next_token only
    "Whitespace" ///< taihen_config_next_token only
};

int main(int argc, char **argv)
{
    puts("Tokenisation ---------------------------------------------------------");
    {
        taihen_config_lexer ctx;
        taihen_config_init_lexer(&ctx, data);
        while (taihen_config_lex(&ctx) > 0)
        {
#ifdef TAIHEN_PARSER_ALT // Token content is accessed differently
            printf("%-17s: '%.*s'\n", token_names[ctx.token], ctx.end - ctx.start, ctx.start);
#else
            printf("%-17s: '%s'\n", token_names[ctx.token], ctx.line_pos);
#endif
        }
    }

    puts("Validation -----------------------------------------------------------");
    printf("Test data validates %s\n", taihen_config_validate(data) ? "OK" : "FAIL");

    puts("KERNEL Parsing -------------------------------------------------------");
    taihen_config_parse(data, "KERNEL", puts_handler, 0);

    puts("User Parsing ---------------------------------------------------------");
    taihen_config_parse(data, "main", puts_handler, 0);

    puts("ALL Parsing ----------------------------------------------------------");
    taihen_config_parse(data, "NonExistant", puts_handler, 0);

    puts("Halt Parsing ---------------------------------------------------------");
    taihen_config_parse(data, "NPXS10015", puts_handler, 0);

    puts("See data.h for source data -------------------------------------------");

    return 0;
}
