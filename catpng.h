#pragma once

/* INCLUDES */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "lab_png.h"
#include "crc.h"
#include "zutil.h"


int catpng(int num, char *buffers[],long size);