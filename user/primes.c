/**
 * @file primes.c
 * @author Tommy Shen(10215501403@stu.ecnu.edu.cn)
 * @brief Write a concurrent version of prime sieve using pipes.
 * @date 2023.2.28
*/

// Your goal is to use pipe and fork to set up the pipeline. 
// The first process feeds the numbers 2 through 35 into the pipeline. 
// For each prime number, you will arrange to create one process 
// that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. 
// Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/**
 * @return 1: number is prime\
 * @return 0: number is not prime
*/
static inline int is_prime(int number) {
    if (number == 2) {
        return 1;
    }
    for (int i = 2; i < number; i++) {
        if (i*i > number) {
            break;
        }
        if ((number / i) * i == number) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief Write int into pipe (cast to 4 bytes)
*/
static inline void write_int_to_pipe(int pipe_end, int number) {
    // Cast int into char* for writing into pipe
    char num_to_char[4];
    int temp_num = number;
    for (int i = 0; i < 4; i++) {
        num_to_char[i] = (char) (temp_num & 0xff);
        temp_num >>= 8;
    }
    write(pipe_end, num_to_char, 4);
}

/**
 * @brief Read int from pipe, 
 * 
 * @return 0 if pipe closed\
 * @return -1 if number > 32\
 * @return (number) if number is valid (number > 0 && number < 32)\
*/
static inline int read_int_from_pipe(int pipe_end) {
    char buf[8];
    if (read(pipe_end, &buf, 4) == 0) {
        return 0;
    }
    int read_num = *(int *)buf;
    if (read_num > 35 || read_num < 1) {
        return -1;
    }
    return read_num;
}

int main(int argc) {
    if (argc != 1) {
        printf("Usage: primes\n");
        exit(1);
    }

    int prime_pipe[2];
    pipe(prime_pipe);
    // allocate space for pipelines
    if (fork() == 0) {
        close(prime_pipe[0]); // Close prime pipe read end in child/grandchild process
        // for each number, write to pipeline
        for (int num = 2; num <= 35; num++) {
            int hnum_pipe[2];
            pipe(hnum_pipe);
            if (fork() == 0) {
                close(hnum_pipe[1]); // Close read end;

                int read_num = read_int_from_pipe(hnum_pipe[0]);
                close(hnum_pipe[0]);
                if (read_num == -1) {
                    fprintf(2, "Invalid number from pipe\n");
                    exit(1);
                }
                // if prime, send number into parent proc
                if (is_prime(read_num)) {
                    write_int_to_pipe(prime_pipe[1], read_num);
                }
                exit(0);
            } else {
                // for child process, write each number into pipeline
                close(hnum_pipe[0]); // Close read end
                write_int_to_pipe(hnum_pipe[1], num);
                close(hnum_pipe[1]);
            }
        }
        // Wait until all grandchild process finished
        wait(0);
        close(prime_pipe[1]);

        exit(0);
    } else {
        close(prime_pipe[1]); // Close prime pipe write end

        int prime_num;
        while ((prime_num = read_int_from_pipe(prime_pipe[0])) != 0) {
            printf("prime %d\n", prime_num);
        }
        close(prime_pipe[0]);

        exit(0);
    }
}
