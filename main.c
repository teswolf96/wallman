#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_parse.h"
#include "config_save.h"

//#define NUM_PROFILES 200 //Does anyone really need more than 200 profiles?

int load_profiles();

int list_profiles();

//int save_profiles();
//int set_profile(char *profile_name);

int set_profile(struct wallpaper profile);

int set_path(char profile_name[], int mon_num, char path[]);

int set_profile_disp_name(char *profile_name, char *disp_name);

int get_wallpaper_num(char *profile_name);

//int delete_profile(int profile_num);

int delete_profile(struct wallpaper delete_me);

struct wallpaper get_wallpaper(char *profile_name);

int set_profile_temp(struct wallpaper temp);

char* int_to_bool(int bool);

int wallpapers_equal(struct wallpaper wall1, struct wallpaper wall2);

struct Config config;
//struct wallpaper profiles[NUM_PROFILES];
char curr_wallpaper[256];
int num_profiles;

int main(int argc, char **argv) {

    /*
     * Yeah I know, that if statement is terrible. Deal with it.
     */
    if (argc < 2 || strncmp(argv[1], "--help", 256) == 0 ||
        strncmp(argv[1], "-h", 256) == 0) {
        printf("Usage: wallman [option(s)] [profile]\n");
        printf(" The options are: \n");
        printf(" -l --list \tlist profiles\n");
        printf(" -c --current - apply currently set profile\n");
        printf(" -p --config - set the config file to use\n");
        printf(" -s --set profile_name monitor_num wallpaper_path - change a single wallpaper in a profile\n");
        printf(" -s --set monitor_num wallpaper_path - sets a wallpaper temporarily\n");
        printf(" profile_name - set profile\n");
        printf(" -d --display profile_name display_name - set a profile's display name\n");
        printf(" -C --category category_name profile_name - set a profile's category (not required)\n");
        //printf(" -D --duplicate profile_name new_profile_name - Create a copy of a profile with a new name\n");
        printf("\n");
        printf("Examples:\n");
        printf(" wallman cats    Apply the cats profile\n");
        printf(" wallman --set cats 1 /path/to/cat.jpg    Set monitor 1 of the cats profile to /path/to/cats.jpg\n");
        printf(" wallman -C Animals cats    Add the cats profile to the Animals category\n");

        /*
            printf("Profiles can be set up in ~/.config/wallman\n");
            printf("Example config:\n");
            printf("---------------\n");
            printf("current: Profile_Name\n");
            printf("Profile_Name:\n");
            printf("\tmonitors:2\n");
            printf("\t/path/to/wallpaper1.jpg\n");
            printf("\t/path/to/wallpaper2.png\n");
            printf("\n");
            printf("Profile_Name_2:\n");
            printf("\tmonitors:3\n");
            printf("\t/path/to/wallpaper1.jpg\n");
            printf("\t/path/to/wallpaper2.png\n");
            printf("\t/path/to/wallpaper3.png\n");
             */
        return 0;
    }


//    num_profiles = 0;
//    for (int idx = 0; idx < NUM_PROFILES; idx++) {
//        strncpy(profiles[idx].name, "undefined name", 256);
//
//    }

    int load_status = 0;
    config = load_profiles_new();

    if (strncmp(argv[1], "--list", 256) == 0 ||
        strncmp(argv[1], "-l", 256) == 0) {
        list_profiles();
        return 0;
    } else if (strncmp(argv[1], "--category", 256) == 0 ||
               strncmp(argv[1], "-C", 256) == 0) {
        if (argc < 4) {
            printf("Not enough arguments\n");
            return 0;
        }

        char cat_name[256];
        strncpy(cat_name, argv[2], 256);
        char profile_name[256];
        strncpy(profile_name, argv[3], 256);

        int profile_num = get_wallpaper_num(profile_name);
        strncpy(config.wallpaper_list[profile_num].category, cat_name, 256);
        //save_profiles();

        return 0;

    } else if (strncmp(argv[1], "--set", 256) == 0 ||
               strncmp(argv[1], "-s", 256) == 0) {

        if (argc < 4) {
            printf("Not enough arguments\n");
            return 0;
        } else if (argc == 4) {

            //struct wallpaper temp = get_wallpaper(curr_wallpaper);

            int monitor = atoi(argv[2]);
            monitor--; //Correct indexing because reasons
            //printf("Applying to monitor: %d\n",monitor);
            char path[256];
            strncpy(path, argv[3], 256);
            strncpy(config.current.paths[monitor], path, 256);

            set_profile_temp(config.current);


        } else if (argc == 5) {
            int mon_num = atoi(argv[3]);
            set_path(argv[2], mon_num, argv[4]);
            //save_profiles();
        }
        return 0;
    } else if (strncmp(argv[1], "--current", 256) == 0 ||
               strncmp(argv[1], "-c", 256) == 0) {
        set_profile(config.current);
        return 0;
    } else if (strncmp(argv[1], "--config", 256) == 0 ||
               strncmp(argv[1], "-p", 256) == 0) {
        if (argc < 3) {
            printf("Not enough arguments\n");
            return 0;
        }
        strncpy(config.active_profile,argv[2],256);
        save_main_config(config);
        return 0;
    } else if (strncmp(argv[1], "--displayname", 256) == 0 ||
               strncmp(argv[1], "-d", 256) == 0) {
        if (argc < 4) {
            printf("Not enough arguments\n");
            return 0;
        }
        set_profile_disp_name(argv[2], argv[3]);
        return 0;


    } else if (strncmp(argv[1], "--remove", 256) == 0 ||
               strncmp(argv[1], "-r", 256) == 0) {
        if (argc < 3) {
            printf("Not enough arguments\n");
            return 0;
        }

        struct wallpaper remove_profile = get_wallpaper(argv[2]);

        delete_profile(remove_profile);
        //save_profiles();
        printf("Removed profile: %s\n",argv[2]);
        list_profiles();
        return 0;

    } else /*if(strncmp(argv[1],"apply") == 0) */{
        if (argc < 2) {
            printf("Not enough arguments\n");
            return 0;
        }

        struct wallpaper apply_me = get_wallpaper(argv[1]);
        printf("Loaded wallpaper: %s\n",apply_me.name);
        config.current = apply_me;
        save_main_config(config);
        set_profile(apply_me);
        return 0;
    }

    return 0;

}

