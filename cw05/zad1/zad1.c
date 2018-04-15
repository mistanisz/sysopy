#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int exec_(char *args, int *in, int *out){
    char **ap, *t[10], *input, *tofree;
    input = tofree = strdup(args);

    //below code from: http://www.manpagez.com/man/3/strsep/
    for(ap = t; (*ap = strsep(&input, " ")) != NULL;)
        if (**ap != '\0')
            if( ++ap >= &t[10])
                break;

    pid_t pid = fork();
    if(pid == 0){
        close(out[0]);
        close(in[1]);
        dup2(out[1], STDOUT_FILENO);
        dup2(in[0], STDIN_FILENO);
        execvp(t[0], t);
    }
    free(tofree);
    return 0;
}


int split_(char * args){
    char **ap, *t[10], *input, *tofree;
    input = tofree = strdup(args);
    int in[2], out[2];
    pipe(in);
    pipe(out);

    //below code from: http://www.manpagez.com/man/3/strsep/
    for(ap = t; (*ap = strsep(&input, "|")) != NULL;)
        if (**ap != '\0')
            if( ++ap >= &t[10])
                break;

    char **l;
    for(l=t;l<ap;++l){
        exec_(*l, in, out);
        close(in[0]);
        close(in[1]);
        in[0]=out[0];
        in[1]=out[1];
        pipe(out);
    }
    close(out[0]);
    close(out[1]);
    close(in[1]);
    char c;
    while (read(in[0], &c, 1) > 0){
        printf("%c", c);
    }
    return 0;
}


int main(int argc, char *argv[]){
    if(argc<1){
        printf("Not enough arguments\n");
        return -1;
    }
    FILE *file = fopen(argv[1], "r");
    char *line = NULL;
    size_t len = 0, read;
    int res;
    while ((read = getline(&line, &len, file)) != -1){
        line[read-1] = '\0';
        printf("\nExecuting: %s\n", line);
        res = split_(line);
        if(res){
            printf("Program %s exited with status code %d.\n", line, res);
            break;
        }
    }
    free(line);
    fclose(file);
}
