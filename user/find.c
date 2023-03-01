/**
 * @file find.c
 * @author Tommy Shen(10215501403@stu.ecnu.edu.cn)
 * @brief Recursively find file with certain file name
 * @date 2023.2.28
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* path, char* name) {
    // read stats info from the current dir
    int fd;
    struct stat st;
    struct dirent de;

    if ((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        exit(0);
    }
    
    if (fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    char buf[512];
    char *p;
    switch(st.type) {
        case T_DEVICE:
        case T_FILE:
#ifdef FIND_DEBUG
            printf("Finding file name: %s\n", path);
            printf("Target name: %s\n", name);
#endif
            for(p=path+strlen(path); p >= path && *p != '/'; p--)
                ;
            p++;
            if (!strcmp(name, p)) {
                printf("%s\n", path);
            }
            break;
        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
                printf("ls: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf+strlen(buf);
            *p++ = '/';
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0 || !strcmp(de.name, ".") || !strcmp(de.name, ".."))
                    continue;

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if (stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }
                // Find first character after last slash.
                find(buf, name);
            }
            break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: find (path) (name)\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}