struct wallpaper get_wallpaper(char *profile_name) {
    int conv = -1;

    for (int idx = 0; idx < vector_size(config.wallpaper_list); idx++) {
        //printf("Comparing %s to %s\n",profile_name,config.wallpaper_list[idx].name);
        if (strncmp(profile_name, config.wallpaper_list[idx].name, 256) == 0) {
            conv = idx;
            break;
        }
    }
    if (conv == -1) {
        printf("Could not find profile\n");
        exit(0);
    }

    //printf("Current wallpaper is: %s\n",profiles[conv].name);
    return config.wallpaper_list[conv];

}

int get_wallpaper_num(char *profile_name) {
    int conv = -1;

    for (int idx = 0; idx < num_profiles; idx++) {
        //printf("Comparing %s to %s",argv[2],profiles[idx].name);
        if (strncmp(profile_name, config.wallpaper_list[idx].name, 256) == 0) {
            conv = idx;
            break;
        }
    }
    if (conv == -1) {
        printf("Could not find profile\n");
        exit(0);
    }

    //printf("Current wallpaper is: %s\n",profiles[conv].name);
    return conv;

}

int set_profile_temp(struct wallpaper temp) {

    /*Since we construct the temp profile ourselves, assume it is correct*/
    char command[1024] = "feh";
    for (int idx = 0; idx < temp.mon_num; idx++) {
        strcat(command, " --bg-scale \'");
        strcat(command, temp.paths[idx]);
        strcat(command, "\'");
    }
    strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
    //printf("Command to apply: %s\n", command);
    system(command);

}

