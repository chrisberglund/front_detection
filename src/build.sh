#!/bin/bash
gcc -std=gnu99 -c -fPIC -pthread -o filter.o filter.c
gcc -std=gnu99 -c -fPIC -pthread -o cayula.o cayula.c
gcc -std=gnu99 -c -fPIC -pthread -o helpers.o helpers.c
gcc -std=gnu99 -c -fPIC -pthread -o prefilter.o prefilter.c
gcc -std=gnu99 -c -fPIC -pthread -o cohesion.o cohesion.c
gcc -std=gnu99 -c -fPIC -pthread -o contour.o contour.c
gcc -std=gnu99 -c -fPIC -pthread -o histogram.o histogram.c
gcc -std=gnu99 -c -fPIC -pthread -o initialize.o initialize.c

gcc -shared -fPIC -pthread -o sied.so filter.o cayula.o helpers.o prefilter.o cohesion.o contour.o histogram.o initialize.o

