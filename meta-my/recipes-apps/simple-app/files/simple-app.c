#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/simple_char"

int main()
{
    int fd = -1;
    char choice;
    char buffer[1024];

    while (1) {
        printf("\nOptions:\n");
        printf(" o - open device\n");
        printf(" w - write to device\n");
        printf(" r - read from device\n");
        printf(" c - close device\n");
        printf(" q - quit\n");
        printf("Enter choice: ");
        scanf(" %c", &choice);

        switch (choice) {
        case 'o':
            fd = open(DEVICE, O_RDWR);
            if (fd < 0)
                perror("open");
            else
                printf("Device opened successfully\n");
            break;

        case 'w':
            if (fd < 0) {
                printf("Device not open!\n");
                break;
            }
            printf("Enter string to write: ");
            scanf(" %[^\n]", buffer);
            write(fd, buffer, strlen(buffer));
            break;

        case 'r':
            if (fd < 0) {
                printf("Device not open!\n");
                break;
            }
            memset(buffer, 0, sizeof(buffer));
            read(fd, buffer, sizeof(buffer));
            printf("Read from device: %s\n", buffer);
            break;

        case 'c':
            if (fd >= 0) {
                close(fd);
                printf("Device closed\n");
                fd = -1;
            } else {
                printf("Device already closed\n");
            }
            break;

        case 'q':
            if (fd >= 0)
                close(fd);
            exit(0);

        default:
            printf("Invalid option!\n");
            break;
        }
    }

    return 0;
}
