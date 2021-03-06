#include "config_save.h"


int save_main_config(struct Config curr){

    //assert(main_config_tokens != NULL);

//    for(int idx=0;idx<vector_size(main_config_tokens);idx++){
//        printf("%s ",main_config_tokens[idx].TOKEN_VAL);
//    }

    if(verbose_flag)
        printf("Saving Current Profile: %s\n",curr.current.name);

    char file_name[600]  = "";
    strncpy(file_name,getenv("HOME"),256);
    strncat(file_name,"/.config/wallman/config",512);

    if(verbose_flag)
        printf("Saving to file: %s\n",file_name);

    FILE *config = fopen(file_name, "w+");

    //fprintf(config,"meow_2!\n");

    int profile_loc = 0;
        //We want to save an active profile

    fprintf(config,"%s",main_config_tokens[0].TOKEN_VAL);
    fprintf(config,": ");
    fprintf(config,"%s",curr.active_profile);
    fprintf(config,"\n");

    //Move the profile pointer forward twice
    profile_loc+=2;


//    while(profile_loc < vector_size(main_config_tokens)) {
      //  profile_loc = save_profile(curr.current, config, main_config_tokens, profile_loc);
//    }

    fprintf(config,"Profile: ");
    fprintf(config,"%s",curr.current.name);
    fprintf(config,"\n");

    if(strncmp(curr.current.disp_name,
               curr.current.name,256) != 0){
        fprintf(config,"\tTitle: ");
        fprintf(config,"%s",curr.current.disp_name);
        fprintf(config,"\n");
    }

    if(strncmp(curr.current.category,"none",256) != 0){
        fprintf(config,"\tCategory: ");
        fprintf(config,"%s",curr.current.category);
        fprintf(config,"\n");
    }

    if(curr.current.hidden){
        fprintf(config,"\tHidden: True\n");
    }

    //Paths
    fprintf(config,"\tPaths:\n");

    for(int path_idx = 0; path_idx<vector_size(curr.current.paths);path_idx++){
        fprintf(config,"\t\t");
        fprintf(config,"%s",curr.current.paths[path_idx]);
        fprintf(config,"\n");
    }


    fprintf(config,"\n");
    fclose(config);
    return 0;

}


int save_profile_config(struct Config curr){

    //This is a vector to store all categories. Used for jgmenu config
    char** category_list = NULL;
    char** category_list_hidden = NULL;

    assert(curr.wallpaper_list != NULL);


//    for(int idx=0;idx<vector_size(main_config_tokens);idx++){
//        printf("%s ",main_config_tokens[idx].TOKEN_VAL);
//    }

    char file_name[512] = "";
    strncpy(file_name,getenv("HOME"),256);
    strncat(file_name,"/.config/wallman/",128);
    strncat(file_name,curr.active_profile,64);
    strncat(file_name,".profile",64);

    if(verbose_flag)
        printf("Saving to file: %s\n",file_name);
    FILE *config = fopen(file_name, "w+");



    for(int idx=0;idx<vector_size(curr.wallpaper_list);idx++){

        fprintf(config,"Profile: ");
        fprintf(config,"%s",curr.wallpaper_list[idx].name);
        fprintf(config,"\n");

        if(strncmp(curr.wallpaper_list[idx].disp_name,
                   curr.wallpaper_list[idx].name,256) != 0){
            fprintf(config,"\tTitle: ");
            fprintf(config,"%s",curr.wallpaper_list[idx].disp_name);
            fprintf(config,"\n");
        }

        if(strncmp(curr.wallpaper_list[idx].category,"none",256) != 0){

            if(!curr.wallpaper_list[idx].hidden) {
                category_list = vector_pushback_unique(category_list, curr.wallpaper_list[idx].category);
            }else{
                category_list_hidden = vector_pushback_unique(category_list_hidden,curr.wallpaper_list[idx].category);
            }

            fprintf(config,"\tCategory: ");
            fprintf(config,"%s",curr.wallpaper_list[idx].category);
            fprintf(config,"\n");
        }

        if(curr.wallpaper_list[idx].hidden){
            fprintf(config,"\tHidden: True\n");
        }

        //Paths
        fprintf(config,"\tPaths:\n");

        for(int path_idx = 0; path_idx<vector_size(curr.wallpaper_list[idx].paths);path_idx++){
            fprintf(config,"\t\t");
            fprintf(config,"%s",curr.wallpaper_list[idx].paths[path_idx]);
            fprintf(config,"\n");
        }


        fprintf(config,"\n");
    }

    fclose(config);


//    printf("Found Categories: ");
//    for(int idx=0;idx<vector_size(category_list);idx++){
//        printf("%s ",category_list[idx]);
//    }printf("\n");

    write_jgmenu(curr,category_list);
    write_jgmenu_hidden(curr,category_list_hidden);

    return 0;

}


