#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>


int main()
{
    const size_t PAGE_SIZE = (size_t) getpagesize();
    size_t mem_size = PAGE_SIZE * 2;
    char* mem;
    size_t counter = 0;
    size_t result = 0;
    while (mem_size > PAGE_SIZE) {

        mem = (char*) malloc(mem_size);
        if (mem) {
            counter++;
            result += mem_size;
            mem_size *= 2;
        } else {
            mem_size /= 2;
        }
    }

    printf("result  = %zu\n", result);
    printf("counter = %zu\n", counter);

    return 0;
}
