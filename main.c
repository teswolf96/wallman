#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "config_parse.h"
#include <unistd.h>
#include <getopt.h>

#include "config_save.h"

//#define NUM_PROFILES 200 //Does anyone really need more than 200 profiles?

int list_profiles();

int set_profile(struct wallpaper profile);

int delete_profile(struct wallpaper delete_me);

struct wallpaper get_wallpaper(char *profile_name);

int set_wallpaper_path(int current, char* profile_name, int mon_num, char* wall_path);

int set_hidden(int current, char* profile_name, int hidden);

int set_title_func(int current, char* profile_name, char* title);

int set_category_func(int current, char* profile_name, char* category);

int create_new_profile(char* name, char* title, char* category, int hidden, char** paths);

int wallpapers_equal(struct wallpaper wall1, struct wallpaper wall2);

void print_help();

int verbose_flag = 0;
int overwrite = 0;

struct Config config;
//struct wallpaper profiles[NUM_PROFILES];
int num_profiles;

int main(int argc, char **argv) {

    config.wallpaper_list = NULL;

    if (argc < 2) {
        print_help();
        return 0;
    }

    int list = 0;
    int set = 0;
    int regen_jgmenu = 0;
    int set_current = 0;
    int switch_profile = 0;
    int save_new_config_file = 0;
    int save_as_current = 0;
    int delete_profile_arg = 0;
    char *profile_to_apply = NULL;
    char *new_config_file = NULL;
    char old_config_file[256];
    char *delete_profile_name = NULL;
    char **wall_path = NULL;
    char *profile_to_modify = NULL;
    int set_new_wall_path = 0;

    int new_profile = 0;
    char* new_profile_name = NULL;

    int set_hidden_arg = 0;
    char* hidden_val = NULL;

    int set_title = 0;
    char* title_val = NULL;

    int set_category = 0;
    char* category_val = NULL;

    int save_as_new_profile_arg = 0;
    int use_current_name = 0;
    char* save_as_new_profile_name = NULL;
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
                        {"jgmenu",     no_argument,       0, 'j'},
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
                        {"new",    required_argument, 0, 'n'},
                        {"hidden",    required_argument, 0, 'H'},
                        {"title",    required_argument, 0, 't'},
                        {"category",    required_argument, 0, 'C'},
                        {"save",    optional_argument, 0, 'S'},
                        {"overwrite",    no_argument, 0, 'O'},
                        {0, 0, 0, 0}
                };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "vjOhlca:f:F:s:d:m:w:p:n:t:C:H:S:",
                         long_options, &option_index);


        if (c == -1)
            break;

        switch (c) {
            case 'v':
                verbose_flag = 1;
                break;
            case 'O':
                overwrite = 1;
                break;
            case 'h':
                print_help_arg = 1;
                break;
            case 'j':
                regen_jgmenu = 1;
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
                vector_push_back(wall_path,optarg);
                if(verbose_flag){
                    printf("Got wallpaper path arg: %s\n",wall_path[vector_size(wall_path)-1]);
                }
                break;
            case 'n':
                new_profile = 1;
                new_profile_name = optarg;
                if(verbose_flag){
                    printf("Creating new profile: %s\n",new_profile_name);
                }
                break;
            case 'H':
                set_hidden_arg = 1;
                hidden_val = optarg;
                if(verbose_flag){
                    printf("Setting hidden to: %s\n",hidden_val);
                }
                break;
            case 't':
                set_title = 1;
                title_val = optarg;
                if(verbose_flag){
                    printf("Setting title to: %s\n",title_val);
                }
                break;
            case 'C':
                set_category = 1;
                category_val = optarg;
                if(verbose_flag){
                    printf("Setting category to: %s\n",category_val);
                }
                break;
            case 'S':
                save_as_new_profile_arg = 1;
                save_as_new_profile_name = optarg;
                if(verbose_flag){
                    printf("Found optional arg: %s\n",save_as_new_profile_name);
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
                else if (optopt == 'n')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'C')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 't')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'H')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (optopt == 'S') {
                    //If saving without passing in a name
                    save_as_new_profile_arg = 1;
                    use_current_name = 1;
                    break;
                }
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
    config = load_profiles();

    //Check if we want to switch profiles
    //If so, do this before loading the profile file
    if(switch_profile){
        printf("Using profile: %s\n",new_config_file);
        if(!save_new_config_file){
            strncpy(old_config_file,config.active_profile,256);
            if(verbose_flag){
                printf("Found temporary config file: %s, storing it for later\n",old_config_file);
            }
        }
        strncpy(config.active_profile,new_config_file,256);
        if(save_new_config_file){
            save_main_config(config);
        }
    }

    //Load the profile file
    config = load_profile_file(config);

    //Creating a new profile is important

    if(new_profile){

        //Check what we have
        if(verbose_flag){
            printf("Creating new profile named %s\n",new_profile_name);
        }

        int hidden = 0;
        if(set_hidden_arg){
            if(strncmp(hidden_val,"true",256) == 0 || strncmp(hidden_val,"True",256) == 0){
                hidden = 1;
                if(verbose_flag){
                    printf("Setting hidden to true\n");
                }
            }
        }
        printf("New profile hidden: %d\n",hidden);

        char* category = "none";
        if(set_category){
            category = category_val;
        }
        printf("New profile category: %s\n",category);

        char* title = "none";
        if(set_title){
            title = title_val;
        }
        printf("New profile title: %s\n",title);

        if(wall_path == NULL){
            printf("Please provide at least one wallpaper for your new profile.\n");
            exit(0);
        }
        create_new_profile(new_profile_name,title,category,hidden,wall_path);
        save_main_config(config);
        save_profile_config(config);

    }


    //Make modifications before setting

    if(set_new_wall_path && !new_profile){
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

        set_wallpaper_path(modify_current,profile_to_modify,monitor,wall_path[0]);

        if(modifying_currently_applied){
            printf("Modifying currently applied profile\n");
            set_wallpaper_path(1,profile_to_modify,monitor,wall_path[0]);
        }

        if(modify_current || modifying_currently_applied){
            //Re-apply current with changes
            set_current = 1;
        }

        if(old_config_file != NULL){
            strncpy(config.active_profile,old_config_file,256);
            printf("At time of saving, the active profile is %s and the old is %s\n",config.active_profile,old_config_file);
        }
        save_main_config(config);
        save_profile_config(config);
    }

    if(set_hidden_arg && !new_profile){

        if(verbose_flag){
            printf("Setting hidden value\n");
        }
        int modify_current = 0;
        //Default to current
        struct wallpaper modify_me = config.current;

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
            printf("Modifying hidden: %s\n",modify_me.name);
        }

        //Is the one we are modifying the same as current?
        //Cause if it is we want to modify current too
        //But only if they are exactly the same
        int modifying_currently_applied = 0;
        if(!modify_current){
            //Only check if we have a new one
            modifying_currently_applied = wallpapers_equal(config.current,modify_me);
        }

        //If they entered nonsense, just set it false
        int hidden_int = 0;
        if(set_hidden_arg){
            if(strncmp(hidden_val,"true",256) == 0 || strncmp(hidden_val,"True",256) == 0){
                hidden_int = 1;
                if(verbose_flag){
                    printf("Setting hidden to true\n");
                }
            }
        }
        if(modifying_currently_applied){
            printf("Modifying currently applied profile\n");
            config.current.hidden = 1;
        }
        set_hidden(modify_current,profile_to_modify,hidden_int);