int set_profile_disp_name(char *profile_name, char *disp_name) {
    //printf("Let's change the display profile\n");
    int conv = atoi(profile_name);
    if (conv > 0) { /* User passed in an integer */
        if (conv > num_profiles) {
            printf("No matching profiles\n");
            return 0;
        }


    } else if (conv == 0) { /* User passed in a string or is trying to be tricky >_> */
        conv = -1;
        //printf("Word detected\n");
        for (int idx = 0; idx < num_profiles; idx++) {
            //printf("Comparing %s to %s",argv[2],profiles[idx].name);
            if (strncmp(profile_name, config.wallpaper_list[idx].name, 256) == 0) {
                conv = idx;
                break;
            }
        }
        if (conv == -1) {
            printf("No matching profiles\n");
            return 0;
        }
    } else {
        printf("No matching profiles\n");
    }

    //printf("Attempting to change: %s display name to %s\n",profiles[conv].name, disp_name);

    strncpy(config.wallpaper_list[conv].disp_name, disp_name, 256);

    //save_profiles();

}


int set_profile(struct wallpaper profile){

    char command[1024] = "feh";
    for (int idx = 0; idx < profile.mon_num; idx++) {
        strcat(command, " --bg-scale \'");
        strcat(command, profile.paths[idx]);
        strcat(command, "\'");
    }

    strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
    //printf("Command to apply: %s\n",command);
    system(command);

}

int list_profiles() {
    //printf("Number of profiles loaded: %d\n", num_profiles);
    printf("Active Wallpaper:\n");
    printf("Profile: %s\n", config.current.name);
    printf("\tTitle: %s\n",config.current.disp_name);
    printf("\tCategory: %s\n", config.current.category);
    printf("\tHidden: %s\n",int_to_bool(config.current.hidden));
    printf("\tPaths:\n");
    for (int path_idx = 0; path_idx < vector_size(config.current.paths); path_idx++) {
        printf("\t\t%s\n", config.current.paths[path_idx]);
    }
    printf("\n");


    printf("Loaded Profiles from file: %s:\n",config.active_profile);
    for (int idx = 0; idx < vector_size(config.wallpaper_list); idx++) {

        printf("%d) %s - %s\n", idx + 1, config.wallpaper_list[idx].name, config.wallpaper_list[idx].disp_name);
        printf("\tCategory: %s\n", config.wallpaper_list[idx].category);
        printf("\tHidden: %s\n",int_to_bool(config.wallpaper_list[idx].hidden));

        for (int path_idx = 0; path_idx < vector_size(config.wallpaper_list[idx].paths); path_idx++) {
            printf("\t%s\n", config.wallpaper_list[idx].paths[path_idx]);
        }
        printf("\n");
    }
    return 0;
}

//Come back to this later
//int save_profiles() {
//    //printf("Saving profiles\n");
//    //char *fileName = strcat(getenv("HOME"), "/.config/wallman");
//    char *fileName = getenv("HOME");
//    //printf("hi! %s - :-)\n",fileName);
//    FILE *config = fopen(fileName, "w+"); /* should check the result */
//
//    if (config == NULL) {
//        fileName = "/etc/wallman";
//        config = fopen(fileName, "r");
//        if (config == NULL) {
//            printf("File could not be opened\n");
//            return -1;
//        }
//    }
//    //fprintf(config,"meow!\n");
//    //fputs("meow2!\n",config);
//    char current[100] = "";
//    sprintf(current, "current: %s\n", curr_wallpaper);
//    fprintf(config, current);
//
//    int profile_idx = 0;
//    while (profile_idx < NUM_PROFILES && strncmp(profiles[profile_idx].name, "undefined name", 256) != 0) {
//        char name_bak[50];
//        strncpy(name_bak, profiles[profile_idx].name, 256);
//        char *title = strcat(profiles[profile_idx].name, ":");
//        if (strncmp(profiles[profile_idx].disp_name, name_bak, 256) == 0) {
//            /* They are the same */
//            title = strcat(name_bak, ":\n");
//        } else {
//            //printf("%s != %s\n",profiles[profile_idx].disp_name,profiles[profile_idx].name);
//            title = strcat(title, " ");
//            title = strcat(title, profiles[profile_idx].disp_name);
//            title = strcat(title, "\n");
//        }
//        fprintf(config, title);
//        char monitor_count_str[10] = "";
//        sprintf(monitor_count_str, "    monitors: %d\n", profiles[profile_idx].mon_num);
//        fprintf(config, monitor_count_str);
//
//        char cat_name[256] = "";
//        sprintf(cat_name, "    category: %s\n", profiles[profile_idx].category);
//        fprintf(config, cat_name);
//
//        for (int idx = 0; idx < profiles[profile_idx].mon_num; idx++) {
//            char path[500] = "";
//            sprintf(path, "    %s\n", profiles[profile_idx].paths[idx]);
//            fprintf(config, path);
//        }
//        fprintf(config, "\n");
//        profile_idx++;
//    }
//
//
//    fclose(config);
//
//    char command[1024] = "export HOME=/home/"; //lucifer && exec wallman-genmenu";
//    char *user = getenv("USER");
//    strcat(command, user);
//    strcat(command, " && exec wallman-genmenu;");
//    strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
//    //printf("Command to apply: %s\n", command);
//    system(command);
//
//}

