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

    /* open the shared memory region (file descriptor) -- expected to be existed */
    /* created by the producer */
    int fd = shm_open(REGION_NAME, O_RDWR, DEFFILEMODE);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    /* creating a new mapping in the virtual address space */
    buf = mmap(NULL, sizeof(*buf), PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);

    while (true) {
        /* check if the buffer is empty */
        while (buf->in == buf->out) {
            printf("BUFFER IS EMPTY!!!\n");
            sleep(SLEEP_TIME);
        }
        /* timer is just for consuming simulation */
        printf("CONSUMING...\n");
        sleep(SLEEP_TIME);

        /* consuming messages, and increasing output counter */
        struct message msg = buf->msgs[buf->out];
        buf->out = (buf->out + 1) % BUFFER_SIZE;

        printf("[RECV] -- MSG SIZE: %zu - MSG CONTENT: %s\n", strlen(msg.body), msg.body);
        /* terminate the consumer */
        if (*msg.body == QUIT) {
            printf("Consumer Terminated");
            break;
        }
    }
    return 0;
}
