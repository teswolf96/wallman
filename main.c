#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_PROFILES 200 //Does anyone really need more than 200 profiles?

int load_profiles();

int list_profiles();

int save_profiles();

int set_profile(char *profile_name);

int set_path(char profile_name[], int mon_num, char path[]);

int set_profile_disp_name(char* profile_name, char* disp_name);

struct wallpaper {
    char name[50]; //Profile name
    char disp_name[50];
    int mon_num; //Number of monitors
    char paths[3][256]; //Number of paths
};

struct wallpaper get_wallpaper(char *profile_name);

int set_profile_temp(struct wallpaper temp);

struct wallpaper profiles[NUM_PROFILES];
char curr_wallpaper[256];
int num_profiles;

int main(int argc, char **argv) {

    /*
     * Yeah I know, that if statement is terrible. Deal with it.
     */
    if (argc < 2 || strcmp(argv[1],"help")==0 ) {
        printf("Usage: \n");
        printf("wallman list - list profiles\n");
        printf("wallman current - apply currently set profile\n");
        printf("wallman set profile_name monitor_num wallpaper_path - change a single wallpaper in a profile\n");
        printf("wallman set monitor_num wallpaper_path - sets a wallpaper temporarily\n");
        printf("wallman profile_name - set profile\n");
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
        return 0;
    }


    num_profiles = 0;
    for (int idx = 0; idx < NUM_PROFILES; idx++) {
        strcpy(profiles[idx].name, "undefined name");

    }

    int load_status = load_profiles();
    if (load_status == -1) {
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        list_profiles();

    } else if (strcmp(argv[1], "set") == 0) {

        if (argc < 4) {
            printf("Not enough arguments\n");
            return 0;
        } else if (argc == 4) {

            struct wallpaper temp = get_wallpaper(curr_wallpaper);

            int monitor = atoi(argv[2]);
            monitor--; //Correct indexing because reasons
            //printf("Applying to monitor: %d\n",monitor);
            char path[256];
            strcpy(path, argv[3]);
            strcpy(temp.paths[monitor], path);

            set_profile_temp(temp);


        } else if (argc == 5) {
            int mon_num = atoi(argv[3]);
            set_path(argv[2], mon_num, argv[4]);
            save_profiles();
        }

    } else if (strcmp(argv[1], "current") == 0) {
        set_profile(curr_wallpaper);
    } else if (strcmp(argv[1], "display") == 0){
        if(argc < 4){
            printf("Not enough arguments");
            return 0;
        }
        set_profile_disp_name(argv[2],argv[3]);
    } else /*if(strcmp(argv[1],"apply") == 0) */{
        if (argc < 2) {
            printf("Not enough arguments\n");
            return 0;
        }

        set_profile(argv[1]);
    }

    return 0;

}

struct wallpaper get_wallpaper(char *profile_name) {
    int conv = -1;

    for (int idx = 0; idx < num_profiles; idx++) {
        //printf("Comparing %s to %s",argv[2],profiles[idx].name);
        if (strcmp(profile_name, profiles[idx].name) == 0) {
            conv = idx;
            break;
        }
    }
    if (conv == -1) {
        printf("Could not find current profile\n");
        exit(0);
    }

