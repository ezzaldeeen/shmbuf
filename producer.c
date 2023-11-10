#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define REGION_NAME "/shared"
#define BUFFER_SIZE 5
#define QUIT 'q'
#define MSG_SIZE 10
#define SLEEP_TIME 2

struct message {
    char body[MSG_SIZE];
};

struct buffer {
    int in;
    int out;
    struct message msgs[BUFFER_SIZE];
};

int main() {
    /* shared memory configurations */
    struct buffer *buf;

    /* create a new shared memory region (file descriptor) */
    int fd = shm_open(REGION_NAME, O_CREAT | O_RDWR, DEFFILEMODE);
    if (ftruncate(fd, sizeof(*buf)) == -1) {
        /* an issue while truncating shared memory region */
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }


    /* creating a new mapping in the virtual address space */
    buf = mmap(NULL, sizeof(*buf), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) {
        /* an issue while creating mapping in virtual address space */
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    while (true) {
        /* check if the buffer is full */
        while ((buf->in + 1) % BUFFER_SIZE == buf->out) {
            printf("BUFFER IS FULL!!!\n");
            sleep(SLEEP_TIME);
        }
        /* getting the message from the stdin */
        struct message msg;
        printf("send message: ");
        scanf("%s", msg.body);

        /* push the message to the circular buffer */
        buf->msgs[buf->in] = msg;
        /* increase the input counter for the next message */
        buf->in = (buf->in + 1) % BUFFER_SIZE;
        /* terminate the producer */
        if (*msg.body == QUIT) {
            printf("Producer Terminated");
            break;
        }
    }
    /* unlink shared memory region (file descriptor) */
    if (shm_unlink(REGION_NAME) == -1) {
        perror("shm_unlink");
        exit(EXIT_FAILURE);
    }
    return 0;
}
