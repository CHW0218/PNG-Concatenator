# Makefile for the project

CC = gcc
CFLAGS = -Wall -g -std=gnu99
LD = gcc
LDFLAGS = -g
LDLIBS = -lcurl -lz

LIB_UTIL = zutil.o crc.o lab_png.o curl.o catpng.o
SRCS   = paster.c zutil.c crc.c lab_png.c curl.c catpng.c
OBJS1  = paster.o $(LIB_UTIL)
TARGETS= paster

all: ${TARGETS}

paster: $(OBJS1)
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

# main_write_header_cb.out: $(OBJS3)
# 	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *~ *.d *.o $(TARGETS) *.png