    //printf("Current wallpaper is: %s\n",profiles[conv].name);
    return profiles[conv];

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

int set_profile_disp_name(char* profile_name, char* disp_name){
    printf("Let's change the display profile\n");
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
            if (strcmp(profile_name, profiles[idx].name) == 0) {
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

    strcpy(profiles[conv].disp_name,disp_name);

    save_profiles();

}


int set_profile(char *profile_name) {
    int conv = atoi(profile_name);
    if (conv > 0) { /* User passed in an integer */
        if (conv > num_profiles) {
            printf("No matching profiles\n");
            return 0;
        }
        strcpy(curr_wallpaper, profile_name);
        char command[1024] = "feh";
        for (int idx = 0; idx < profiles[conv - 1].mon_num; idx++) {
            strcat(command, " --bg-scale \'");
            strcat(command, profiles[conv - 1].paths[idx]);
            strcat(command, "\'");
        }

        strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
        //printf("Command to apply: %s\n",command);
        system(command);

    } else if (conv == 0) { /* User passed in a string or is trying to be tricky >_> */
        conv = -1;
        //printf("Word detected\n");
        for (int idx = 0; idx < num_profiles; idx++) {
            //printf("Comparing %s to %s",argv[2],profiles[idx].name);
            if (strcmp(profile_name, profiles[idx].name) == 0) {
                conv = idx;
                break;
            }
        }
        if (conv == -1) {
            printf("No matching profiles\n");
            return 0;
        }
        strcpy(curr_wallpaper, profile_name);
        char command[1024] = "feh";
        for (int idx = 0; idx < profiles[conv].mon_num; idx++) {
            strcat(command, " --bg-scale \'");
            strcat(command, profiles[conv].paths[idx]);
            strcat(command, "\'");
        }

        strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
        //printf("Command to apply: %s\n",command);
        system(command);

    } else {
        printf("No matching profiles\n");
    }
    save_profiles();
}

int list_profiles() {
    //printf("Number of profiles loaded: %d\n", num_profiles);
    printf("Current profile: %s\n", curr_wallpaper);
    printf("Loaded Profiles:\n");
    for (int idx = 0; idx < num_profiles; idx++) {
        printf("%d) %s - %s\n", idx + 1, profiles[idx].name, profiles[idx].disp_name);
        printf("\tMonitor Count: %d\n", profiles[idx].mon_num);
        for (int path_idx = 0; path_idx < profiles[idx].mon_num; path_idx++) {
            printf("\t%s\n", profiles[idx].paths[path_idx]);
        }
        printf("\n");
    }
    return 0;
}

int load_profiles() {

    char *fileName = strcat(getenv("HOME"), "/.config/wallman");
    FILE *config = fopen(fileName, "r"); /* should check the result */

    if (config == NULL) {
        fileName = "/etc/wallman";
        config = fopen(fileName, "r");
        if (config == NULL) {
            printf("File could not be opened\n");
            return -1;
        }
    }

    char line[256];

    fgets(line, sizeof(line), config);
    char *curr_wallpaper_found = strtok(line, ":");
    curr_wallpaper_found = strtok(NULL, ":");
    for (size_t i = 0, j = 0; curr_wallpaper_found[j] = curr_wallpaper_found[i]; j += !isspace(
            curr_wallpaper_found[i++])); //Trim whitespace
    strcpy(curr_wallpaper, curr_wallpaper_found);
    //printf("Found current wallpaper: %s\n",curr_wallpaper);

    int config_read = 0;
    while (!config_read) {
        struct wallpaper new_wallpaper;

        if (!fgets(line, sizeof(line), config)) {
            //printf("Trailing whitespace at end of file");
            return 0;
        }

        char *mode_name;
        char *disp_name;

        mode_name = strtok(line, ":");
        //printf("Mode name: %s\n", mode_name);
        disp_name = strtok (NULL, ":");
        for (size_t i = 0, j = 0; disp_name[j] = disp_name[i]; j += !isspace(
                disp_name[i++])); //Trim whitespace

        if(strcmp(disp_name,"")){
            //printf("%s\n", disp_name);
            strcpy(new_wallpaper.disp_name, disp_name);
        }else{
            strcpy(new_wallpaper.disp_name, mode_name);
        }

        strcpy(new_wallpaper.name, mode_name);

        fgets(line, sizeof(line), config);
        char *monitor_count = strtok(line, ":");
        monitor_count = strtok(NULL, ":");
        int num_monitors = atoi(monitor_count);
        //printf("Number of monitors: %d\n", num_monitors);
        new_wallpaper.mon_num = num_monitors;

        for (int wall_number = 0; wall_number < num_monitors; wall_number++) {
            fgets(line, sizeof(line), config);
            for (size_t i = 0, j = 0; line[j] = line[i]; j += !isspace(
                    line[i++])); /* Magic from a guy on SO, trims whitespace */
            //printf("Wallpaper %d: %s\n", wall_number, line);
            strcpy(new_wallpaper.paths[wall_number], line);
        }
        //printf("\n");
        if (!fgets(line, sizeof(line), config)) {
            config_read = 1;
        }

        strcpy(profiles[num_profiles].name, new_wallpaper.name);
        strcpy(profiles[num_profiles].disp_name,new_wallpaper.disp_name);
        profiles[num_profiles].mon_num = new_wallpaper.mon_num;
        for (int idx = 0; idx < new_wallpaper.mon_num; idx++) {
            strcpy(profiles[num_profiles].paths[idx], new_wallpaper.paths[idx]);
        }
        num_profiles++;
    }

    fclose(config);

    return 0;
}

int save_profiles() {
    printf("Saving profiles\n");
    //char *fileName = strcat(getenv("HOME"), "/.config/wallman");
    char *fileName = getenv("HOME");
    //printf("hi! %s - :-)\n",fileName);
    FILE *config = fopen(fileName, "w+"); /* should check the result */

    if (config == NULL) {
        fileName = "/etc/wallman";
        config = fopen(fileName, "r");
        if (config == NULL) {
            printf("File could not be opened\n");
            return -1;
        }
    }
    //fprintf(config,"meow!\n");
    //fputs("meow2!\n",config);
    char current[100] = "";
    sprintf(current, "current: %s\n", curr_wallpaper);
    fprintf(config, current);

    int profile_idx = 0;
    while (profile_idx < NUM_PROFILES && strcmp(profiles[profile_idx].name, "undefined name") != 0) {
        char name_bak[50];
        strcpy(name_bak,profiles[profile_idx].name);
        char *title = strcat(profiles[profile_idx].name, ":");
        if( strcmp(profiles[profile_idx].disp_name,name_bak) == 0){
            /* They are the same */
            title = strcat(name_bak, ":\n");
        }else{
            //printf("%s != %s\n",profiles[profile_idx].disp_name,profiles[profile_idx].name);
            title = strcat(title," ");
            title = strcat(title,profiles[profile_idx].disp_name);
            title = strcat(title,"\n");
        }
        fprintf(config, title);
        char monitor_count_str[10] = "";
        sprintf(monitor_count_str, "    monitors: %d\n", profiles[profile_idx].mon_num);
        fprintf(config, monitor_count_str);
        for (int idx = 0; idx < profiles[profile_idx].mon_num; idx++) {
            char path[500] = "";
            sprintf(path, "    %s\n", profiles[profile_idx].paths[idx]);
            fprintf(config, path);
        }
        fprintf(config, "\n");
        profile_idx++;
    }


    fclose(config);

    char command[1024] = "export HOME=/home/"; //lucifer && exec wallman-genmenu";
    char* user = getenv("USER");
    strcat(command,user);
    strcat(command," && exec wallman-genmenu;");
    strcat(command, " > /dev/null 2>&1"); /* Hide any errors because it looks better */
    //printf("Command to apply: %s\n", command);
    system(command);

}

int set_path(char profile_name[80], int mon_num, char path[160]) {
    printf("Setting profile: %s on monitor %d to path %s\n", profile_name, mon_num, path);
    int conv = -1;
    //printf("Word detected\n");
    for (int idx = 0; idx < num_profiles; idx++) {
        //printf("Comparing %s to %s",argv[2],profiles[idx].name);
        if (strcmp(profile_name, profiles[idx].name) == 0) {
            conv = idx;
            break;
        }
    }
    if (conv == -1) {
        printf("No matching profiles\n");
        return 0;
    }

    printf("Identified profile index as %d\n", conv);

    if (mon_num > profiles[conv].mon_num || mon_num <= 0) {
        printf("Invalid monitor selection\n");
        return 0;
    }

    strcpy(profiles[conv].paths[mon_num - 1], path);
    return 0;
}