#include "config_parse.h"

static struct Config global_config;
static struct Token *tokens = NULL;
static int token_idx = 0;

struct Token* main_config_tokens = NULL;

struct Config load_profiles_new() {

    char fileName[512];
    strncpy(fileName,getenv("HOME"),512); //Get this one first
    strncat(fileName, "/.config/wallman/config",512);

    FILE *config_file = fopen(fileName, "r"); /* should check the result */

    if (config_file == NULL) {
        printf("Default config missing.\n");
        return global_config;
    }

    fseek(config_file, 0, SEEK_END);
    long size = ftell(config_file);
    fseek(config_file, 0, SEEK_SET);

    char profile_arr[size];

    char nxt = (char) fgetc(config_file);
    int tmp_idx = 0;
    while (nxt != EOF) {
        profile_arr[tmp_idx] = nxt;
        nxt = (char) fgetc(config_file);
        tmp_idx++;
    }

    int arr_idx = 0;
    int prev_idx = 0;


    while (arr_idx < size) { //for each character in the array
        //printf("%c",profile_arr[arr_idx]);

        if (profile_arr[arr_idx] == '#') {
            //printf("Found comment\n");
            while (profile_arr[arr_idx] != '\n') {
                arr_idx++;
            }
            arr_idx++;
            int com_len = arr_idx - prev_idx + 1;
            char comment[com_len];
            int sub_idx = 0;

            for (int idx = prev_idx; idx < arr_idx; idx++) {
                comment[sub_idx] = profile_arr[idx];
                sub_idx++;
            }
            comment[sub_idx] = 0;
            //char* comment_trim = trimwhitespace(comment);
            //printf("Comment: %s\n",comment_trim);
            struct Token comment_tok;
            comment_tok.TOKEN_NAME = COMMENT;
            strncpy(comment_tok.TOKEN_VAL,comment,256);
            vector_push_back(tokens,comment_tok);
            prev_idx = arr_idx;

            continue;

        }

        if (!is_valid_str_char(profile_arr[arr_idx])) {
            int sub_len = arr_idx - prev_idx + 1;
            char substring[sub_len];
            int sub_idx = 0;

            for (int idx = prev_idx; idx < arr_idx; idx++) {
                substring[sub_idx] = profile_arr[idx];
                sub_idx++;
            }
            substring[sub_idx] = 0;
            char *trimmed_substr = trimwhitespace(substring);

            if(strlen(trimmed_substr) != 0) {
                //printf("Substring: %s\n",trimmed_substr);

                struct Token meow = get_token(trimmed_substr);
                vector_push_back(tokens, meow);

            }
            //arr_idx++;
            prev_idx = arr_idx+1;
        }

        arr_idx++;
    }

    global_config = parse_tokens();
    main_config_tokens = tokens;

    if(strncmp(global_config.active_profile,"undefined_profile",256) == 0){
        printf("No profile file specified. You can use --set-profile to set it or add it to your config file.\n");
    }

    fclose(config_file);
    return global_config;
}


struct Config load_profile_file(struct Config config){

    //They defined a profile, let's see if we can open it.

    char profileFileName[512];
    strncpy(profileFileName,getenv("HOME"),256); //Get this one first
    strcat(profileFileName, "/.config/wallman/");
    strcat(profileFileName, config.active_profile);
    strcat(profileFileName, ".profile");

    if(verbose_flag)
        printf("Current Profile: %s\n",config.active_profile);

    FILE *profile_file = fopen(profileFileName, "r"); /* should check the result */

