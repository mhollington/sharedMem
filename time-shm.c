#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>

int main(int argc, char *argv[]) {

    // if no commands were given, show message and return early
    if (argc < 2) {
        printf("not enough arguments supplied");
        return 0;
    }

    pid_t pid;

    // variable used to store the starting time
    struct timeval start_time;


    // the size (in bytes) of shared memory object
    const int SIZE = 4096;

    // name of the shared memory object
    const char *name = "TIME";

    // shared memory file descriptor 
    int fd;

    // pointer to shared memory object
    struct timeval *ptr;

    // create the shared memory object
        fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    // configure the size of the shared memory object
        ftruncate(fd, SIZE);

    // memory map  the shared memory object
        ptr = (struct timeval *)
            mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);


    // fork a child process
    pid = fork();

    if (pid < 0) { // error occured
        fprintf(stderr, "Fork failed");
        return 1;

    }
    else if (pid == 0) { // child process

        // record starting time into start_time
        gettimeofday(&start_time, NULL); 
        
        // write starting time (time before execution of command)
        // into shared memory
        *ptr = start_time;

        // excecute given command
        execvp(argv[1], argv + 1);
        exit(0);

    }
    else { // parent process
        // parent waiting for child to terminate
        wait(NULL);

        // variables used to get the elapsed time
        double elapsed_time = 0.0;
        double start;
        double end;
        struct timeval end_time;

        // record ending time (time after execution of command)
        gettimeofday(&end_time, NULL);

        // calculate the start time and the end time from the 
	    // timeval variables by dividing the microseconds by 1000000 to 
        // covert it to seconds and then adding it to the number of seconds
        start = (double) ptr->tv_sec + (((double) ptr->tv_usec)/1000000);
        end = (double) end_time.tv_sec + (((double) end_time.tv_usec)/1000000);

        // calculate and display the elapsed time
	    // elapsed time is the difference b/w the end time and the start time
        printf("The elapsed time is: %lf\n", end - start);

    }
    

    // remove the shared memory object
    shm_unlink(name);

    return 0;


}