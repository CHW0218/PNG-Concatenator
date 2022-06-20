#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "lab_png.h"

int is_png(U8 *buf ,size_t n){
    unsigned char signature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    int result = memcmp(signature, buf, n);
    return !result;
}

int get_png_data_IHDR(struct data_IHDR *out, char * buffer, long offset, int whence){
    whence += offset;
    memcpy(out, buffer + whence, DATA_IHDR_SIZE);
    out->width = ntohl(out->width);
    out->height = ntohl(out->height);
    whence -= offset;
    return 0;
}