//        save_main_config(config);
//        save_profile_config(config);

    }

    if(set_title && !new_profile){

        if(verbose_flag){
            printf("Setting title value\n");
        }
        int modify_current = 0;
        //Default to current
        struct wallpaper modify_me = config.current;

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
            printf("Modifying title on: %s\n",modify_me.name);
        }

        //Is the one we are modifying the same as current?
        //Cause if it is we want to modify current too
        //But only if they are exactly the same
        int modifying_currently_applied = 0;
        if(!modify_current){
            //Only check if we have a new one
            modifying_currently_applied = wallpapers_equal(config.current,modify_me);
        }

        //If they entered nonsense, just set it false

        set_title_func(modify_current,profile_to_modify,title_val);

        if(modifying_currently_applied){
            printf("Modifying currently applied profile\n");
            strncpy(config.current.disp_name,title_val,256);
        }

    }

    if(set_category && !new_profile){

        if(verbose_flag){
            printf("Setting category value\n");
        }
        int modify_current = 0;
        //Default to current
        struct wallpaper modify_me = config.current;

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
            printf("Modifying category on: %s\n",modify_me.name);
        }

        //Is the one we are modifying the same as current?
        //Cause if it is we want to modify current too
        //But only if they are exactly the same
        //int modifying_currently_applied = 0;
        if(!modify_current){
            //Only check if we have a new one
            /*modifying_currently_applied = */wallpapers_equal(config.current,modify_me);
        }

        //If they entered nonsense, just set it false

        set_category_func(modify_current,profile_to_modify,category_val);

