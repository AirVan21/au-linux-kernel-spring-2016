#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "vsd_device.h"
#include "../vsd_driver/vsd_ioctl.h"

const char* path = "/dev/vsd";
static int fd;

int vsd_init()
{
    fd = open(path, O_RDWR);
    return fd != -1 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int vsd_deinit()
{
    return close(fd);
}

int vsd_get_size(size_t *out_size)
{
    vsd_ioctl_get_size_arg_t size_arg;
    
    int result = ioctl(fd, VSD_IOCTL_GET_SIZE, &size_arg);
    if (result != 0) {
        return EXIT_FAILURE;
    }

    *out_size = size_arg.size;

    return EXIT_SUCCESS;
}

int vsd_set_size(size_t size)
{
    if (size < 0) {
        return EXIT_FAILURE;
    }

    vsd_ioctl_get_size_arg_t size_arg;
    size_arg.size = size;

    int result = ioctl(fd, VSD_IOCTL_SET_SIZE, &size_arg);
    if (result != 0) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

ssize_t vsd_read(char* dst, off_t offset, size_t size)
{
    int result = lseek(fd, offset, SEEK_SET);
    if (result == -1) {
        return EXIT_FAILURE;
    }

    return read(fd, dst, size);
}

ssize_t vsd_write(const char* src, off_t offset, size_t size)
{
    int result = lseek(fd, offset, SEEK_SET);
    if (result == -1) {
        return EXIT_FAILURE;
    }

    return write(fd, src, size);
}

void* vsd_mmap(size_t offset)
{
    const size_t PAGE_SIZE = (size_t)getpagesize();
    if (offset % PAGE_SIZE != 0) {
        return MAP_FAILED;
    }

    size_t size;
    vsd_get_size(&size);

    return mmap(NULL, size - offset, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
}

int vsd_munmap(void* addr, size_t offset)
{
    const size_t PAGE_SIZE = (size_t)getpagesize();
    if (offset % PAGE_SIZE != 0) {
        return EXIT_FAILURE;
    }
    
    size_t size;
    vsd_get_size(&size);

    return munmap(addr, size - offset);
}
