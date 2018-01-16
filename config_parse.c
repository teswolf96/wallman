#include "config_parse.h"

int load_profiles_new() {
    char *fileName = strcat(getenv("HOME"), "/.config/wallman/config");
    FILE *config = fopen(fileName, "r"); /* should check the result */

    if (config == NULL) {
        printf("File could not be opened\n");
        return -1;
    }


    fseek(config, 0, SEEK_END);
    long size = ftell(config);
    fseek(config, 0, SEEK_SET);

    char profile_arr[size];

    char nxt = (char) fgetc( config );
    int tmp_idx=0;
    while(nxt != EOF){
        profile_arr[tmp_idx] = nxt;
        nxt = (char) fgetc( config );
        tmp_idx++;
    }

    int arr_idx = 0;
    int prev_idx = 0;

    while(arr_idx < size){ //for each character in the array

        if(!is_valid_str_char(profile_arr[arr_idx])){
            int sub_len = arr_idx-prev_idx+1;
            char substring[sub_len];
            int sub_idx=0;

            for(int idx=prev_idx;idx<arr_idx;idx++){
                substring[sub_idx] = profile_arr[idx];
                sub_idx++;
            }
            substring[sub_idx] = 0;
            char* trimmed_substr = trimwhitespace(substring);
            //printf("Substring: %s\n",trimmed_substr);

            struct Token meow = get_token(trimmed_substr);
            printf("<%s> ", stringFromToken(meow.TOKEN_NAME));

            arr_idx++;
            prev_idx = arr_idx;
        }

        arr_idx++;
    }


    fclose(config);
    return 0;
}

struct Token get_token(char* str){

    if(strncmp(str,"Active Profile",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = ACTIVE_PROFILE;
        new_tok.TOKEN_VAL = "ACTIVE_PROFILE";
        return new_tok;
    }

    else if(strncmp(str,"Category",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = CATEGORY;
        new_tok.TOKEN_VAL = "CATEGORY";
        return new_tok;
    }

    else if(strncmp(str,"Title",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = TITLE;
        new_tok.TOKEN_VAL = "TITLE";
        return new_tok;
    }

    else if(strncmp(str,"Hidden",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = HIDDEN;
        new_tok.TOKEN_VAL = "HIDDEN";
        return new_tok;
    }

    else if(strncmp(str,"Path",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = PATHLIST;
        new_tok.TOKEN_VAL = "PATHLIST";
        return new_tok;
    }

    else{
        struct Token new_tok;
        new_tok.TOKEN_NAME = STR;
        new_tok.TOKEN_VAL = str;
        return new_tok;
    }

}

int is_valid_str_char(char val){

    if(val == '\n' || val == ':'){
        return 0;
    }
    return 1;
}

char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char) *str)) str++;

    if (*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;

    // Write new null terminator
    *(end + 1) = 0;

    return str;
}