//        if(modifying_currently_applied){
//            printf("Modifying currently applied profile\n");
//            strncpy(config.current.category,category_val,256);
//        }

    }

    //These are mutually exclusive. Highest priority is setting a new profile
    if(set){
        struct wallpaper apply_profile = get_wallpaper(profile_to_apply);
        if(verbose_flag)
            printf("Setting current profile to %s\n",apply_profile.name);
        config.current = apply_profile;
        set_profile(apply_profile);
        if(save_as_current){
            if(verbose_flag){
                printf("Switch profile is %d and save_new_config_file is %d\n",switch_profile,save_new_config_file);
            }
            if(switch_profile && !save_new_config_file){
                if(verbose_flag){
                    printf("Storing the old config file %s into the active_profile to prevent overwriting\n",old_config_file);
                }
                strncpy(config.active_profile,old_config_file,256);
            }
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

    //Save the current profile after any modifications to it

    if(save_as_new_profile_arg){

        if(!use_current_name){
            strncpy(config.current.name,save_as_new_profile_name,256);
        }
        printf("Calling create new profile\n");
        create_new_profile(config.current.name,config.current.disp_name,config.current.category,
                           config.current.hidden,config.current.paths);
        save_main_config(config);
        save_profile_config(config);
    }


    //save_profile_config(config);

    if(regen_jgmenu){
        printf("Re-generating jgmenu config\n");
        save_profile_config(config);
    }

    //List is ALWAYS last
    if(list){
        //struct Config temp = config;
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
        //printf("Could not find profile to modify\n");
        struct wallpaper dummy;
        strncpy(dummy.name,"NOTHINGFOUND",256);
        return  dummy;
    }

    //printf("Current wallpaper is: %s\n",profiles[conv].name);
    return config.wallpaper_list[conv];

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
    return 0;
}

int set_hidden(int current, char* profile_name, int hidden){
    if(current){
        config.current.hidden = hidden;
        save_main_config(config);

    }else{
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

        if(verbose_flag){
            printf("Set hidden on %s to val %d\n",config.wallpaper_list[index].name,hidden);
        }
        config.wallpaper_list[index].hidden = hidden;
        save_main_config(config);
        save_profile_config(config);
    }
    return 0;
}

int set_title_func(int current, char* profile_name, char* title){
    if(current){
        strncpy(config.current.disp_name,title,256);
        save_main_config(config);

    }else{
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

        if(verbose_flag){
            printf("Set title on %s to val %s\n",config.wallpaper_list[index].name,title);
        }
        strncpy(config.wallpaper_list[index].disp_name,title,256);

        save_main_config(config);
        save_profile_config(config);
    }

    return 0;
}

int set_category_func(int current, char* profile_name, char* category){
    if(current){
        strncpy(config.current.category,category,256);
        save_main_config(config);
    }else{
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

        if(verbose_flag){
            printf("Set category on %s to val %s\n",config.wallpaper_list[index].name,category);
        }
        strncpy(config.wallpaper_list[index].category,category,256);
        save_main_config(config);
        save_profile_config(config);
    }
    return 0;
}

int create_new_profile(char* name, char* title, char* category, int hidden, char** paths){

    struct wallpaper test_existing = get_wallpaper(name);
    //If searching for the wallpaper returns nothing
    if(strncmp(test_existing.name,"NOTHINGFOUND",256) == 0){
        struct wallpaper new;
        strncpy(new.name,name,256);
        strncpy(new.disp_name,title,256);
        strncpy(new.category,category,256);
        new.hidden = hidden;
        new.paths = paths;
        vector_push_back(config.wallpaper_list,new);
    }else{
        if(verbose_flag){
            printf("Found existing profile: %s\n",test_existing.name);
        }
        if(overwrite){
            if(verbose_flag){
                printf("Overwriting existing profile %s\n",name);
            }
            int index = -1;
            for(int idx=0;idx<vector_size(config.wallpaper_list);idx++){
                if(strncmp(config.wallpaper_list[idx].name,name,256)==0){
                    index = idx;
                    break;
                }
            }
            
            strncpy(config.wallpaper_list[index].name,name,256);
            strncpy(config.wallpaper_list[index].disp_name,title,256);
            strncpy(config.wallpaper_list[index].category,category,256);
            config.wallpaper_list[index].hidden = hidden;
            config.wallpaper_list[index].paths = paths;

        }else{
            printf("A profile with that name already exists!\n"
                           "Try using a different name or use the -O flag to overwrite it!\n");
        }
    }
    return 0;

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
    printf(" -p --profile - set the profile to modify\n");
    printf(" -n --new - Create a new profile\n");
    printf(" -t --title - set the profile to modify\n");
    printf(" -C --category - set the profile to modify\n");
    printf(" -H --hidden - set the monitor to modify\n");
    printf(" -w --wallpaper - set the path to a wallpaper\n"
                   "\tIf no profile is set, this will change only your current profile\n");
    printf(" -S --save - save the current profile as a new profile\n");
    printf(" -O --overwrite - when saving, overwrite an existing profile if it exists\n");
    printf(" -j --jgmenu - force regenerate the jgmenu\n");
}