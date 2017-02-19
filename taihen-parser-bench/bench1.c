#include "data.h"
#include <taihen/lexer.h>
#include <taihen/parser.h>
#include <time.h>
#include <stdio.h>

void handle(const char *module, void *x)
{
}

int main(int argc, char **argv)
{
    clock_t t, te;
    taihen_config_lexer ctx;

    // Basic Lexer
#ifdef TAIHEN_PARSER_ALT
    t = clock();
    
    for (int i = 100000; --i;)
    {
        taihen_config_init_lexer(&ctx, data);
        while(taihen_config_next_token(&ctx) != CONFIG_END_TOKEN);
    }
    
    te = clock();
    
    printf("LexerA    x100000: %6ld\n", te - t);
#endif

    // Lexer
    t = clock();

    for (int i = 100000; --i;)
    {
        taihen_config_init_lexer(&ctx, data);
        while(taihen_config_lex(&ctx) > 0);
    }
    
    te = clock();
    
    printf("Lexer     x100000: %6ld\n", te - t);
    
    // Kernel Parser
    t = clock();
    
    for (int i = 100000; --i;)
    {
        taihen_config_parse(data, "KERNEL", &handle, 0);
    }
    
    te = clock();
    
    printf("ParserK   x100000: %6ld\n", te - t);
    
    // User Parser
    t = clock();
    
    for (int i = 100000; --i;)
    {
        taihen_config_parse(data, "NPXS10031", &handle, 0);
    }
    
    te = clock();
    
    printf("ParserU   x100000: %6ld\n", te - t);
    
    // User Parser w/ halt
    t = clock();
    
    for (int i = 100000; --i;)
    {
        taihen_config_parse(data, "NPXS10015", &handle, 0);
    }
    
    te = clock();
    
    printf("ParserH   x100000: %6ld\n", te - t);
    
    // Validate
    t = clock();
    
    for (int i = 100000; --i;)
    {
        taihen_config_validate(data);
    }
    
    te = clock();
    
    printf("Validate  x100000: %6ld\n", te - t);
    
    return 0;
}