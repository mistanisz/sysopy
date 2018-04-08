#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __USE_XOPEN
#include <time.h>

void list_dir(char *dir_name, time_t date, char type){
    DIR *dir =  opendir(dir_name);
    if(dir == NULL) return;
    struct dirent* dirent = readdir(dir);
    char *path = calloc(500, sizeof(char));
    while(dirent){
        struct stat fileStat;
        strcpy(path, dir_name);
        strcat(path, "/");
        strcat(path, dirent->d_name);
        char *n = dirent->d_name;
        if((strcmp(n, ".") == 0) || (strcmp(n, "..") == 0)){
            dirent = readdir(dir);
            continue; 
        }
        lstat(path, &fileStat);
        
        int is_dir = 0;
        if(S_ISDIR(fileStat.st_mode)) is_dir = 1;
        else if(!S_ISREG(fileStat.st_mode)){
            dirent = readdir(dir);
            continue;
        }
     
        if(is_dir){
            pid_t pid = vfork();
            if(pid == 0){ 
                list_dir(path, date, type);
                exit(0);
            }
        }
        else {
            time_t time = fileStat.st_mtime;
            switch(type){
                case '=':
                    if((time < date) || (time > date + 86400)) {
                        dirent = readdir(dir); 
                        continue;
                    }
                    break;
                case '<':
                    if(time>=date) {dirent = readdir(dir); continue;}
                    break;
                case '>':
                    if(time<=(date + 86400)) {dirent = readdir(dir); continue;}
                    break;
            }
            printf("%s\n", path);
            printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
            struct tm *mtime = gmtime(&time);
            char *buf = calloc(20, sizeof(char));
            strftime(buf, 20, "%Y %b %d %H:%M", mtime);
            printf("\t%s", buf); 
            printf("\tPID: %d", (int)getpid());
            printf("\t%ld bytes", fileStat.st_size);
            free(buf);
            printf("\n\n");
        }
        dirent = readdir(dir);
    }
    free(path);
    closedir(dir);
}

int main(int argc, char *argv[]){
    if(argc<4)return 1;
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    time_t date;
    printf("%s\n", argv[3]);
    if(strptime(argv[3], "%Y-%m-%d", &tm))
        date = mktime(&tm);
    else {
        printf("Bad date format\n");
        return 1;
    }
    char *dir_name = realpath(argv[1], NULL);
    list_dir(dir_name, date, argv[2][0]);
    free(dir_name);
    return 0;
}
