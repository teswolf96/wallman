//
// Created by lucifer on 1/19/18.
//

#ifndef WALLMAN_CONFIG_SAVE_H
#define WALLMAN_CONFIG_SAVE_H

#include "config_parse.h"
#include <sys/stat.h>

int save_main_config(struct Config curr);
int save_profile_config(struct Config curr);


/***
 *
 * @param file - file to save to
 * @param tokens - list of tokens
 * @param profile_loc - current index in token array
 * @return - New location in the token array
 */
int save_profile(struct wallpaper print_wall, FILE* file, struct Token* tokens, int profile_loc);
char** vector_pushback_unique(char** vector, char* str);
void write_jgmenu(struct Config config, char** categories);
#endif //WALLMAN_CONFIG_SAVE_H
