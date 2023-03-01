/**
 * @file pingpong.c
 * @author Tommy Shen(10215501403@stu.ecnu.edu.cn)
 * @brief use pipe() to pass info between procs
 * @date 2023.2.28
*/

// Linux pipe manual:
// pipe() creates a pipe, a unidirectional data channel that can be
//        used for interprocess communication.  The array pipefd is used to
//        return two file descriptors referring to the ends of the pipe.
//        pipefd[0] refers to the read end of the pipe.  pipefd[1] refers
//        to the write end of the pipe.  Data written to the write end of
//        the pipe is buffered by the kernel until it is read from the read
//        end of the pipe.  For further details, see pipe(7).

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc) {
    if (argc != 1) {
        printf("Usage: pingpong\n");
        exit(1);
    }

    int p_to_child[2];
    int p_to_parent[2];
    pipe(p_to_child);
    pipe(p_to_parent);

    int cpid = fork();
    /* check fork result first */
    if (cpid == -1) {
        fprintf(2, "Fork error\n");
        exit(1);
    }

    if (cpid == 0) {
        // Close unused p_to_child write end and p_to_parent read end
        close(p_to_child[1]);
        close(p_to_parent[0]);

        // Recv message from parent, then send message to it
        char child_buf[128];
        read(p_to_child[0], &child_buf, 4);
        printf("%d: received %s\n", getpid(), child_buf);
        close(p_to_child[0]);

        write(p_to_parent[1], "pong", 5);
        close(p_to_parent[1]);

        exit(0);
    } else {
        // Close unused p_to_child read end
        close(p_to_child[0]);
        // Close unused p_to_parend write end
        close(p_to_parent[1]);
        // Send message, wait child proc finish, recv message
        write(p_to_child[1], "ping", 4);
        close(p_to_child[1]);

        if (wait(0) == -1) {
            fprintf(2, "Wait error\n");
            exit(1);
        }

        char parent_buf[128];
        read(p_to_parent[0], &parent_buf, 4);
        printf("%d: received %s\n", getpid(), parent_buf);
        close(p_to_parent[0]);

        exit(0);
    }
}
