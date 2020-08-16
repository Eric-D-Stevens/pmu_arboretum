CC=gcc
CFLAGS= -I. -Wall -g
LIBS= -lpfm

all: get_pmu_data.c get_pfm_encoding.c
	$(CC) -o get_pmu_data get_pmu_data.c get_pfm_encoding.c $(CFLAGS) $(LIBS)