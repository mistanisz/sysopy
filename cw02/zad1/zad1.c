/*
 *  Author: Michal Stanisz
 *  Date: 17.03.2018
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/*
 * Generate file filled with records of size bytes
 */
void generate(char *name, int records, int size){
    int i, j;
    char *tmp = calloc(size, sizeof(char));
    FILE *file = fopen(name, "w");
    for(i = 0; i < records; ++i){
        for(j = 0; j < size - 1; ++j){
            tmp[j] = rand() % 70 + 48;
        }
        tmp[size-1] = '\n';
        fwrite(tmp, sizeof(char), size, file);
    }
    fclose(file);
}

int seek_sys(int file, int offset, int whence){
    if(lseek(file, offset, whence) < 0){
        printf("\nUnable to set new position. Aborting.");
        return 1;
    }
    return 0;
}

int seek_lib(FILE *file, int offset, int mode){
    if(fseek(file, offset, mode) != 0){
        printf("\nUnable to set new position. Aborting.");
        return 1;
    }
    return 0;
}

/*
 * Sort file using system functions
 */
int sort_sys(char *name, int records, int size){
    char *tmp1 = calloc(size, sizeof(char)), *tmp2 = calloc(size, sizeof(char));
    int i, j, file;
    file = open(name, O_RDWR);
    for(i = 0; i < records; ++i){
        int min = i;
        if(seek_sys(file, i*size, SEEK_SET)) return 1;
        read(file, tmp1, size);
        for(j = i + 1;j < records; ++j){
            read(file, tmp2, size);
            if(strcmp(tmp1, tmp2)>0){
                min = j;
                strcpy(tmp1, tmp2);
            }
        }
        if(min != i){
            if(seek_sys(file, i*size, SEEK_SET)) return 1;
            read(file, tmp2, size);
            if(seek_sys(file, i*size, SEEK_SET)) return 1;
            write(file, tmp1, size);
            if(seek_sys(file, min*size, SEEK_SET)) return 1;
            write(file, tmp2, size);
        }
    }
    free(tmp1);
    free(tmp2);
    close(file);
    return 0;
}

/*
 * Sort file using library functions
 */
int sort_lib(char *name, int records, int size){
    char *tmp1 = calloc(size, sizeof(char)), *tmp2 = calloc(size, sizeof(char));
    int i, j;
    FILE *file = fopen(name, "r+");
    for(i = 0; i < records; ++i){
        int min = i;
        if(seek_lib(file, i*size, 0)) return 1;
        fread(tmp1, sizeof(char), size, file);
        for(j = i + 1;j < records; ++j){
            fread(tmp2, sizeof(char), size, file);
            if(strcmp(tmp1, tmp2)>0){
                min = j;
                strcpy(tmp1, tmp2);
            }
        }
        if(min != i){
            if(seek_lib(file, i*size, 0)) return 1;
            fread(tmp2, sizeof(char), size, file);
            if(seek_lib(file, i*size, 0)) return 1;
            fwrite(tmp1, sizeof(char), size, file);
            if(seek_lib(file, min*size, 0)) return 1;
            fwrite(tmp2, sizeof(char), size, file);
        }
    }
    free(tmp1);
    free(tmp2);
    fclose(file);
    return 0;
}

/*
 * Copy file using system functions using size buffer
 */
int copy_sys(char *src, char *dest, int records, int size){
    char *tmp = calloc(size, sizeof(char));
    int src_file, dest_file, read_bytes;
    src_file = open(src, O_RDONLY);
    dest_file = open(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR|S_IWUSR);
    while((read_bytes = read(src_file, tmp, size)) > 0)
        write(dest_file, tmp, read_bytes);
    free(tmp);
    close(src_file);
    close(dest_file);
    return 0;
}

/*
 * Copy file using library functions using size buffer
 */
int copy_lib(char *src, char *dest, int records, int size){
    char *tmp = calloc(size, sizeof(char));
    int read_bytes; 
    FILE *dest_file, *src_file;
    src_file = fopen(src, "r");
    dest_file = fopen(dest, "w");
    while((read_bytes = fread(tmp, sizeof(char), size, src_file)) > 0)
        fwrite(tmp, sizeof(char), read_bytes, dest_file);
    free(tmp);
    fclose(src_file);
    fclose(dest_file);
    return 0;
}

/*
 * Prints help
 */
void print_help(char *name){
    printf("Usage: %s generate | sort | copy <args>\n\n"
           "generate <filename> <records> <size>       creates new file named\n"
           "sort <filename> <records> <size> <type>    sorts file <filename>\n"
           "copy <src> <dest> <records> <size> <type>  copies content of <src>" 
           " to <dest>\n\n"
           "    filename                name of file\n"
           "    records                 number of records in file\n"
           "    size                    size of one record\n"
           "    type                    use either system or library funtions, "
           "one of [sys, lib]\n\n",
           name);
}

int main(int argc, char *argv[]){
    if(argc<5){
        print_help(argv[0]);
        return 2;
    }
    char *name = argv[2], *dest, *type;
    int records = atoi(argv[3]);
    int size = atoi(argv[4]);
    switch(argv[1][0]){
        case 'g':
            records = atoi(argv[3]);
            size = atoi(argv[4]);
            printf("Generating file %s\nRecords: %d, Size: %d\n", 
                   name, records, size);
            generate(name, records, size);
            break;           
        case 'c':
            dest = argv[3];
            records = atoi(argv[4]);
            size = atoi(argv[5]);
            type = argv[6];
            printf("Copying file %s to %s\nRecords: %d, Size: %d\n", 
                   name, dest, records, size);
            switch(type[0]){
                case 's':
                    printf("Using system functions\n");
                    return copy_sys(name, dest, records, size);
                case 'l':
                    printf("Using library functions\n");
                    return copy_lib(name, dest, records, size);
                default:
                    print_help(argv[0]);
                    return 2;
            }
            break;           
        case 's':
            records = atoi(argv[3]);
            size = atoi(argv[4]);
            type = argv[5];
            printf("Sorting file %s\nRecords: %d, Size: %d\n",
                   name, records, size);
            switch(type[0]){
                case 's':
                    printf("Using system functions\n");
                    return sort_sys(name, records, size);
                case 'l':
                    printf("Using library functions\n");
                    return sort_lib(name, records, size);
                default:
                    print_help(argv[0]);
                    return 1;
            }
            break;           
        default:
            print_help(argv[0]);
            return 1;
    }
    return 0;
}

