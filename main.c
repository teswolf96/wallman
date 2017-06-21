#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUM_PROFILES 10

int load_profiles();
int list_profiles();
int save_profiles();
int set_path(char profile_name[], int mon_num, char path[]);

struct wallpaper{
    char name[50]; //Profile name
    int mon_num; //Number of monitors
    char paths[3][256]; //Number of paths
};

struct wallpaper profiles[NUM_PROFILES];
int num_profiles;

int main(int argc, char **argv) {

    if(argc < 2){
        printf("Usage: \n");
        printf("wallman list - list profiles\n");
        printf("wallman set profile_name monitor_num wallpaper_path\n");
        printf("wallman profile_name - set profile\n");
        printf("Profiles can be set up in ~/.config/wallman\n");
        printf("Example config:\n");
        printf("---------------\n");
        printf("Profile_Name:\n");
        printf("\tmonitors:2\n");
        printf("\t/path/to/wallpaper1.jpg\n");
        printf("\t/path/to/wallpaper2.png\n");
        printf("\n");
        printf("Profile_Name_2:\n");
        printf("\tmonitors:2\n");
        printf("\t/path/to/wallpaper1.jpg\n");
        printf("\t/path/to/wallpaper2.png\n");
        return 0;
    }

    num_profiles = 0;
    for(int idx=0;idx<10;idx++){
        strcpy(profiles[idx].name,"undefined name");

    }

    int load_status = load_profiles();
    if(load_status == -1){
        return 0;
    }

    if(strcmp(argv[1],"list") == 0) {
        list_profiles();

    }else if(strcmp(argv[1],"set") == 0){
        if(argc < 5){
            printf("Not enough arguments\n");
            return 0;
        }
        int mon_num = atoi(argv[3]);
        set_path(argv[2],mon_num,argv[4]);
        save_profiles();
    }else /*if(strcmp(argv[1],"apply") == 0) */{
        if(argc < 2){
            printf("Not enough arguments\n");
            return 0;
        }

        int conv = atoi(argv[1]);
        if(conv > 0) { /* User passed in an integer */
            if(conv > num_profiles){
                printf("No matching profiles\n");
                return 0;
            }

            char command[1024] = "feh";
            for(int idx=0;idx<profiles[conv-1].mon_num;idx++){
                strcat(command," --bg-scale \'");
                strcat(command,profiles[conv-1].paths[idx]);
                strcat(command,"\'");
            }

            strcat(command, " > /dev/null 2>&1" ); /* Hide any errors because it looks better */
            //printf("Command to apply: %s\n",command);
            system(command);

        }else if(conv == 0){ /* User passed in a string or is trying to be tricky >_> */
            conv = -1;
            //printf("Word detected\n");
            for(int idx=0;idx<num_profiles;idx++){
                //printf("Comparing %s to %s",argv[2],profiles[idx].name);
                if(strcmp(argv[1],profiles[idx].name) == 0){
                    conv = idx;
                    break;
                }
            }
            if(conv == -1){
                printf("No matching profiles\n");
                return 0;
            }

            char command[1024] = "feh";
            for(int idx=0;idx<profiles[conv].mon_num;idx++){
                strcat(command," --bg-scale \'");
                strcat(command,profiles[conv].paths[idx]);
                strcat(command,"\'");
            }

            strcat(command, " > /dev/null 2>&1" ); /* Hide any errors because it looks better */
            //printf("Command to apply: %s\n",command);
            system(command);

        }else{
            printf("No matching profiles\n");
        }

    }


    return 0;

}

int list_profiles(){
    //printf("Number of profiles loaded: %d\n", num_profiles);
    printf("Loaded Profiles:\n");
    for(int idx=0;idx<num_profiles;idx++){
        printf("%d) %s\n",idx+1,profiles[idx].name);
        printf("\tMonitor Count: %d\n",profiles[idx].mon_num);
        for(int path_idx=0;path_idx<profiles[idx].mon_num;path_idx++){
            printf("\t%s\n",profiles[idx].paths[path_idx]);
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

    int config_read = 0;
    while (!config_read){
        struct wallpaper new_wallpaper;

        if(!fgets(line, sizeof(line), config)){
            //printf("Trailing whitespace at end of file");
            return 0;
        }
        char *mode_name;
        mode_name = strtok(line, ":");
        //printf("Mode name: %s\n", mode_name);
        strcpy(new_wallpaper.name,mode_name);

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
            strcpy(new_wallpaper.paths[wall_number],line);
        }
        //printf("\n");
        if (!fgets(line, sizeof(line), config)) {
            config_read = 1;
        }

        strcpy(profiles[num_profiles].name,new_wallpaper.name);
        profiles[num_profiles].mon_num = new_wallpaper.mon_num;
        for(int idx =0;idx<new_wallpaper.mon_num;idx++){
            strcpy(profiles[num_profiles].paths[idx],new_wallpaper.paths[idx]);
        }
        num_profiles++;
    }

    fclose(config);

    return 0;
}

int save_profiles(){
    printf("Saving profiles\n");
    //char *fileName = strcat(getenv("HOME"), "/.config/wallman");
    char* fileName = getenv("HOME");
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
    int profile_idx = 0;
    while(profile_idx < NUM_PROFILES && strcmp(profiles[profile_idx].name,"undefined name") != 0){
        char* title = strcat(profiles[profile_idx].name,":\n");
        fprintf(config,title);
        char monitor_count_str[10] = "";
        sprintf(monitor_count_str,"    monitors: %d\n",profiles[profile_idx].mon_num);
        fprintf(config,monitor_count_str);
        for(int idx=0;idx<profiles[profile_idx].mon_num;idx++){
            char path[500] = "";
            sprintf(path,"    %s\n",profiles[profile_idx].paths[idx]);
            fprintf(config,path);
        }
        fprintf(config,"\n");
        profile_idx++;
    }


    fclose(config);

}

int set_path(char profile_name[80], int mon_num, char path[160]){
    printf("Setting profile: %s on monitor %d to path %s\n",profile_name,mon_num,path);
    int conv = -1;
    //printf("Word detected\n");
    for(int idx=0;idx<num_profiles;idx++){
        //printf("Comparing %s to %s",argv[2],profiles[idx].name);
        if(strcmp(profile_name,profiles[idx].name) == 0){
            conv = idx;
            break;
        }
    }
    if(conv == -1){
        printf("No matching profiles\n");
        return 0;
    }

    printf("Identified profile index as %d\n",conv);

    if(mon_num > profiles[conv].mon_num || mon_num <= 0){
        printf("Invalid monitor selection\n");
        return 0;
    }

    strcpy(profiles[conv].paths[mon_num-1],path);
    return 0;
}