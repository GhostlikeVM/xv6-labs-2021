/**
 * @file xargs.c
 * @author Tommy Shen(10215501403@stu.ecnu.edu.cn)
 * @brief imply xargs in xv6
 * @date 2023.3.1
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"
#include "kernel/fs.h"

#define XARGS_DEBUG
// #undef XARGS_DEBUG

/**
 * @note if not optimized, just concat string from input to arg[2] after command\
 * If optimized (xargs -n 1 (command) (args...)), set a xarglist to fork child process\
 * for each xarg, then concat it to arg[4]
*/

// static const int kXargsMaxLen = 512;
// static const int kStringMaxLen = 1024;
/**
 * @return (len of result) if correctly return\
 * @return -1 if ret exceed kXargsMaxLen or kStringMaxLen
*/
// static inline int concat(char* ret, char* str1, char* str2) {
//     char* start = ret;
//     if (strlen(str1) + strlen(str2) > kStringMaxLen) {
//         return -1;
//     }

//     for (int i = 0; str1[i]; i++) {
//         *ret = str1[i];
//         ret++;
//     }

//     // add ' ' to end of concat
//     *ret = ' ';
//     ret++;

//     for (int i = 0; str2[i] && str2[i] != '\n'; i++) {
//         *ret = str2[i];
//         ret++;
//     }

//     *ret = '\0';
//     return ret - start;
// }

// static inline void xstrcpy(char* dest, char* res) {
//     for(int i = 0; res[i]; i++) {
// #ifdef XARGS_DEBUG
//         printf("copying char: %c\n", res[i]);
// #endif
//         *(dest++) = res[i];
//     }
// }

// // convert xargs into group
// static inline int parse_xargs(char xargs[32][64]) {
//     char ch;
//     char* xarg = (char *)malloc(kXargsMaxLen);
//     int xargs_index = 0;
//     int xarg_index = 0;

//     int finish_line = 0; // if last char is \n, 0, else 1
//     int aaflag = 0;

//     while(read(0, &ch, 1)) {
// #ifdef XARGS_DEBUG
//         printf("reading input char: %c\n", ch);
// #endif
//         if (ch == '"') {
//             if (aaflag) {
//                 break;
//             }
//             aaflag = 1;
//             continue;
//         }
//         if (ch == '\n') {
//             xstrcpy(xargs[xargs_index++], xarg);
//             xarg_index = 0;
//             memset(xarg, 0, kXargsMaxLen);
//             finish_line = 0;
//         } else if (ch == '\\') {
//             if (!read(0, &ch, 1)) {
//                 break;
//             }
//             if (ch == 'n') {
// #ifdef XARGS_DEBUG
//                 printf("put xarg: %s, addr %x, into xargs[%d], addr %d\n", xarg, &xarg, xargs_index, &xargs[xargs_index]);
// #endif
//                 xstrcpy(xargs[xargs_index++], xarg);
//                 xarg_index = 0;
//                 memset(xarg, 0, kXargsMaxLen);
//                 finish_line = 0;
//             } else {
//                 xarg[xarg_index++] = '\\';
//                 xarg[xarg_index++] = ch;
//                 finish_line = 1;
//             } 
//         } else {
//             xarg[xarg_index++] = ch;
//             finish_line = 1;
//         }
//     }
//     if (finish_line) {
// #ifdef XARGS_DEBUG
//         printf("put xarg: %s, addr %x, into xargs[%d], addr %d\n", xarg, &xarg, xargs_index, &xargs[xargs_index]);
// #endif
//         xstrcpy(xargs[xargs_index++], xarg);
//     }
//     return xargs_index;
// }

// int main(int argc, char* argv[]) {
// #ifdef XARGS_DEBUG
//     printf("xargs argc: %d\n", argc);
//     for (int i = 0; i < argc; i++) {
//         printf("xargs argv[%d]: %s\n", i, argv[i]);
//     }
// #endif