    if (profile_file == NULL) {
        printf("Could not open profile: %s\n",config.active_profile);
        printf("Please verify %s exists and is readable.\n",profileFileName);
        config.wallpaper_list = NULL;
        return config;
    }else{


        fseek(profile_file, 0, SEEK_END);
        long profile_size = ftell(profile_file);
        fseek(profile_file, 0, SEEK_SET);

        char profile_file_arr[profile_size];

        char nxt_prf = (char) fgetc(profile_file);
        int tmp_idx_prf = 0;
        while (nxt_prf != EOF) {
            profile_file_arr[tmp_idx_prf] = nxt_prf;
            nxt_prf = (char) fgetc(profile_file);
            tmp_idx_prf++;
        }

        int prf_arr_idx = 0;
        int prf_prev_idx = 0;

        struct Token* profile_tokens = NULL;

        while (prf_arr_idx < profile_size) { //for each character in the array
            //printf("%c",profile_arr[arr_idx]);

            if (profile_file_arr[prf_arr_idx] == '#') {
                while (profile_file_arr[prf_arr_idx] != '\n') {
                    prf_arr_idx++;
                }
                prf_arr_idx++;

                int com_len = prf_arr_idx - prf_prev_idx + 1;
                char comment[com_len];
                int sub_idx = 0;

                for (int idx = prf_prev_idx; idx < prf_arr_idx; idx++) {
                    comment[sub_idx] = profile_file_arr[idx];
                    sub_idx++;
                }
                comment[sub_idx] = 0;
                //char* comment_trim = trimwhitespace(comment);
                //printf("Comment: %s\n",comment_trim);
                struct Token comment_tok;
                comment_tok.TOKEN_NAME = COMMENT;
                strncpy(comment_tok.TOKEN_VAL,comment,256);
                vector_push_back(profile_tokens,comment_tok);
                prf_prev_idx = prf_arr_idx;
                continue;

            }

            if (!is_valid_str_char(profile_file_arr[prf_arr_idx])) {
                int sub_len = prf_arr_idx - prf_prev_idx + 1;
                char substring[sub_len];
                int sub_idx = 0;

                for (int idx = prf_prev_idx; idx < prf_arr_idx; idx++) {
                    substring[sub_idx] = profile_file_arr[idx];
                    sub_idx++;
                }
                substring[sub_idx] = 0;
                char *trimmed_substr = trimwhitespace(substring);

                if(strlen(trimmed_substr) != 0) {
                    //printf("Substring: %s\n",trimmed_substr);

                    struct Token meow = get_token(trimmed_substr);
                    vector_push_back(profile_tokens, meow);

                }
                //arr_idx++;
                prf_prev_idx = prf_arr_idx+1;
            }

            prf_arr_idx++;
        }

        int profile_idx = 0;
        config.wallpaper_list = NULL;
        while (profile_idx < vector_size(profile_tokens)) {
            //printf("Switching on: %s\n", stringFromToken(profile_tokens[profile_idx].TOKEN_NAME));
            switch (profile_tokens[profile_idx].TOKEN_NAME) {
                case PROFILE: {
                    struct return_new_wallpaper ret = parse_wallpaper(profile_tokens, profile_idx);
                    profile_idx = ret.arr_idx;
//                    printf("Got new idx: %d\n", profile_idx);
//                    print_wallpaper(ret.return_val);
                    vector_push_back(config.wallpaper_list,ret.return_val);


                    break;
                }
                case COMMENT: {
                    break;
                }
                default:
                    printf("Expecting profile, got %s\n", tokens[token_idx].TOKEN_VAL);
            }
            profile_idx++;

        }

        fclose(profile_file);
    }
    return config;
}

struct Token get_token(char *str) {

