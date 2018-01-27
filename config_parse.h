#ifndef CONFIG_PARSE
#define CONFIG_PARSE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vector.h"



typedef enum NAME{ACTIVE_PROFILE, CATEGORY, TITLE, HIDDEN, PATHLIST, PROFILE, COMMENT, STR} Token_Enum;
static inline char *stringFromToken(enum NAME f)
{
    static const char *strings[] = { "Active Profile", "Category", "Title", "Hidden","Paths", "Profile", "#", "STR"};

    return strings[f];
}

struct Token{
    Token_Enum TOKEN_NAME;
    char TOKEN_VAL[256];
};

extern struct Token* main_config_tokens;

struct wallpaper {
    char name[256]; //Profile name
    char category[256]; //Category name
    char disp_name[256];
    int hidden;
    int mon_num; //Number of monitors
    char** paths; //Number of paths
};

struct Config {
    char active_profile[256]; //Only for master config
    struct wallpaper current;
    struct wallpaper* wallpaper_list;
};

struct return_new_wallpaper {
    int arr_idx;
    struct wallpaper return_val;
};

struct Config load_profiles_new();

struct Config load_profile_file(struct Config config);

struct Token get_token(char* str);

struct Config parse_tokens();

struct return_new_wallpaper parse_wallpaper(struct Token* vec, int token_idx);

int is_valid_str_char(char val);

char *trimwhitespace(char *str);

void print_wallpaper(struct wallpaper printme);

char *dynstr(const char *str);

#endif