int set_path(char profile_name[80], int mon_num, char path[160]) {
    printf("Setting profile: %s on monitor %d to path %s\n", profile_name, mon_num, path);
    int conv = -1;
    //printf("Word detected\n");
    for (int idx = 0; idx < vector_size(config.wallpaper_list); idx++) {
        //printf("Comparing %s to %s",argv[2],profiles[idx].name);
        if (strncmp(profile_name, config.wallpaper_list[idx].name, 256) == 0) {
            conv = idx;
            break;
        }
    }
    if (conv == -1) {
        printf("No matching profiles\n");
        return 0;
    }

    printf("Identified profile index as %d\n", conv);

    if (mon_num > config.wallpaper_list[conv].mon_num || mon_num <= 0) {
        printf("Invalid monitor selection\n");
        return 0;
    }

    strncpy(config.wallpaper_list[conv].paths[mon_num - 1], path, 256);
    return 0;
}

int delete_profile(struct wallpaper delete_me){
//
//    for(int idx = remove_idx; idx < num_profiles; idx++){
//        //printf("Copying %s over %s\n",profiles[idx+1].name,profiles[idx].name);
//
//        strncpy(profiles[idx].name,profiles[idx+1].name,256);
//        profiles[idx].mon_num = profiles[idx+1].mon_num;
//        strncpy(profiles[idx].disp_name,profiles[idx+1].disp_name,256);
//        strncpy(profiles[idx].category,profiles[idx+1].category,256);
//        for(int jdx=0;jdx<profiles[idx].mon_num;jdx++){
//            strncpy(profiles[idx].paths[jdx],profiles[idx+1].paths[jdx],256);
//        }
//
//    }

    struct wallpaper* new_vec = NULL;
    for(int idx=0;idx<vector_size(config.wallpaper_list);idx++){
        if(!wallpapers_equal(config.wallpaper_list[idx],delete_me)){
            vector_push_back(new_vec,config.wallpaper_list[idx]);
        }
    }
    config.wallpaper_list = new_vec;
    //Fix num profiles before returning
    num_profiles--;

    return 0;
}

char* int_to_bool(int bool){
    char* str_to_ret;
    if(bool){
        str_to_ret = "True";
        return str_to_ret;
    }else{
        str_to_ret = "False";
        return str_to_ret;
    }
}

int wallpapers_equal(struct wallpaper wall1, struct wallpaper wall2){

    if(strncmp(wall1.name,wall2.name,256) != 0){
        return 0;
    }

    if(strncmp(wall1.category,wall2.category,256) != 0){
        return 0;
    }

    if(vector_size(wall1.paths) != vector_size(wall2.paths)){
        return 0;
    }

    for(int idx=0;idx < vector_size(wall1.paths);idx++){
        if(strncmp(wall1.paths[idx],wall2.paths[idx],256) != 0){
            return 0;
        }
    }

    return 1;
}