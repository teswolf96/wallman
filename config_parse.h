#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum NAME{ACTIVE_PROFILE, CATEGORY, TITLE, HIDDEN, PATHLIST , STR} Token_Enum;

static inline char *stringFromToken(enum NAME f)
{
    static const char *strings[] = { "ACTIVE_PROFILE", "CATEGORY", "TITLE", "HIDDEN","PATHLIST", "STR"};

    return strings[f];
}

struct Token{
    Token_Enum TOKEN_NAME;
    char* TOKEN_VAL;
};

int load_profiles_new();

struct Token get_token(char* str);

int is_valid_str_char(char val);

char *trimwhitespace(char *str);
