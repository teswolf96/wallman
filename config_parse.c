#include "config_parse.h"

static struct Config global_config;
static struct Token* tokens = NULL;
static int token_idx = 0;

struct Config load_profiles_new() {
    char *fileName = strcat(getenv("HOME"), "/.config/wallman/config"); //Get this one first
    FILE *config_file = fopen(fileName, "r"); /* should check the result */

    if (config_file == NULL) {
        printf("File could not be opened\n");
        return global_config;
    }


    fseek(config_file, 0, SEEK_END);
    long size = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);

    char profile_arr[size];

    char nxt = (char) fgetc( config_file );
    int tmp_idx=0;
    while(nxt != EOF){
        profile_arr[tmp_idx] = nxt;
        nxt = (char) fgetc( config_file );
        tmp_idx++;
    }

    int arr_idx = 0;
    int prev_idx = 0;


    while(arr_idx < size){ //for each character in the array

        if(profile_arr[arr_idx] == '#'){
            while(profile_arr[arr_idx] != '\n'){
                arr_idx++;
            }
            prev_idx = arr_idx;
            arr_idx++;
        }

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
            vector_push_back(tokens,meow);

//            printf("<%s> ", stringFromToken(meow.TOKEN_NAME));
//            if(meow.TOKEN_NAME == STR){
//                printf("%s ",meow.TOKEN_VAL);
//            }

            arr_idx++;
            prev_idx = arr_idx;
        }

        arr_idx++;
    }


//    if(tokens) {
//        size_t i;
//        for(i = 0; i < vector_size(tokens); ++i) {
//            printf("%s\n",stringFromToken(tokens[i].TOKEN_NAME));
//            if(tokens[i].TOKEN_NAME == STR){
//                printf("%s\n",tokens[i].TOKEN_VAL);
//            }
//        }
//    }

    global_config = parse_tokens();

    fclose(config_file);
    return global_config;
}

struct Token get_token(char* str){

    if(strncmp(str,"Active Profile",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = ACTIVE_PROFILE;
        strncpy(new_tok.TOKEN_VAL,"ACTIVE_PROFILE",256);
        return new_tok;
    }

    else if(strncmp(str,"Category",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = CATEGORY;
        strncpy(new_tok.TOKEN_VAL,"CATEGORY",256);
        return new_tok;
    }

    else if(strncmp(str,"Title",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = TITLE;
        strncpy(new_tok.TOKEN_VAL,"TITLE",256);
        return new_tok;
    }

    else if(strncmp(str,"Hidden",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = HIDDEN;
        strncpy(new_tok.TOKEN_VAL,"HIDDEN",256);
        return new_tok;
    }

    else if(strncmp(str,"Path",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = PATHLIST;
        strncpy(new_tok.TOKEN_VAL,"PATHLIST",256);
        return new_tok;
    }

    else if(strncmp(str,"Profile",strlen(str)) == 0){
        struct Token new_tok;
        new_tok.TOKEN_NAME = PROFILE;
        strncpy(new_tok.TOKEN_VAL,"PROFILE",256);
        return new_tok;
    }

    else{
        struct Token new_tok;
        new_tok.TOKEN_NAME = STR;
        strncpy(new_tok.TOKEN_VAL,str,256);
        return new_tok;
    }

}

struct Config parse_tokens(){
    struct Config config;

    if(vector_size(tokens) == 0){
        //Empty config file
    }

    while(token_idx < vector_size(tokens)) {
        switch (tokens[token_idx].TOKEN_NAME) {
            case ACTIVE_PROFILE:
                printf("Got active profile\n");
                token_idx++;
                strncpy(config.active_profile,tokens[token_idx].TOKEN_VAL,256);
                break;
            //Get the current active profile
            case PROFILE:
                printf("Got profile\n");
                token_idx++;
                config.current = parse_wallpaper();
                break;
            default:
                printf("Got invalid token: %s\n", tokens[token_idx].TOKEN_VAL);
        }
        token_idx++;

    }

    return config;
}

struct wallpaper parse_wallpaper(struct Token vec, int vec_idx){

    struct wallpaper new_wall;

    if(tokens[token_idx].TOKEN_NAME != STR){
        printf("Missing profile name. Proceeding to next profile");
        while(tokens[token_idx].TOKEN_NAME != PROFILE){
            token_idx++;
        }
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
