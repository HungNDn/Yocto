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

static uint8_t g_image[IMAGE_SIZE];
static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static int g_ready_flag = 0;

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
    static unsigned int user_id_counter = VALID_USER_ID_START;

    while (1)
    {
        st_user_t* p_user = get_user_input();
        if (!p_user)
        {
            fprintf(stderr, "thread1_func: Failed to get user input\n");
            sleep(1);
            continue;
        }

        p_user->id = user_id_counter++;
        p_user->is_valid = USER_INVALID;

        if (mq_send(mq, (const char*)&p_user, sizeof(p_user), 0) == -1)
        {
            perror("thread1_func: mq_send failed");
            free(p_user);
            sleep(1);
            continue;
        }

        pthread_mutex_lock(&g_mutex);
        while (g_ready_flag == 0)
        {
            pthread_cond_wait(&g_cond, &g_mutex);
        }
        g_ready_flag = 0; 
        pthread_mutex_unlock(&g_mutex);

        sleep(1);
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
            continue;
        }

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

        print_user_info(p_user, g_image);
        memset(g_image, 0, IMAGE_SIZE);
        free(p_user);

        pthread_mutex_lock(&g_mutex);
        g_ready_flag = 1;
        pthread_cond_signal(&g_cond);
        pthread_mutex_unlock(&g_mutex);

        sleep(1);
    }

    pthread_exit(NULL);
}

int main(void)
{
    struct mq_attr attr = {
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

    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_cond);

    return EXIT_SUCCESS;
}
