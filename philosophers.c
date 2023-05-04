#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// functions used by the philosopher threads
void *philosophers(void *arg); 
int pickup_forks(int philo_num);
int return_forks(int philo_num);
int eat_test(int philo_num);


// Global variables
pthread_t philos[5]; // philosopher threads
pthread_mutex_t eating;
pthread_cond_t check_philos[5]; // conditional variable for each philosopher
char *states[5]; // current state of each philosopher
int meals; // number of meals each philosopher must have
int num_of_meals[5]; // current number of meals eaten by each philosopher

// philosopher ids
int first = 0;
int second = 1;
int third = 2;
int fourth = 3;
int fifth = 4;


int main(int argc, const char * argv[]) {
    srand(time(NULL)); // initialize random generator


    // check if correct number of arguments were given
    if (argc != 2) { // if the number of arguments is incorrect an error message is shown
        printf("Wrong number of arguments\n");
        return 0;
    }

    // set the number of meals each philosopher has to eat to given command line argument
    meals = atoi(argv[1]);

    // go through each philosopher and initialize their state to thinking, the number of meals they've
    // eaten  to 0 and their conditional variable
    for (int k = 0; k < 5; k ++) {
        states[k] = "thinking";
        num_of_meals[k] = 0;
        pthread_cond_init(&check_philos[k], NULL);
    }

    // initialize the mutex
    pthread_mutex_init(&eating, NULL);

    // create the 5 philosopher threads
    pthread_create(&philos[0], 0, philosophers, &first);
    pthread_create(&philos[1], 0, philosophers, &second);
    pthread_create(&philos[2], 0, philosophers, &third);
    pthread_create(&philos[3], 0, philosophers, &fourth);
    pthread_create(&philos[4], 0, philosophers, &fifth);


    // wait for each thread to terminate
    for (int j = 0; j < 5; j++) {
        pthread_join(philos[j], NULL);
    }


    return 0;
}

// function that is passed to each philosopher thread
void *philosophers(void *arg) {
    // change the argument back into an integer
    int num = *((int *) arg); 

    // display the beginning state of the philosopher 
    // which will be "thinking"
    printf("philosopher %d is %s\n", num, states[num]);
    
    // keep alternating through the states until the philosopher has eaten 
    // the given amount of meals 
    while (num_of_meals[num] < meals) {

        // calculate a random integer between 1 and 3, and make the philosopher thread
        // sleep for that amount of time to simulate the activity of thinking
        int think_time = (rand() % 3) + 1; 
        sleep(think_time); 

        // Once the philosopher is done thinking they can try to eat by calling
        // the function pickup_forks()
        pickup_forks(num);

        // Once the philosopher is done eating they put down the forks and change
        // their state back to thinking by calling return_forks()
        return_forks(num);

    }

    // output how many meals the given philosopher has eaten
    printf("Philosopher %d has eaten %d meals\n", num, num_of_meals[num]);
    
    // terminate the thread
    pthread_exit(0);

}


// function that is called when a philosopher thread wants to eat
int pickup_forks(int philo_num) {

    // lock the mutex
    pthread_mutex_lock(&eating);

    // change the philosopher's state to hungry and display the change in state
    states[philo_num] = "hungry";
    printf("philosopher %d is now %s\n", philo_num, states[philo_num]);


    // call the eat_test function to see if the philosopher can start eating
    eat_test(philo_num);

    // if the philosopher still has not eaten they have to wait until a signal is sent
    while (states[philo_num] == "hungry") {
        pthread_cond_wait(&check_philos[philo_num], &eating);
        }

    // unlock the mutex
    pthread_mutex_unlock(&eating);

    // display the state of the philosopher 
    // which will be "eating"
    printf("philosopher %d is %s\n", philo_num, states[philo_num]);

    // calculate a random integer between 1 and 3, and make the philosopher thread
    // sleep for that amount of time to simulate the activity of eating
    int eat_time = (rand() % 3) + 1;
    sleep(eat_time);


    return 0;

}


// this function checks if the philosophers beside the given philosopher are eating
// and alters the state of the given philosopher accordingly
int eat_test(int philo_num) {

    // checks to see that the philosopher is hungry and the two philosopers beside her are 
    // not eating
    if (states[(philo_num + 4) % 5] != "eating" && states[(philo_num + 1) % 5] != "eating" && states[philo_num] == "hungry") {
        // the philosopher can eat

        // change the philosopher's state to eating
        states[philo_num] = "eating";

        // show that the philosopher picks up the forks beside her
        printf("Philosoper %d grabs fork on the left\n", philo_num); 
        printf("Philosoper %d grabs fork on the right\n", philo_num);

        

        // signals the corresponding conditional variable
        // if the conditional variable is not in wait the signal does not do anything
        // but if the conditional variable was waiting the signal stops the wait
        pthread_cond_signal(&check_philos[philo_num]);


    }

    return 0;

}

// function that is called once a philosopher is done eating
int return_forks(int philo_num) {

    // lock the mutex
    pthread_mutex_lock(&eating);

    // incrememnt the number of meals eaten by the given philosopher by 1
    num_of_meals[philo_num] += 1;

    // change the state of the philosopher back to thinking and display this change
    states[philo_num] = "thinking";
    printf("philosopher %d puts down the forks and is now back to %s\n", philo_num, states[philo_num]);


    // call eat_test to see if the neighbouring philosophers can eat 
    // now that this philosopher has put down their forks and stopped eating
    eat_test((philo_num + 4) % 5);
    eat_test((philo_num + 1) % 5);

    // unlock the mutex
    pthread_mutex_unlock(&eating);

    return 0;

}


