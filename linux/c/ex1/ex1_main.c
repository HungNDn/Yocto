#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <mqueue.h>
#include <stdint.h>
#include <sys/stat.h>
#include <time.h>

#include "user_data.h"

#define QUEUE_NAME "/ex1_user_queue"
#define MAX_MSG 10

volatile int g_user_data_ready = 0; // Global volatile flag to synchronize two threads: thread2 has validated user data, thread1 can continue
/* 
 * Using volatile because this variable is accessed by two threads,
 * to avoid compiler caching issues.
 * This variable signals that thread2 has finished processing user data.
 */

static uint8_t g_image[IMAGE_SIZE]; // Simulated global image buffer, static for file-private, persists throughout the program

static bool load_image_from_file(const char* path, uint8_t* buffer, size_t size)
{
    if (!path || !buffer) 
    {
        return false;
    }

    FILE* fp = fopen(path, "rb");
    if (!fp) 
    {
        perror("load_image_from_file: fopen failed");
        return false;
    }

    size_t bytes_read = fread(buffer, 1, size, fp);
    fclose(fp);

    if (bytes_read < size) 
    {
        fprintf(stderr, "Image too small: expected %zu bytes, got %zu\n", size, bytes_read);
        return false;
    }

    return true;
}

static void* thread1_func(void* p_arg)
{
    if (p_arg == NULL)
    {
        fprintf(stderr, "thread1_func: NULL argument\n");
        pthread_exit(NULL);
    }

    mqd_t mq = *(mqd_t*)p_arg;
    static unsigned int user_id_counter = VALID_USER_ID_START; // ID starting from 1000

    while (1)
    {
        st_user_t* p_user = get_user_input();
        if (p_user == NULL)
        {
            fprintf(stderr, "thread1_func: Failed to get user input\n");
            sleep(1);
        }
        else
        {
            p_user->id = user_id_counter++;
            p_user->is_valid = USER_INVALID; // reset valid flag before sending

            // Send pointer to user struct via message queue
            if (mq_send(mq, (const char*)&p_user, sizeof(p_user), 0) == -1)
            {
                perror("thread1_func: mq_send failed");
                free(p_user);
                sleep(1);
            }
            else
            {
                // Wait for thread2 to process (synchronize)
                while (g_user_data_ready == 0)
                {
                    usleep(100 * 1000); // 100ms
                }
                g_user_data_ready = 0; // reset for next time
                sleep(1);
            }
        }
    }

    pthread_exit(NULL);
}

static void* thread2_func(void* p_arg)
{
    if (p_arg == NULL)
    {
        fprintf(stderr, "thread2_func: NULL argument\n");
        pthread_exit(NULL);
    }

    mqd_t mq = *(mqd_t*)p_arg;

    while (1)
    {
        st_user_t* p_user = NULL;
        ssize_t bytes = mq_receive(mq, (char*)&p_user, sizeof(p_user), NULL);
        if (bytes == -1)
        {
            perror("thread2_func: mq_receive failed");
            sleep(1);
        }
        else
        {
            if (validate_user_data(p_user))
            {
                p_user->is_valid = USER_VALID;
                if (!load_image_from_file(p_user->image_path, g_image, IMAGE_SIZE))
                {
                    fprintf(stderr, "Error: Failed to load image from %s\n", p_user->image_path);                  
                }
            }
            else
            {
                p_user->is_valid = USER_INVALID;
            }

            g_user_data_ready = 1; // notify thread1 that data has been processed

            print_user_info(p_user, g_image);
            memset(g_image, 0, IMAGE_SIZE); // clear buffer to avoid showing stale data
            free(p_user);
            sleep(1);
        }
    }

    pthread_exit(NULL);
}

int main(void)
{
    struct mq_attr attr = 
    {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MSG,
        .mq_msgsize = sizeof(st_user_t*),
        .mq_curmsgs = 0
    };

    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
    if (mq == (mqd_t)-1)
    {
        perror("main: mq_open failed");
        return EXIT_FAILURE;
    }

    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, thread1_func, &mq) != 0)
    {
        perror("main: pthread_create thread1 failed");
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    if (pthread_create(&t2, NULL, thread2_func, &mq) != 0)
    {
        perror("main: pthread_create thread2 failed");
        pthread_cancel(t1);
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
        return EXIT_FAILURE;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    if (mq_close(mq) == -1)
    {
        perror("main: mq_close failed");
    }

    if (mq_unlink(QUEUE_NAME) == -1)
    {
        perror("main: mq_unlink failed");
    }

    return EXIT_SUCCESS;
}
