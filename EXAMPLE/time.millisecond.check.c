#include <stdio.h>
#include <time.h>
#include <stdint.h>

int main(int argc, char ** argv) {
    time_t v = time(NULL);      // 8 BYTES INTEGER
    uint32_t n = (uint32_t)(v);
    printf("sizeof(time_t) => %lu\n", sizeof(time_t));
    printf("%u => %lu\n", n, v);
    return 0;
}