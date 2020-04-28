#ifndef SIED_CONTOUR_H
#define SIED_CONTOUR_H
typedef struct contour_point {
    int bin;
    int angle;
    struct contour_point *prev;
    struct contour_point *next;
} ContourPoint;
double gradient_ratio(const int *window);
ContourPoint * new_contour_point(ContourPoint *prev, int bin, int angle);
ContourPoint * find_best_front(ContourPoint *prev, const int *data,  int row, const int *basebins, const int *nbins_in_row);
int follow_contour(ContourPoint *prev, const int *data, const int *filtered_data, int *pixel_in_contour, int row, int nrows, const int *basebins, const int *nbins_in_row);
void contour(int *data, int *filtered_data, int *out_data, int nbins, int nrows, const int *nbins_in_row, const int *basebins);
#endif //SIED_CONTOUR_H
