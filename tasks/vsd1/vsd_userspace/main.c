#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/errno.h>

 #include "../vsd_driver/vsd_ioctl.h"

typedef enum {
    GET_SIZE_ARGS_NUM = 2
    , SET_SIZE_ARGS_NUM = 3
} args_type;

typedef enum {
    PATH_POS = 0
    , COMMAND_POS = 1
    , ARGUMENT_POS = 2
} args_structure;

const char* DEV_PATH     = "/dev/vsd";

void show_help() 
{
    printf("Use cases:\n");
    printf("$ vsd_userspace size_get\n");
    printf("$ vsd_userspace size_set SIZE_IN_BYTES\n");
}

int get_size()
{
    int result = EXIT_FAILURE;
    vsd_ioctl_get_size_arg_t size_arg;
    
    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        printf("Usuccessful open!");
        return result;
    }

    int error_code = ioctl(fd, VSD_IOCTL_GET_SIZE, &size_arg);
    
    switch (error_code) {
        case EBADF:
            printf("fd is not a valid file descriptor\n");
            break;
        case EFAULT:
            printf("References an inaccessible memory area\n");
            break;
        case EINVAL:
            printf("Request or argp is not valid\n");
            break;
        case ENOTTY:
            printf("fd is not associated with a character special device\n");
            break;
        case 0:
            printf("size = %zu\n", size_arg.size);
            result = EXIT_SUCCESS;
            break;
        default:
            break;
    }

    close(fd);

    return result;
}

int set_size(int size)
{
    int result = EXIT_FAILURE;

    if (size < 0) {
        return result;
    }

    vsd_ioctl_get_size_arg_t size_arg;
    size_arg.size = size;

    int fd = open(DEV_PATH, O_RDONLY);
    if (fd == -1) {
        printf("Usuccessful open!");
        return result;
    }

    int error_code = ioctl(fd, VSD_IOCTL_SET_SIZE, &size_arg);
    
    switch (error_code) {
        case EBADF:
            printf("fd is not a valid file descriptor\n");
            break;
        case EFAULT:
            printf("References an inaccessible memory area\n");
            break;
        case EINVAL:
            printf("Request or argp is not valid\n");
            break;
        case ENOTTY:
            printf("fd is not associated with a character special device\n");
            break;
        case 0:
            printf("size set to = %zu\n", size_arg.size);
            result = EXIT_SUCCESS;
            break;
        default:
            break;
    }

    close(fd);

    return result;

}

int main(int argc, char **argv) 
{
    if (argc != GET_SIZE_ARGS_NUM && argc != SET_SIZE_ARGS_NUM) {
        goto return_with_failure;
    }

    if (argc == GET_SIZE_ARGS_NUM) {
        // size_get command
        if (strcmp(argv[COMMAND_POS], "size_get") != 0) {
            goto return_with_failure;
        }
        
        if (get_size() != EXIT_SUCCESS) {
            goto return_with_failure;
        }
        
    } else {
        // size_set command
        if (strcmp(argv[COMMAND_POS], "size_set") != 0) {
            goto return_with_failure;
        }

        if (set_size(atoi(argv[ARGUMENT_POS])) != EXIT_SUCCESS) {
            goto return_with_failure;
        }
    }

return_with_success:
    return EXIT_SUCCESS;

return_with_failure:
    show_help();
    return EXIT_FAILURE;
}
