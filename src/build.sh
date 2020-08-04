#!/bin/bash
gcc -std=gnu99 -c -g -fPIC -pthread -o filter.o filter.c
gcc -std=gnu99 -c -g -fPIC -pthread -o cayula.o cayula.c
gcc -std=gnu99 -c -g -fPIC -pthread -o helpers.o helpers.c
gcc -std=gnu99 -c -g -fPIC -pthread -o cohesion.o cohesion.c
gcc -std=gnu99 -c -g -fPIC -pthread -o contour.o contour.c
gcc -std=gnu99 -c -g -fPIC -pthread -o histogram.o histogram.c

gcc -shared -fPIC -pthread -g -o ../sied.so filter.o cayula.o helpers.o cohesion.o contour.o histogram.o

