#ifndef SIED_CONTOUR_H
#define SIED_CONTOUR_H
typedef struct contour_point {
    int bin;
    int angle;
    struct contour_point *prev;
    struct contour_point *next;
} ContourPoint;
double gradient_ratio(const int *window, int fillValue);
ContourPoint * new_contour_point(ContourPoint *prev, int bin, int angle);
ContourPoint * find_best_front(ContourPoint *prev, const int *data,  int row, const int *basebins, const int *nbins_in_row);
#endif //SIED_CONTOUR_H
