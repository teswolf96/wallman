#include "config_parse.h"


int save_main_config(struct Config curr){

    assert(main_config_tokens != NULL);

    for(int idx=0;idx<vector_size(main_config_tokens);idx++){
        printf("%s ",main_config_tokens[idx].TOKEN_VAL);
    }

    char *file_name = getenv("HOME");
    strncat(file_name,"/.config/wallman/config_testing",512);

    printf("Saving to file: %s\n",file_name);
    FILE *config = fopen(file_name, "w+");

    //fprintf(config,"meow_2!\n");

    int profile_loc = 0;
    if(main_config_tokens[0].TOKEN_NAME == ACTIVE_PROFILE){
        //We want to save an active profile

        fprintf(config,main_config_tokens[0].TOKEN_VAL);
        fprintf(config,": ");
        fprintf(config,curr.active_profile);
        fprintf(config,"\n");

        //Move the profile pointer forward twice
        profile_loc+=2;
    }

//    while(profile_loc < vector_size(main_config_tokens)) {
        profile_loc = save_profile(curr.current, config, main_config_tokens, profile_loc);
//    }

    return 0;

}


int save_profile(struct wallpaper print_wall, FILE* file, struct Token* tokens, int profile_loc){
//    printf("Called save profile\n");
//    printf("Current token: %s\n",tokens[profile_loc].TOKEN_VAL);

    //Print any comments before the profile
    while(tokens[profile_loc].TOKEN_NAME == COMMENT){
        fprintf(file,tokens[profile_loc].TOKEN_VAL);
        //fprintf(file,"\n");
        profile_loc++;
    }

    while(tokens[profile_loc].TOKEN_NAME != PROFILE && profile_loc < vector_size(tokens)){
        printf("Expected profile, got %s - Skipping ahead\n", tokens[profile_loc].TOKEN_VAL);
        profile_loc++;
    }

    fprintf(file,tokens[profile_loc].TOKEN_VAL);
    fprintf(file, ": ");
    profile_loc++;
    fprintf(file,tokens[profile_loc].TOKEN_VAL);
    fprintf(file,"\n");
    profile_loc++;

    int printed_title=0;
    int printed_hidden=0;
    int printed_cat=0;

    while(profile_loc < vector_size(tokens) && tokens[profile_loc].TOKEN_NAME != PROFILE) {
        printf("Current Token: %s\n", tokens[profile_loc].TOKEN_VAL);
        switch (tokens[profile_loc].TOKEN_NAME) {
            case TITLE:
                printed_title = 1;
                fprintf(file, "\t");
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case HIDDEN:
                printed_hidden = 1;
                fprintf(file, "\t");
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case CATEGORY:
                printed_cat = 1;
                fprintf(file, "\t");
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case STR:
                break;
            case COMMENT:
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                //profile_loc++;

                break;
            case PATHLIST:

                //Do our checks:
                if(!printed_title && strncmp(print_wall.disp_name,print_wall.name,256)!=0){
                    //We have NOT printed it and it is NOT the same as the name
                    printed_title = 1;
                    fprintf(file, "\t");
                    fprintf(file, "Title: ");
                    fprintf(file, ": ");
                    fprintf(file, print_wall.disp_name);
                    fprintf(file, "\n");
                }

                if(!printed_cat && strncmp(print_wall.category,"none",256)!=0){
                    //We have NOT printed it and it is NOT the same as the name
                    printed_title = 1;
                    fprintf(file, "\t");
                    fprintf(file, "Category: ");
                    fprintf(file, ": ");
                    fprintf(file, print_wall.category);
                    fprintf(file, "\n");
                }

                if(!printed_cat && print_wall.hidden!=0){
                    //We have NOT printed it and it is NOT the same as the name
                    printed_title = 1;
                    fprintf(file, "\t");
                    fprintf(file, "Hidden: True\n");
                }

                fprintf(file, "\t");
                fprintf(file, tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, "\n");
                while(tokens[profile_loc].TOKEN_NAME == STR){
                    //Paths
                    fprintf(file, "\t\t");
                    fprintf(file, tokens[profile_loc].TOKEN_VAL);
                    fprintf(file,"\n");
                    profile_loc++;
                }


                break;
            default:
                printf("Unexpected token %s\n", tokens[profile_loc].TOKEN_VAL);
        }


        profile_loc++;
    }
    return  profile_loc;
}

