    if (strncmp(str, "Active Profile", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = ACTIVE_PROFILE;
        strncpy(new_tok.TOKEN_VAL, "Active Profile", 256);
        return new_tok;
    } else if (strncmp(str, "Category", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = CATEGORY;
        strncpy(new_tok.TOKEN_VAL, "Category", 256);
        return new_tok;
    } else if (strncmp(str, "Title", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = TITLE;
        strncpy(new_tok.TOKEN_VAL, "Title", 256);
        return new_tok;
    } else if (strncmp(str, "Hidden", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = HIDDEN;
        strncpy(new_tok.TOKEN_VAL, "Hidden", 256);
        return new_tok;
    } else if (strncmp(str, "Paths", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = PATHLIST;
        strncpy(new_tok.TOKEN_VAL, "Paths", 256);
        return new_tok;
    } else if (strncmp(str, "Profile", strlen(str)) == 0) {
        struct Token new_tok;
        new_tok.TOKEN_NAME = PROFILE;
        strncpy(new_tok.TOKEN_VAL, "Profile", 256);
        return new_tok;
    } else {
        struct Token new_tok;
        new_tok.TOKEN_NAME = STR;
        strncpy(new_tok.TOKEN_VAL, str, 256);
        return new_tok;
    }

}

struct Config parse_tokens() {
    struct Config config;
    strncat(config.active_profile,"undefined_profile",256);

    if (vector_size(tokens) == 0) {
        //Empty config file
    }

    while (token_idx < vector_size(tokens)) {
        switch (tokens[token_idx].TOKEN_NAME) {
            case ACTIVE_PROFILE:
                //printf("Got active profile\n");
                token_idx++;
                strncpy(config.active_profile, tokens[token_idx].TOKEN_VAL, 256);
                break;
                //Get the current active profile
            case PROFILE: {
                //printf("Got profile\n");
                //token_idx++;
                struct return_new_wallpaper ret = parse_wallpaper(tokens, token_idx);
                token_idx = ret.arr_idx;
                config.current = ret.return_val;
                //printf("Got new idx: %d\n", token_idx);
                //print_wallpaper(config.current);
                break;
            }
            case COMMENT:{
                break;
            }
            default:
                printf("Expecting profile, got %s\n", tokens[token_idx].TOKEN_VAL);
        }
        token_idx++;

    }

    //At this point we need to (if it exists) load more wallpapers

    return config;
}

struct return_new_wallpaper parse_wallpaper(struct Token *vec, int token_idx) {

    struct return_new_wallpaper return_me;

    struct wallpaper new_wall;

    new_wall.mon_num = 0;
    new_wall.hidden = 0;
    strncpy(new_wall.category,"none",256);
    new_wall.paths = NULL;

    int found_wallpaper = 0;

    while (!found_wallpaper) {

        if (vec[token_idx].TOKEN_NAME != PROFILE) {
            printf("Expected PROFILE, got: %s - Proceeding to next profile.\n", vec[token_idx].TOKEN_VAL);
            while (vec[token_idx].TOKEN_NAME != PROFILE) {
                token_idx++;
            }
            token_idx++;
            continue;
        }

        token_idx++;

        if (vec[token_idx].TOKEN_NAME != STR) {
            printf("Couldn't get profile name. Proceeding to next profile\n");
            while (vec[token_idx].TOKEN_NAME != PROFILE) {
                token_idx++;
            }
            token_idx++;
            continue;
        }

        found_wallpaper = 1;
        strncpy(new_wall.name, vec[token_idx].TOKEN_VAL, 256);
        strncpy(new_wall.disp_name, vec[token_idx].TOKEN_VAL, 256);

        //printf("Got profile name: %s\n", new_wall.name);

        token_idx++;
    }

    //Now we have our switch statement

    int parsed_all_options = 0;
    while(!parsed_all_options) {

        if(token_idx >= vector_size(vec)){
            break;
        }
        switch (vec[token_idx].TOKEN_NAME) {
            case TITLE:
                token_idx++;
                if(vec[token_idx].TOKEN_NAME != STR){
                    printf("Got token %s when expecting a string. Trying again.\n",vec[token_idx].TOKEN_VAL);
                    token_idx++;
                    continue;
                }

                strncpy(new_wall.disp_name,vec[token_idx].TOKEN_VAL,256);
                token_idx++;
                break;
            case CATEGORY:
                token_idx++;
                if(vec[token_idx].TOKEN_NAME != STR){
                    printf("Got token %s when expecting a string. Trying again.\n",stringFromToken(vec[token_idx].TOKEN_NAME));
                    token_idx++;
                    continue;
                }
                strncpy(new_wall.category,vec[token_idx].TOKEN_VAL,256);

                token_idx++;
                break;
            case HIDDEN:
                token_idx++;
                if(vec[token_idx].TOKEN_NAME != STR){
                    printf("Got token %s when expecting a string. Trying again.\n",vec[token_idx].TOKEN_VAL);
                    token_idx++;
                    continue;
                }

                if(strncmp(vec[token_idx].TOKEN_VAL,"true",6) == 0){
                    new_wall.hidden = 1;
                }else if(strncmp(vec[token_idx].TOKEN_VAL,"True",6) == 0){
                    new_wall.hidden = 1;
                }else if(strncmp(vec[token_idx].TOKEN_VAL,"false",6) == 0) {
                    new_wall.hidden = 0;
                }else if(strncmp(vec[token_idx].TOKEN_VAL,"False",6) == 0){
                    new_wall.hidden = 0;
                }else{
                    new_wall.hidden = 0;
                    printf("Expected true or false and got %s. Defaulting to false.\n",vec[token_idx].TOKEN_VAL);
                }

                token_idx++;

                break;
            case PATHLIST:
                token_idx++;
                int wallpaper_count = 0;
                while(vec[token_idx].TOKEN_NAME == STR){

                    //strncpy(new_wall.paths[wallpaper_count],vec[token_idx].TOKEN_VAL,256);
                    vector_push_back(new_wall.paths,vec[token_idx].TOKEN_VAL);
                    //printf("Pushed back: %s into %s\n",vec[token_idx].TOKEN_VAL,new_wall.name);
                    wallpaper_count++;
                    token_idx++;
                }
                new_wall.mon_num = wallpaper_count;
                break;
            case PROFILE:
                //This means we're on the next one!
                token_idx--; //Step back one since we aren't on this one yet
                parsed_all_options = 1;
                break;
            case STR:
                printf("Got string %s when expecting a token. Trying again.\n",vec[token_idx].TOKEN_VAL);
                token_idx++;
                break;
            case COMMENT:
                token_idx++;
                break;
            default:
                printf("Got invalid token: %s\n", stringFromToken(vec[token_idx].TOKEN_NAME));
                token_idx++;
        }

    }

    //Do some checking to see if we have everything we need

    return_me.return_val = new_wall;
    return_me.arr_idx = token_idx;

    return return_me;

}


int is_valid_str_char(char val) {

    if (val == '\n' || val == ':') {
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

/**
 * @brief print_wallpaper - Prints a wallpaper to terminal
 * @param printme - The wallpaper to print
 */
void print_wallpaper(struct wallpaper printme){

    printf("Profile: %s\n",printme.name);
    printf("\tTitle: %s\n",printme.disp_name);
    printf("\tCategory: %s\n",printme.category);
    printf("\tHidden: %d\n",printme.hidden);
    printf("\tPaths:\n");
    for(int idx=0;idx<printme.mon_num;idx++){
        printf("\t\t%s\n",printme.paths[idx]);
    }
}

char *dynstr(const char *str) {
    size_t str_len = strlen(str);
    char *new_str = (char *)malloc(sizeof(char) * (str_len + 1));
    if (new_str == NULL)
        return (char *)-1;

    strncpy(new_str, str, str_len);
    return new_str;
}