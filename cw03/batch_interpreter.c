#include<stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>

int exec_(char *args, int cpu_limit, int mem_limit){
    char **ap, *t[10], *input, *tofree;
    input = tofree = strdup(args);

    //below code from: http://www.manpagez.com/man/3/strsep/
    for(ap = t; (*ap = strsep(&input, " ")) != NULL;)
        if (**ap != '\0')
            if( ++ap >= &t[10])
                break;

    pid_t pid = fork();
    struct rlimit cpu_rlimit = {cpu_limit, cpu_limit};
    struct rlimit mem_rlimit = {mem_limit, mem_limit};
    if(pid == 0){
        setrlimit(RLIMIT_CPU, &cpu_rlimit);
        setrlimit(RLIMIT_AS, &mem_rlimit);
        int res = execvp(t[0], t);
        if(res == -1)exit(-1);
    }
    int res;
    waitpid(pid, &res,0);
    free(tofree);
    return res;
}


int main(int argc, char *argv[]){
    if(argc<3){
        printf("Not enough arguments\n");
        return -1;
    }
    FILE *file = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0, read;
    int res;
    int cpu_limit = atoi(argv[2]), mem_limit = atoi(argv[3]);
    struct timeval sys_begin, sys_end, usr_begin, usr_end, sys_res, usr_res;
    struct rusage usage;
    while ((read = getline(&line, &len, file)) != -1){
        line[read-1] = '\0';
        printf("\nExecuting program: %s\n", line);
        getrusage(RUSAGE_CHILDREN, &usage);
        usr_begin = usage.ru_utime;
        sys_begin = usage.ru_stime;
        res = exec_(line, cpu_limit, 1024 * 1024 * mem_limit);
        getrusage(RUSAGE_CHILDREN, &usage);
        usr_end = usage.ru_utime;
        sys_end = usage.ru_stime;
        timersub(&usr_end, &usr_begin, &usr_res);
        timersub(&sys_end, &sys_begin, &sys_res);
        printf("System time: %ld.%06ld, User time: %ld.%06ld\n", sys_res.tv_sec, sys_res.tv_usec, usr_res.tv_sec, usr_res.tv_usec); 
        if(res){
            printf("Program %s exited with status code %d.\n", line, res);
            break;
        }
    }
    free(line);
    fclose(file);
}
