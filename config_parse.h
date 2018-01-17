#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "vector.h"

typedef enum NAME{ACTIVE_PROFILE, CATEGORY, TITLE, HIDDEN, PATHLIST, PROFILE, END, STR} Token_Enum;

static inline char *stringFromToken(enum NAME f)
{
    static const char *strings[] = { "ACTIVE_PROFILE", "CATEGORY", "TITLE", "HIDDEN","PATHLIST", "PROFILE", "END", "STR"};

    return strings[f];
}

struct Token{
    Token_Enum TOKEN_NAME;
    char TOKEN_VAL[256];
};


struct wallpaper {
    char name[256]; //Profile name
    char category[256]; //Category name
    char disp_name[256];
    int hidden;
    int mon_num; //Number of monitors
    char paths[10][256]; //Number of paths
};

struct Config {
    char active_profile[256]; //Only for master config
    struct wallpaper current;
    struct wallpaper* wallpaper_list;
};

struct Config load_profiles_new();

struct Token get_token(char* str);

struct Config parse_tokens();

struct wallpaper parse_wallpaper(struct Token vec, int vec_idx);

int is_valid_str_char(char val);

char *trimwhitespace(char *str);
