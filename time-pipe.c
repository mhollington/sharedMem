#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>


#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]) {

    // if no commands were given, show message and return early
    if (argc < 2) {
        printf("not enough arguments supplied");
        return 0;
    }

    int fd[2];
    pid_t pid;

    // variable used to store the starting time
    struct timeval start_time;

    // create the pipe
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    // fork a child process
    pid = fork();

    if (pid < 0) {
        // error occurred
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid == 0) { // child process
        //record starting time 
        gettimeofday(&start_time, NULL);

        // close the unused end of the pipe
        close(fd[READ_END]);

        // write to the pipe
        write(fd[WRITE_END], &start_time, sizeof(start_time));

        // close the write end of the pipe 
        close(fd[WRITE_END]);

        // execute the given command
        execvp(argv[1], argv + 1);
        exit(0);

    }
    else { // parent process
        // parent waits for child to terminate
        wait(NULL);

        // variables used for recording time
        struct timeval end_time;
        double end;
        double start;
        double elapsed_time;

        // record ending time
        gettimeofday(&end_time, NULL);

        // close the unused end of the pipe
        close(fd[WRITE_END]);

        // read from the pipe
        read(fd[READ_END], &start_time, sizeof(start_time));

        // close the read end of the pipe
        close(fd[READ_END]);

        // calculate the recorded time from before the execution of the command   
        // by using the timeval variable read from the pipe and adding the seconds
        // and the microseconds together
        start = ((double) start_time.tv_sec + (((double) start_time.tv_usec)/1000000));; 

        // calculate the recorded time from after the execution of the command 
        // by adding the seconds and microseconds together
        end = ((double) end_time.tv_sec + (((double) end_time.tv_usec)/1000000));

        // display the elapsed time
        printf("The elapsed time is: %lf\n", end - start);

    }

    return 0;
}