#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_parse.h"
#include <unistd.h>

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

void print_help();

struct Config config;
//struct wallpaper profiles[NUM_PROFILES];
char curr_wallpaper[256];
int num_profiles;

int main(int argc, char **argv) {

    config.wallpaper_list = NULL;

    if (argc < 2) {
        print_help();
        return 0;
    }else if (strncmp(argv[1], "--help", 256) == 0 ||
              strncmp(argv[1], "-h", 256) == 0) {
        print_help();
    }


    int list = 0;
    int set = 0;
    int set_current = 0;
    int switch_profile = 0;
    int save_new_config_file = 0;
    int save_as_current = 0;
    int delete_profile_arg = 0;
    char *profile_to_apply = NULL;
    char *new_config_file = NULL;
    char *delete_profile_name = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt (argc, argv, "lca:p:P:s:d:")) != -1)
        switch (c)
        {
            case 'l':
                list = 1;
                break;
            case 'c':
                set_current = 1;
                break;
            case 'a':
                set = 1;
                profile_to_apply = optarg;
                break;
            case 's':
                set = 1;
                save_as_current = 1;
                profile_to_apply = optarg;
                break;
            case 'p':
                switch_profile = 1;
                new_config_file = optarg;
                break;
            case 'P':
                switch_profile = 1;
                save_new_config_file = 1;
                new_config_file = optarg;
                break;
            case 'd':
                delete_profile_arg = 1;
                delete_profile_name = optarg;
                break;
            case '?':
                if (optopt == 'p')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'a')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'P')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'd')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            default:
                abort ();
        }

    //So this loads the main config file
    config = load_profiles_new();

    //Check if we want to switch profiles
    //If so, do this before loading the profile file
    if(switch_profile){
        printf("Using profile: %s\n",new_config_file);
        strncpy(config.active_profile,new_config_file,256);
        if(save_new_config_file){
            save_main_config(config);
        }
    }

    //Load the profile file
    config = load_profile_file(config);

    //These are mutually exclusive. Highest priority is setting a new profile
    if(set){
        struct wallpaper apply_profile = get_wallpaper(profile_to_apply);
        printf("Setting current profile to %s\n",apply_profile.name);
        config.current = apply_profile;
        set_profile(apply_profile);
        if(save_as_current){
            save_main_config(config);
        }
    }else if(set_current){
        set_profile(config.current);
    }

    if(delete_profile_arg){
        struct wallpaper delete_me = get_wallpaper(delete_profile_name);
        delete_profile(delete_me);
        save_profile_config(config);
    }


    save_profile_config(config);

    //List is ALWAYS last
    if(list){
        struct Config temp = config;
        list_profiles();
    }


    return 0;

}

/***
 * This function takes the name of a profile and returns the structure
 * @param profile_name - name of profile to find
 * @return - return the complete wallpaper
 */
struct wallpaper get_wallpaper(char *profile_name) {
    int conv = -1;

    assert(profile_name != NULL);

    for (int idx = 0; idx < vector_size(config.wallpaper_list); idx++) {
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


/**
 * Uses feh to set a background
 * @param profile - profile to apply
 * @return - this should probably return a status code or something
 */
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

    return 0;
}

/**
 * List all currently loaded profiles
 * @return - doesn't really return anything, but just in case
 */
int list_profiles() {
    //printf("Number of profiles loaded: %d\n", num_profiles);
    printf("Active Wallpaper:\n");
    printf("Profile: %s\n", config.current.name);
    printf("\tTitle: %s\n",config.current.disp_name);
    printf("\tCategory: %s\n", config.current.category);

    if(config.current.hidden)
        printf("\tHidden: True\n");
    else
        printf("\tHidden: False\n");


    printf("\tPaths:\n");
    for (int path_idx = 0; path_idx < vector_size(config.current.paths); path_idx++) {
        printf("\t\t%s\n", config.current.paths[path_idx]);
    }
    printf("\n");

    if(config.wallpaper_list == NULL){
        return 0;
    }

    printf("Loaded %d Profiles from file: %s\n",(int)vector_size(config.wallpaper_list),config.active_profile);
    for (int idx = 0; idx < vector_size(config.wallpaper_list); idx++) {

        printf("%d) %s - %s\n", idx + 1, config.wallpaper_list[idx].name, config.wallpaper_list[idx].disp_name);
        printf("\tCategory: %s\n", config.wallpaper_list[idx].category);

        if(config.wallpaper_list[idx].hidden)
            printf("\tHidden: True\n");
        else
            printf("\tHidden: False\n");

        printf("\tPaths:\n");
        for (int path_idx = 0; path_idx < vector_size(config.wallpaper_list[idx].paths); path_idx++) {
            printf("\t\t%s\n", config.wallpaper_list[idx].paths[path_idx]);
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

//TODO: Make this work with the new system
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

    printf("Deleting profile: %s\n",delete_me.name);
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
        return dynstr("True");
    }else{
        return dynstr("False");
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

void print_help(){
    printf("Usage: wallman [option(s)] [profile]\n");
    printf(" The options are: \n");
    printf(" -l - list profiles\n");
    printf(" -a - apply a profile (Does not set as current)\n");
    printf(" -s - apply a profile and set as current\n");
    printf(" -c - apply currently set profile\n");
    printf(" -p - set the config file to use for the command\n");
    printf(" -P - set the default config file to use\n");
//    printf(" -s --set profile_name monitor_num wallpaper_path - change a single wallpaper in a profile\n");
//    printf(" -s --set monitor_num wallpaper_path - sets a wallpaper temporarily\n");
//    printf(" profile_name - set profile\n");
//    printf(" -d --display profile_name display_name - set a profile's display name\n");
//    printf(" -C --category category_name profile_name - set a profile's category (not required)\n");
//    //printf(" -D --duplicate profile_name new_profile_name - Create a copy of a profile with a new name\n");
//    printf("\n");
//    printf("Examples:\n");
//    printf(" wallman cats    Apply the cats profile\n");
//    printf(" wallman --set cats 1 /path/to/cat.jpg    Set monitor 1 of the cats profile to /path/to/cats.jpg\n");
//    printf(" wallman -C Animals cats    Add the cats profile to the Animals category\n");
    return;
}