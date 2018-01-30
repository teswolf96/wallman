//
// Created by lucifer on 1/19/18.
//

#ifndef WALLMAN_CONFIG_SAVE_H
#define WALLMAN_CONFIG_SAVE_H

#include "config_parse.h"
#include <sys/stat.h>
#include <unistd.h>

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

/**
 * Adds a string to a vector of strings if the string is not in the vector
 * @param vector - vector to push string into
 * @param str - string to push into vector
 * @return - returns a pointer to the passed in vector
 */
char** vector_pushback_unique(char** vector, char* str);
/**
 * Writes the jgmenu scripts and config to ~/.config/wallman
 * @param config - config to save
 * @param categories - list of categories
 */
void write_jgmenu(struct Config config, char** categories);
#endif //WALLMAN_CONFIG_SAVE_H
