/**
 * @file main.c
 * @biref To create and join 2 threads. Each thread takes more than one input parameter
 *        and returns more than one result value.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 50

pthread_mutex_t lock;
int count = 0;
int buffers[50];

struct thread_args              /* thread input parameters struct */
{
    int x;
    int y;
};

struct thread_ret               /* thread return values struct   */
{
    int sum;
    int product;
};

void *do_work(void *arg);  /* a routine that can run as a thread by pthreads */

void *do_work(void *arg)
{
    struct thread_args *p_in = arg;
    struct thread_ret *p_out = malloc(sizeof(struct thread_ret));
                             /* the root thread will need to free the memory */


    while(1){
        if(count >= 50){
            break;
        }
        int num = rand() % 50;
        sleep(1);
        pthread_mutex_lock(&lock);
        printf (" %d \n", num);
        for(int i = 0; i < 50 ; i++) {
        printf("%d ",buffers[i]);
        }
        printf ("\n");
        if(buffers[num]== 0){
             buffers[num] = num;
             count++;
             printf("count:%d\n",count);
        }
        pthread_mutex_unlock(&lock);
    }





    /* pthread_exit memory leak bug
       @see https://bugzilla.redhat.com/show_bug.cgi?id=483821
       replacing it with return will avoid the bug
    */

    /* pthread_exit((void *)p_out);*/

    return ((void *)p_out);
}


int main(int argc, char **argv)
{
    pthread_t *p_tids = malloc(sizeof(pthread_t) * NUM_THREADS);
    struct thread_args in_params[NUM_THREADS];
    struct thread_ret *p_results[NUM_THREADS];
    for(int i = 0; i < 50 ; i++) {
        buffers[i] = 0;
    }



    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }

    for (int i=0; i<NUM_THREADS; i++) {
        in_params[i].x = i + 1;
        in_params[i].y = i + 2;
        pthread_create(p_tids + i, NULL, do_work, in_params + i);
    }


    for (int i=0; i<NUM_THREADS; i++) {
        pthread_join(p_tids[i], (void **)&(p_results[i]));
        printf("Thread ID %lu joined.\n", p_tids[i]);
        printf("sum(%d,%d) = %d.\n", \
               in_params[i].x, in_params[i].y, p_results[i]->sum);
        printf("product(%d,%d) = %d.\n", \
               in_params[i].x, in_params[i].y, p_results[i]->product);
    }

    /* cleaning up */

    free(p_tids);
    pthread_mutex_destroy(&lock);

    /* the memory was allocated in the do_work thread for return values */
    /* we need to free the memory here */
    for (int i=0; i<NUM_THREADS; i++) {
        free(p_results[i]);
    }

    return 0;
}