int save_profile(struct wallpaper print_wall, FILE* file, struct Token* tokens, int profile_loc){
    //printf("Called save profile on %s\n",print_wall.name);

//    printf("Debug: Print All Tokens:\n");
//    for(int idx=0;idx<vector_size(tokens);idx++){
//        printf("Token: %s\n",tokens[idx].TOKEN_VAL);
//    }


    //printf("Current token: %s\n",tokens[profile_loc].TOKEN_VAL);

    //Print any comments before the profile
    while(tokens[profile_loc].TOKEN_NAME == COMMENT){
        fprintf(file,"%s",tokens[profile_loc].TOKEN_VAL);
        //fprintf(file,"\n");
        profile_loc++;
    }

    while(tokens[profile_loc].TOKEN_NAME != PROFILE && profile_loc < vector_size(tokens)){
        printf("Expected profile, got %s - Skipping ahead\n", tokens[profile_loc].TOKEN_VAL);
        profile_loc++;
    }

    fprintf(file,"%s",tokens[profile_loc].TOKEN_VAL);
    fprintf(file, ": ");
    profile_loc++;
    fprintf(file,"%s",print_wall.name);
    fprintf(file,"\n");
    profile_loc++;

    int printed_title=0;
    int printed_hidden=0;
    int printed_cat=0;

    while(profile_loc < vector_size(tokens) && tokens[profile_loc].TOKEN_NAME != PROFILE) {
        //printf("Current Token: %s\n", tokens[profile_loc].TOKEN_VAL);
        switch (tokens[profile_loc].TOKEN_NAME) {
            case TITLE:
                printed_title = 1;
                fprintf(file, "\t");
                fprintf(file, "%s",tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, "%s",print_wall.disp_name);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case HIDDEN:
                printed_hidden = 1;
                fprintf(file, "\t");
                fprintf(file, "%s",tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;

                char* bool_str;
                if(print_wall.hidden){
                    bool_str = "True";
                }else{
                    bool_str = "False";
                }

                fprintf(file,"%s",bool_str);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case CATEGORY:
                printed_cat = 1;
                fprintf(file, "\t");
                fprintf(file,"%s",tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, "%s",print_wall.category);
                //profile_loc++;
                fprintf(file, "\n");
                break;
            case STR:
                break;
            case COMMENT:
                fprintf(file, "%s",tokens[profile_loc].TOKEN_VAL);
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
                    fprintf(file, "%s",print_wall.disp_name);
                    fprintf(file, "\n");
                }

                if(!printed_cat && strncmp(print_wall.category,"none",256)!=0){
                    //We have NOT printed it and it is NOT the same as the name
                    printed_cat = 1;
                    fprintf(file, "\t");
                    fprintf(file, "Category: ");
                    fprintf(file, ": ");
                    fprintf(file, "%s",print_wall.category);
                    fprintf(file, "\n");

                }

                if(!printed_hidden && print_wall.hidden!=0){
                    //We have NOT printed it and it is NOT the same as the name
                    printed_hidden = 1;
                    fprintf(file, "\t");
                    fprintf(file, "Hidden: True\n");
                }

                fprintf(file, "\t");
                fprintf(file, "%s",tokens[profile_loc].TOKEN_VAL);
                fprintf(file, ": ");
                profile_loc++;
                fprintf(file, "\n");
                //int wall_idx;
                //printf("Printing %d paths\n",vector_size(print_wall.paths));
                for(int idx=0;idx<vector_size(print_wall.paths);idx++){
                    fprintf(file, "\t\t");
                    fprintf(file,"%s", print_wall.paths[idx]);
                    fprintf(file, "\n");
                }
                //Eat extra tokens!
                while(tokens[profile_loc].TOKEN_NAME == STR){
                    //Paths
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


char** vector_pushback_unique(char** vector, char* str){

    int found = 0;
    for(int idx=0;idx<vector_size(vector);idx++){

        if(strncmp(str,vector[idx],256) == 0){
            found = 1;
            break;
        }
    }
    if(found == 0){
        vector_push_back(vector,str);
    }

    return vector;
}

void write_jgmenu(struct Config config, char** categories){

    char file_name_script[600] = "";
    strncpy(file_name_script,getenv("HOME"),256);
    strncat(file_name_script,"/.config/wallman/jgmenu_run",512);

    if( access( file_name_script, F_OK ) != -1 ) {
        //This allows the user to modify the script and prevent it from being overwritten
        if (verbose_flag)
            printf("Script already exists, skipping\n");
    }else {
        if (verbose_flag)
            printf("Writing jgmenu script: %s\n", file_name_script);
        FILE *jgmenu_script = fopen(file_name_script, "w+");

        if (jgmenu_script == NULL) {
            printf("Error opening %s\n", file_name_script);
            return;
        }

        fprintf(jgmenu_script, "#!/bin/sh\n"
                "cat ~/.config/wallman/jgmenu | jgmenu --vsimple --icon-size=0 --at-pointer=1");

        //char mode[] = "755";
        chmod(file_name_script, S_IRWXU);
        fclose(jgmenu_script);
    }

    char file_name[600] = "";
    strncpy(file_name,getenv("HOME"),256);
    strncat(file_name,"/.config/wallman/jgmenu",512);

    if(verbose_flag)
        printf("Writing jgmenu file: %s\n",file_name);
    FILE *jgmenu = fopen(file_name, "w+");

    if(jgmenu == NULL){
        printf("Error opening %s\n",file_name);
        return;
    }

    for(int jdx=0;jdx<vector_size(config.wallpaper_list);jdx++){
        if(strncmp(config.wallpaper_list[jdx].category,"none",256)==0 && !config.wallpaper_list[jdx].hidden){
            //printf("Writing config: %s\n",config.wallpaper_list[jdx].disp_name);
            fprintf(jgmenu,"%s",config.wallpaper_list[jdx].disp_name);
            fprintf(jgmenu,",wallman -s ");
            fprintf(jgmenu,"%s",config.wallpaper_list[jdx].name);
            fprintf(jgmenu,"&");
            fprintf(jgmenu,"\n");
        }
    }

    for(int idx=0;idx<vector_size(categories);idx++){
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,",^checkout(");
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,")\n");
    }


   for(int idx=0;idx<vector_size(categories);idx++){
       fprintf(jgmenu,"%s",categories[idx]);
       fprintf(jgmenu,",^tag(");
       fprintf(jgmenu,"%s",categories[idx]);
       fprintf(jgmenu,")\n");
       for(int jdx=0;jdx<vector_size(config.wallpaper_list);jdx++){
           if(strncmp(config.wallpaper_list[jdx].category,categories[idx],256)==0 && !config.wallpaper_list[jdx].hidden){
               //printf("Writing config: %s\n",config.wallpaper_list[jdx].disp_name);
               fprintf(jgmenu,"%s",config.wallpaper_list[jdx].disp_name);
               fprintf(jgmenu,",wallman -s ");
               fprintf(jgmenu,"%s",config.wallpaper_list[jdx].name);
               fprintf(jgmenu,"&");
               fprintf(jgmenu,"\n");
           }
       }
       fprintf(jgmenu,"^back()\n");

   }
    fclose(jgmenu);

}

void write_jgmenu_hidden(struct Config config, char** categories){

//    printf("Called write jgmenu hidden\n");
//    printf("Got categories: \n");
//    for(int idx=0;idx<vector_size(categories);idx++){
//        printf("%s, ",categories[idx]);
//    }printf("\n");


    char file_name_script[600] = "";
    strncpy(file_name_script,getenv("HOME"),256);
    strncat(file_name_script,"/.config/wallman/jgmenu_run_hidden",512);

    if( access( file_name_script, F_OK ) != -1 ) {
        //This allows the user to modify the script and prevent it from being overwritten
        if (verbose_flag)
            printf("Script already exists, skipping\n");
    }else {
        if (verbose_flag)
            printf("Writing jgmenu script: %s\n", file_name_script);
        FILE *jgmenu_script = fopen(file_name_script, "w+");

        if (jgmenu_script == NULL) {
            printf("Error opening %s\n", file_name_script);
            return;
        }

        fprintf(jgmenu_script, "#!/bin/sh\n"
                "cat ~/.config/wallman/jgmenu_hidden | jgmenu --vsimple --icon-size=0 --at-pointer=1");

        //char mode[] = "755";
        chmod(file_name_script, S_IRWXU);
        fclose(jgmenu_script);
    }

    char file_name[600] = "";
    strncpy(file_name,getenv("HOME"),256);
    strncat(file_name,"/.config/wallman/jgmenu_hidden",512);

    if(verbose_flag)
        printf("Writing jgmenu file: %s\n",file_name);
    FILE *jgmenu = fopen(file_name, "w+");

    if(jgmenu == NULL){
        printf("Error opening %s\n",file_name);
        return;
    }

    for(int jdx=0;jdx<vector_size(config.wallpaper_list);jdx++){
        if(strncmp(config.wallpaper_list[jdx].category,"none",256)==0 && config.wallpaper_list[jdx].hidden){
            //printf("Writing config: %s\n",config.wallpaper_list[jdx].disp_name);
            fprintf(jgmenu,"%s",config.wallpaper_list[jdx].disp_name);
            fprintf(jgmenu,",wallman -s ");
            fprintf(jgmenu,"%s",config.wallpaper_list[jdx].name);
            fprintf(jgmenu,"&");
            fprintf(jgmenu,"\n");
        }
    }

    for(int idx=0;idx<vector_size(categories);idx++){
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,",^checkout(");
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,")\n");
    }


    for(int idx=0;idx<vector_size(categories);idx++){
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,",^tag(");
        fprintf(jgmenu,"%s",categories[idx]);
        fprintf(jgmenu,")\n");
        for(int jdx=0;jdx<vector_size(config.wallpaper_list);jdx++){
            if(strncmp(config.wallpaper_list[jdx].category,categories[idx],256)==0 && config.wallpaper_list[jdx].hidden){
                //printf("Writing config: %s\n",config.wallpaper_list[jdx].disp_name);
                fprintf(jgmenu,"%s",config.wallpaper_list[jdx].disp_name);
                fprintf(jgmenu,",wallman -s ");
                fprintf(jgmenu,"%s",config.wallpaper_list[jdx].name);
                fprintf(jgmenu,"&");
                fprintf(jgmenu,"\n");
            }
        }
        fprintf(jgmenu,"^back()\n");

    }
    fclose(jgmenu);

}
