#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "curl.h"
#include "catpng.h"
#include <pthread.h>


#define NUMBER_OF_IMAGES 50


// initializing a mutex in case you wanted to use it
pthread_mutex_t lock;


// global variables shared between threads
char* g_buffers[NUMBER_OF_IMAGES] = {0};
int g_count = 0;


typedef struct INPUT_OPTION {
    int t;
    int n;

} inputOption;


struct FETCH_IMAGE_FUNC_ARGS {
    int num_threads;
    int pic_num;
    int server_num;
    RECV_BUF *image_buffer;
};


inputOption parserOpt(int argc, char **argv) {
    int c;
    int t = 1;
    int n = 1;
    inputOption res = {.t = -1, .n = -1} ;
    char *str = "option requires an argument";
    while ((c = getopt (argc, argv, "t:n:")) != -1) {
        switch (c) {
            case 't':
	            t = strtoul(optarg, NULL, 10);
                printf("option -t specifies a value of %d.\n", t);
                res.t = t;
	            if (t <= 0) {
                    fprintf(stderr, "%s: %s > 0 -- 't'\n", argv[0], str);
                    return res;
                }
                break;
            case 'n':
                n = strtoul(optarg, NULL, 10);
                printf("option -n specifies a value of %d.\n", n);
                res.n = n;
                if (n <= 0 || n > 3) {
                    fprintf(stderr, "%s: %s 1, 2, or 3 -- 'n'\n", argv[0], str);
                    return res;
                }
                break;
            default:
                return res;
        }
    }

    return res;
}


void fetch_image(struct FETCH_IMAGE_FUNC_ARGS *args) {
    while (g_count != NUMBER_OF_IMAGES) {
        curl(args->image_buffer, args->server_num, args->pic_num);

        pthread_mutex_lock(&lock);
        if (g_buffers[args->image_buffer->seq] == 0) {
            g_buffers[args->image_buffer->seq] = args->image_buffer->buf;
            g_count++;
        }
        pthread_mutex_unlock(&lock);
    }
}


int main(int argc, char *argv[]) {
    inputOption res ;
    res = parserOpt(argc, argv);
    if (res.t <= 0) {
        res.t = 1;
    }


    struct FETCH_IMAGE_FUNC_ARGS args;
    args.image_buffer = malloc(BUF_SIZE);
    args.num_threads = res.t;
    args.pic_num = res.n;

    pthread_t *p_tids = malloc(sizeof(pthread_t) * res.t); // initializing thread pointers

    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }


    for (int i = 1; i <= res.t; i++) {
        if (i % 3 != 0) {
            args.server_num = i % 3;
        } else {
            args.server_num = 3;
        }
        pthread_create(p_tids + i - 1, NULL, fetch_image, &args); // running all of the threads
    }

    for (int i = 0; i < res.t; i++) {
        pthread_join(p_tids[i], NULL); // waiting for the threads to finish
    }

    catpng(g_count, g_buffers, args.image_buffer->size);

    pthread_mutex_destroy(&lock);
    free(args.image_buffer);
    free(p_tids);

    return 0;

}