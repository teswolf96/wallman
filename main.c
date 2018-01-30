#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_parse.h"
#include <unistd.h>
#include <getopt.h>

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

int set_wallpaper_path(int current, char* profile_name, int mon_num, char* wall_path);

int wallpapers_equal(struct wallpaper wall1, struct wallpaper wall2);

void print_help();

int verbose_flag = 0;

struct Config config;
//struct wallpaper profiles[NUM_PROFILES];
char curr_wallpaper[256];
int num_profiles;

int main(int argc, char **argv) {

    config.wallpaper_list = NULL;

    if (argc < 2) {
        print_help();
        return 0;
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
    char *wall_path = NULL;
    char *profile_to_modify = NULL;
    int set_new_wall_path = 0;

    int index;
    int print_help_arg = 0;
    int monitor = 1;
    int c;

    opterr = 0;


    //while ((c = getopt (argc, argv, "lca:p:P:s:d:")) != -1)
    while(1) {
        static struct option long_options[] =
                {
                        /* These options set a flag. */
                        {"brief",   no_argument,       &verbose_flag, 0},
                        /* These options don’t set a flag.
                           We distinguish them by their indices. */
                        {"help",     no_argument,       0, 'h'},
                        {"verbose",     no_argument,       0, 'v'},
                        {"list",     no_argument,       0, 'l'},
                        {"current",  no_argument,       0, 'c'},
                        {"apply",  required_argument, 0, 'a'},
                        {"set",  required_argument, 0, 's'},
                        {"temp-config",    required_argument, 0, 'f'},
                        {"set-config",    required_argument, 0, 'F'},
                        {"delete",    required_argument, 0, 'd'},
                        {"wallpaper",    required_argument, 0, 'w'},
                        {"monitor",    required_argument, 0, 'm'},
                        {"profile",    required_argument, 0, 'p'},
                        {0, 0, 0, 0}
                };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "vhlca:f:F:s:d:m:w:p:",
                         long_options, &option_index);


        if (c == -1)
            break;

        switch (c) {
            case 'v':
                verbose_flag = 1;
                break;
            case 'h':
                print_help_arg = 1;
                break;
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
            case 'f':
                switch_profile = 1;
                new_config_file = optarg;
                break;
            case 'F':
                switch_profile = 1;
                save_new_config_file = 1;
                new_config_file = optarg;
                break;
            case 'd':
                delete_profile_arg = 1;
                delete_profile_name = optarg;
                break;
            case 'm':
                monitor = atoi(optarg);
                if(monitor == 0){
                    printf("Invalid monitor set, defaulting to 1\n");
                    monitor = 1;
                }
                if(verbose_flag){
                    printf("Got monitor arg: %d\n",monitor);
                }
                break;
            case 'p':
                profile_to_modify = optarg;
                if(verbose_flag){
                    printf("Got profile modify arg: %s\n",profile_to_modify);
                }
                break;
            case 'w':
                set_new_wall_path = 1;
                wall_path = optarg;
                if(verbose_flag){
                    printf("Got wallpaper path arg: %s\n",wall_path);
                }
                break;
            case '?':
                if (optopt == 'f')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'a')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'F')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 's')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'd')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'm')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'p')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
                return 1;
            default:
                continue;
        }
    }

    if(print_help_arg){
        print_help();
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

    //Make modifications before setting

    if(set_new_wall_path){
        int modify_current = 0;
        //Default to current
        struct wallpaper modify_me = config.current;

        if(verbose_flag)
            printf("Setting new wallpaper path\n");

        //Do some checks ahead of time
        if(profile_to_modify == NULL){
            if(verbose_flag){
                printf("No profile specified, using current");
            }
            modify_current = 1;
        }else{
            modify_current = 0;
            modify_me = get_wallpaper(profile_to_modify);
            if(strncmp(modify_me.name,"NOTHINGFOUND",256) == 0){
                printf("Could not find specified profile, modifying current instead\n");
                modify_current = 1;
                modify_me = config.current;
            }
        }

        if(verbose_flag){
            printf("Modifying wallpaper: %s\n",modify_me.name);
        }

        //Is the one we are modifying the same as current?
        //Cause if it is we want to modify current too
        //But only if they are exactly the same
        int modifying_currently_applied = 0;
        if(!modify_current){
            //Only check if we have a new one
            modifying_currently_applied = wallpapers_equal(config.current,modify_me);
        }

        set_wallpaper_path(modify_current,profile_to_modify,monitor,wall_path);

        if(modifying_currently_applied){
            printf("Modifying currently applied profile\n");
            set_wallpaper_path(1,profile_to_modify,monitor,wall_path);
        }

        if(modify_current || modifying_currently_applied){
            //Re-apply current with changes
            set_current = 1;
        }
        save_profile_config(config);
    }



    //These are mutually exclusive. Highest priority is setting a new profile
    if(set){
        struct wallpaper apply_profile = get_wallpaper(profile_to_apply);
        if(verbose_flag)
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


    //save_profile_config(config);

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
        printf("Could not find profile to modify\n");
        struct wallpaper dummy;
        strncpy(dummy.name,"NOTHINGFOUND",256);
        return  dummy;
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

int set_wallpaper_path(int current, char* profile_name, int mon_num, char* wall_path){

    //If current, JUST modify the current one
    if(current){

        if(mon_num > config.current.mon_num + 1){
            //Higher than adding an extra
            printf("Monitor number out of range, adding one extra path\n");
            config.current.mon_num += 1;
            vector_push_back(config.current.paths,wall_path);
            return 1;
        }else if(mon_num > config.current.mon_num){
            printf("Adding an extra monitor path\n");
            config.current.mon_num += 1;
            vector_push_back(config.current.paths,wall_path);
        }else{
            if(verbose_flag){
                printf("Replacing %s with %s\n",config.current.paths[mon_num-1],wall_path);
            }
            strncpy(config.current.paths[mon_num-1],wall_path,256);
        }


    }else{

        //We need to find the index of the profile we want to change

        int index = -1;
        for(int idx=0;idx<vector_size(config.wallpaper_list);idx++){
            if(strncmp(config.wallpaper_list[idx].name,profile_name,256)==0){
                index = idx;
                break;
            }
        }

        if(index == -1){
            printf("Unable to find profile\n");
            return 0;
        }

        //Index is now what we are looking for!

        if(mon_num > config.wallpaper_list[index].mon_num + 1){
            //Higher than adding an extra
            printf("Monitor number out of range, adding one extra path\n");
            config.wallpaper_list[index].mon_num += 1;
            vector_push_back(config.wallpaper_list[index].paths,wall_path);
            return 1;
        }else if(mon_num > config.wallpaper_list[index].mon_num){
            printf("Adding an extra monitor path\n");
            config.wallpaper_list[index].mon_num += 1;
            vector_push_back(config.wallpaper_list[index].paths,wall_path);
        }else{
            if(verbose_flag){
                printf("Replacing %s with %s\n",config.wallpaper_list[index].paths[mon_num-1],wall_path);
            }
            strncpy(config.wallpaper_list[index].paths[mon_num-1],wall_path,256);
        }


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
    printf(" -l --list - list profiles\n");
    printf(" -a --apply - apply a profile (Does not set as current)\n");
    printf(" -s --set - apply a profile and set as current\n");
    printf(" -c --current - apply currently set profile\n");
    printf(" -f --temp-config - set the config file to use for the command\n");
    printf(" -F --set-config - set the default config file to use\n");
    printf(" -d --delete - delete a profile\n");
    printf(" -m --monitor - set the monitor to modify\n");
    printf(" -p --profile - set the profile to modify\n");
    printf(" -w --wallpaper - set the path to a wallpaper\n"
                   "\tIf no profile is set, this will change only your current profile\n");


}