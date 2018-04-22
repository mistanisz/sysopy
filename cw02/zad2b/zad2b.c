
/*
 *  Author: Michal Stanisz
 *  Date: 19.03.2018
 *
 */


#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

time_t date;
char type;

int list_dir(const char *path, const struct stat *fileStat, 
             int flag, struct FTW *ftwbuf){
    if(flag == FTW_SL) return 0; 
    time_t time = fileStat->st_mtime;

    switch(type){
        case '=':
            if((time < date) || (time > date + 86400)) return 0;
            
            break;
        case '<':
            if(time >= date) return 0;
            break;
        case '>':
            if(time <= (date + 86400)) return 0;
            break;
    }
    printf("%s\n", path);
    printf( (fileStat->st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat->st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat->st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat->st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat->st_mode & S_IXOTH) ? "x" : "-");
    struct tm *mtime = gmtime(&time);
    char *buf = calloc(80, sizeof(char));
    strftime(buf, 80, "%Y %b %d %H:%M", mtime);
    printf("\t%s", buf); 
    printf("\t%ld bytes", fileStat->st_size);
    free(buf);
    printf("\n\n");
    return 0;
}

int main(int argc, char *argv[]){
    if(argc<4)return 1;
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    if(strptime(argv[2], "%Y-%m-%d", &tm))
        date = mktime(&tm);
    else {
        printf("Bad date format\n");
        return 1;
    }
    char *dir_name = realpath(argv[1], NULL);
    printf("%s\n",dir_name);
    int flags = FTW_PHYS;
    type = argv[3][0];
    nftw(dir_name, list_dir, 20, flags);
    return 0;
}
