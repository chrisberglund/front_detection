#include "cayula.h"

#ifndef SIED_HELPERS_H
#define SIED_HELPERS_H
void sort(int *array, int left, int right);
int findClosestValue(int arr[], int l, int r, int x);
int getNeighborBin(struct binLoc bin, int distance, struct binRows basebins);
int max(const double *arr, int length);
int median(const int *arr, int length);
bool getWindow(struct binLoc bin, int width, const int *data, struct binRows rows, int *window);


#endif //SIED_HELPERS_H
