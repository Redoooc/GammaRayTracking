#include "file_read&write.h"

FILE *file;

int createFile(char *file_addr){
    fclose(file);
    file = fopen(file_addr, "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    return 0;
}

int appendOpenFile(char *file_addr){
    fclose(file);
    file = fopen(file_addr, "a");
    if (file == NULL) {
        perror("Error append opening file");
        return -1;
    }
    return 0;
}

int writeFile(char *msg){
    if (file == NULL) {
        perror("File do not open");
        return -1;
    }
    fprintf(file, msg);
    fflush(file); // 立即刷新缓冲区，否则只有下一次写入时才能写入该条指令，而下一条指令又被存入缓冲区
    return 0;
}

int closeFile(){
    fclose(file);
    return 0;
}