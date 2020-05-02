#include <stdbool.h>

#ifndef SIED_COHESION_H
#define SIED_COHESION_H
int cohesive(const int window[], int threshold);
void find_edge(const int window[], int *out,  int threshold);
#endif //SIED_COHESION_H