//     // Optimized mode
//     if (!strcmp(argv[1], "-n")) {
//         if (strcmp(argv[2], "1")) {
//             printf("Usage: xargs (optional)(-n 1) <command> <args>\n");
//             exit(1);
//         } else {
//             // char** xargs = (char**)malloc(sizeof(char) * kXargsMaxLen * MAXARG);
//             char xargs[32][64];
//             int xarg_num = parse_xargs(xargs);
// #ifdef XARGS_DEBUG
//             printf("xargnum: %d\n", xarg_num);
//             for (int i = 0; i < xarg_num; i++) {
//                 printf("xargs[%d]: %s\n", i, xargs[i]);
//             }
// #endif
//             if (xarg_num < 0 || xarg_num > MAXARG) {
//                 fprintf(2, "xargs: reading xargs error\n");
//                 exit(1);
//             }
//             for (int i = 0; i < xarg_num; i++) {
//                 char* cat_input = (char *)malloc(kXargsMaxLen * sizeof(char));
//                 if (concat(cat_input, argv[4], xargs[i]) < 0) {
//                     fprintf(2, "xargs: concat error\n");
//                     exit(1);
//                 }
// #ifdef XARGS_DEBUG
//                 printf("concat result: %s\n", cat_input);
// #endif
//                 char* command = argv[3];
//                 if (fork() == 0) {
//                     char* child_argv[3];
//                     child_argv[0] = command;
//                     child_argv[1] = cat_input;
//                     child_argv[2] = 0;
//                     exec(command, child_argv);
//                     exit(0);
//                 } else {
//                     wait(0);
//                 }
//             }
//         }
//     } else if (argc == 3) { // Normal mode
//         char buf[512];
//         int read_ret;
//         if ((read_ret = read(0, buf, sizeof(buf))) < 0) {
//             fprintf(2, "xargs: read error\n");
//             exit(1);
//         }
//         if (read_ret > kXargsMaxLen) {
//             fprintf(2, "xargs: xargs too long\n");
//             exit(1);
//         }
// #ifdef XARGS_DEBUG
//         printf("xargs read input:(%s)\nlen = %d\n", buf, strlen(buf));
// #endif
//         // concat xargs to arg[2]
//         char* cat_input = (char *)malloc(4 + strlen(argv[2]) + read_ret);

//         int concat_res = concat(cat_input, argv[2], buf);
//         if (concat_res < 0 || concat_res > kStringMaxLen) {
//             fprintf(1, "xargs: concat error\n");
//             exit(1);
//         }
// #ifdef XARGS_DEBUG
//         printf("concat result: %s\n", cat_input);
// #endif
//         int pipe_to_exec[2];
//         pipe(pipe_to_exec);

//         char* command = argv[1];
//         if (fork() == 0) {
//             close(pipe_to_exec[1]);

//             char read_buf[1024];
//             read(pipe_to_exec[0], read_buf, concat_res);
// #ifdef XARGS_DEBUG
//             printf("READY to exec: command: (%s), args: (%s)\n", command, read_buf);
// #endif
//             char* child_argv[3];
//             child_argv[0] = command;
//             child_argv[1] = read_buf;
//             child_argv[2] = 0;
//             exec(command, child_argv);
            
//             close(pipe_to_exec[0]);
//             exit(0);
//         } else {
//             close(pipe_to_exec[0]);
//             write(pipe_to_exec[1], cat_input, concat_res);
//             close(pipe_to_exec[1]);

//             // Wait child process to finish exec
//             wait(0);
//         }
//     } else {
//         printf("Usage: xargs (optional)(-n 1) <command> <args>\n");
//         exit(1);
//     }
//     exit(0);
// }
#define MAXLINE 32
int
main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(2, "usage: xargs <command> <argu>");
        exit(1);
    }
    char *cmd = argv[1];
    char line[MAXLINE];
    memset(line, 0, sizeof line);

    int i = 0;
    char ch;
    while(read(0, &ch, sizeof (char))) {
        // read individual lines of input, read a character at a time
        // until a newline ('\n') appears.
        if (ch == '\n') {
            char *child_argv[4];
            child_argv[0] = cmd;
            child_argv[1] = argv[2];
            child_argv[2] = line;
            // don't forget this
            child_argv[3] = 0;
            // child process
            if (fork() == 0) {
                exec(cmd, child_argv);
            }
            // use wait in the parent to wait for the child to complete the command
            else {
                wait(0);
            }
            memset(line, 0, sizeof line);
            i = 0;
        }
        else {
            line[i++] = ch;
        }
    } 
    exit(0);
}