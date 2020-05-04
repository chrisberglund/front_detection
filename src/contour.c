#include "contour.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "helpers.h"
#include "cayula.h"

static inline double square(double a) {
    return a * a;
}

static inline int mod(int a, int n) {
    return a - floor(a / n) * n;
}


const int ANGLES[9] = {135, 90, 45,
                       180, 360, 0,
                       225, 270, 315};

struct contour {
    struct contour_point *first_point;
    struct contour *prev;
    struct contour *next;
    int length;
} typedef Contour;

struct vector{
    double x;
    double y;
} typedef Vector;


static inline int dot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y;
}
/*
 * Function:  gradient
 * --------------------
 * Calculates the gradient of the center pixel in the provided array using first order central differences. Fill values
 * at pixels within the directional step of the gradient calculation are replaced with the center pixel value.
 *
 * args:
 *      int *window: pointer to an array of 9 elements representing a window of data values from which to calculate
 *      the gradient of the center pixel
 *
 * returns:
 *      Vector: the vector gradient
 */
Vector gradient(int *window) {
    if (window[5] == FILL_VALUE) window[5] = window[4];
    if (window[3] == FILL_VALUE) window[3] = window[4];
    if (window[7] == FILL_VALUE) window[7] = window[4];
    if (window[1] == FILL_VALUE) window[1] = window[4];
    Vector g;
    g.x = (double) (window[5] - window[3]) / 2;
    g.y = (double) (window[7] - window[1]) / 2;
    return g;
}

/*
 * Function:  turn_too_sharp
 * --------------------
 * Checks to see if adding a point at the provided angle would result in the contour changing direction by more than 90
 * degrees over the course of 5 pixels.
 *
 * args:
 *      ContourPoint *tail: the last point currently on the contour
 *      int next_theta: the angle between the last point on the contour and the point to be added
 *
 * returns:
 *      int: 1 if change in direction exceeds 90 degrees, 0 if it does not
 */
int turn_too_sharp(ContourPoint *tail, int next_theta) {
    int counter = 0;
    int is_turn_too_sharp = 0;
    ContourPoint *tmp = tail;
    while (tmp->prev->prev != NULL && counter < 5) {
        int dtheta = mod(tmp->angle - next_theta + 180, 360) - 180;
        dtheta = dtheta < -180 ? dtheta + 360 : dtheta;
        dtheta = abs(dtheta);
        if (dtheta > 90) {
            is_turn_too_sharp = 1;
            break;
        }
        tmp = tmp->prev;
        counter++;

    }
    return is_turn_too_sharp;
}

/*
 * Function:  get_gradient_ratio
 * --------------------
 * Calculates the ratio between the magnitude of the gradient sum and the sum of magnitude of the gradient of the pixels
 * in a 3x3 window centered on the center pixel of the provided 5x5 window. The gradient is calculated with
 * first order central differences.
 *
 * args:
 *      int *window: pointer to an array 25 elements long representing a 5x5 window centered on the last contour pixel
 *
 * returns:
 *      int: the ratio between the magnitude of the sum of the gradient vectors and the sum of the magnitude of the
 *      gradient vectors for each pixel in a 3x3 window centered on the center pixel of the provided window.
 */
double gradient_ratio(const int *window) {
    double sum_magnitude = 0, sum_x = 0, sum_y = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int inner_window[9];
            inner_window[0] = window[i * 5 + 6 + j];
            inner_window[1] = window[i * 5 + 7 + j];
            inner_window[2] = window[i * 5 + 8 + j];
            inner_window[3] = window[i * 5 + 11 + j];
            inner_window[4] = window[i * 5 + 12 + j];
            inner_window[5] = window[i * 5 + 13 + j];
            inner_window[6] = window[i * 5 + 16 + j];
            inner_window[7] = window[i * 5 + 17 + j];
            inner_window[8] = window[i * 5 + 18 + j];

            Vector g = gradient(inner_window);
            sum_magnitude += sqrt(square(g.x) + square(g.y));
            sum_x += g.x;
            sum_y += g.y;
        }
    }
    return sqrt(square(sum_x) + square(sum_y)) / sum_magnitude;
}

 Contour *new_contour(Contour *prev, int bin) {
     Contour *n = malloc(sizeof(Contour));
     n->prev = prev;
     n->next = NULL;

     if (prev != NULL) prev -> next = n;

     ContourPoint *c = malloc(sizeof(ContourPoint));
     c->bin = bin;
     c->prev = NULL;
     c->next = NULL;
     n->first_point = c;
     return n;
 }

/*
* Function:  del_contour
* --------------------
* Deletes the provided node in the linked list of contours and all the contour point nodes that belongs to it. If
* the node is the middle of the linked list, the nodes before and after are linked to each other.
*
* args:
*      Contour *n: the node of the linked list to delete
* returns:
 *     Contour *: pointer to next contour
*/
Contour * del_contour(Contour *n) {
    ContourPoint *c = n->first_point;
    Contour *next = n->next;
    while (c != NULL) {
        ContourPoint *tmp = c;
        c = c->next;
        free(tmp);
    };
    if (n->prev != NULL) {
        if (n->next != NULL) {
            n->prev->next = n->next;
            if (n->next != NULL) {
                n->next->prev = n->prev;
            }
        } else {
            n->prev->next = NULL;
        }
    }
    free(n);
    return next;
}

/*
 * Function:  new_contour_point
 * --------------------
 * Creates a new contour point and adds it to the end of the doubly linked list representing a single contour.
 *
 * args:
 *      ContourPoint *prev: the last node in the contour linked list
 *      int bin: the bin number of the new point to add to the list
 *      int angle: the angle between the last point in the contour and the new point
 *
 * returns:
 *      ContourPoint *: the new point that is now the last node in the linked list
 */
ContourPoint * new_contour_point(ContourPoint *prev, int bin, int angle) {
    ContourPoint *c = malloc(sizeof(ContourPoint));
    c->bin = bin;
    c->angle = angle;
    c->prev = prev;
    c->next = NULL;
    if (prev != NULL)     prev->next = c;
    return c;
}

/*
 * Function:  get_bin_number
 * --------------------
 * Returns a bin number provided the index of a 9 element array representing a 3x3 window centered on the given bin
 * number.
 *
 * args:
 *      int bin: the center bin number in the window of interest
 *      int i: the index for the desired bin in the window of interest
 *      int row: the row number of the center bin
 *      int *basebins: pointer to an array containing the bin number for the first bin of each row
 *      int *nbins_in_row: pointer to an array containing the number of bins in each row
 *
 * returns:
 *      int: bin number of the desired bin
 */
static int get_bin_number(int bin, int i, int row, const int *basebins, const int *nbins_in_row) {
    double ratio;
    int next_bin;
    switch(i) {
        case 0:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row - 1] + 0.5)  + basebins[row - 1] - 1;
            break;
        case 1:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row - 1] + 0.5)  + basebins[row - 1];
            break;
        case 2:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row - 1] + 0.5)  + basebins[row - 1] + 1;
            break;
        case 3:
            next_bin = bin - 1;
            break;
        case 5:
            next_bin = bin + 1;
        case 6:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row + 1] + 0.5)  + basebins[row + 1] - 1;
            break;
        case 7:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row + 1] + 0.5)  + basebins[row + 1];
            break;
        case 8:
            ratio = (bin - basebins[row]) / (double) nbins_in_row[row];
            next_bin = (int) (ratio * nbins_in_row[row + 1] + 0.5)  + basebins[row + 1] + 1;
            break;
    }
    return next_bin;
}

/*
 * Function:  find_best_front
 * --------------------
 * Of the bins neighboring the last bin on the contour, this function selects the best front bin to add to the contour.
 * Going through all the neighboring bins, the function identifies the next bin that will change the direction of the
 * contour the least. However, if adding the selected bin would result in the contour changing direction by more than
 * 90 degrees over the course of 5 bins, the bin is rejected as a possible addition to the contour. If the provided
 * contour point is the first point in the contour and thus has no direction, the selection is biased towards higher
 * numbered bins as they are the least likely to be contained in other contours.
 *
 * args:
 *      ContourPoint *prev: the last edge pixel in the current contour
 *      int *data: pointer to a boolean array representing the pixels status as an edge pixel
 *      int row: the row of the last edge pixel in the current contour
 *      int *basebins: pointer to an array containing the index of the first bin of each row
 *      int *nbins_in_row: pointer to an array containing the number of bins in each row
 *
 * returns:
 *      ContourPoint *: the selected point to add to the contour. Pointer will be NULL if there is no previously
 *      identified edge pixel to add to the contour.
 */
ContourPoint * find_best_front(ContourPoint *prev, const int *data,  int row, const int *basebins, const int *nbins_in_row) {
    int edge_window[9];
    get_window(prev->bin, row, 3, data, nbins_in_row, basebins, edge_window);
    int next_bin = -1;
    int min_dtheta = 180;
    int next_angle;
    for (int i = 0; i < 9; i++) {
        int dtheta = 180;
        if (i != 4 && edge_window[i]) {
            if (prev->prev == NULL) {
                dtheta = 0;
            } else {
                dtheta = prev->angle - ANGLES[i];
                dtheta = mod(dtheta + 180, 360) - 180;
                dtheta = dtheta < -180 ? dtheta + 360 : dtheta;
                dtheta = abs(dtheta);
            }
            if (dtheta == 0 || dtheta < min_dtheta) {
                min_dtheta = dtheta;
                next_angle = ANGLES[i];
                next_bin = get_bin_number(prev->bin, i, row, basebins, nbins_in_row);
            }
        }
    }

    if (next_bin != -1 && (prev->prev == NULL || !turn_too_sharp(prev, next_angle))) {
        return new_contour_point(prev, next_bin, next_angle);
    } else {
        return NULL;
    }
}

/*
 * Function:  find_best_front
 * --------------------
 * Recursive function for growing the contour using the previously detected edge pixels and gradients.
 *
 * args:
 *      ContourPoint *prev: the last edge pixel in the current contour
 *      int *data: pointer to a boolean array representing the pixels status as an edge pixel
 *      int *filtered_data: point to an array containing the data that resulted from applying a median filter to
 *      the original data
 *      int row: the row of the last edge pixel in the current contour
 *      int nrows: the number of rows in the binning scheme
 *      int *basebins: pointer to an array containing the index of the first bin of each row
 *      int *nbins_in_row: pointer to an array containing the number of bins in each row
 *
 * returns:
 *      int: the number of points in the contour that are contained in the segment of the contour starting with
 *      the current point
 */
int follow_contour(ContourPoint *prev, const int *data, const int *filtered_data, int *pixel_in_contour, int row, int nrows, const int *basebins, const int *nbins_in_row) {
    ContourPoint *next_point;
    next_point = find_best_front(prev, data, row, basebins,nbins_in_row);
    int count = 1;
    double ratio = 0;
    int max_bin;
    if (next_point == NULL) {
        int outer_window[25];
        get_window(prev->bin, row, 5, filtered_data, nbins_in_row, basebins, outer_window);
        double ratio = gradient_ratio(outer_window);
        if (ratio > 0.7) {
            int bin_window[9];
            double max_product = -1;
            int max_idx = -1;
            get_window(prev->bin, row, 3, filtered_data, nbins_in_row, basebins, bin_window);
            Vector gradient0 = gradient(bin_window);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (i != 1 || j != 1) {
                        int bin = get_bin_number(prev->bin, i * 3 + j, row,basebins, nbins_in_row);
                        if (!pixel_in_contour[bin]) {
                            get_window(bin, row + j - 1, 3, filtered_data, nbins_in_row, basebins, bin_window);
                            Vector gradient1 = gradient(bin_window);
                            double product = dot(gradient0, gradient1);
                            if (product > max_product) {
                                max_product = product;
                                max_idx = i * 3 + j;
                                max_bin = bin;
                            }
                        }
                    }

                }
            }
            if (max_product > 0) {
                next_point = new_contour_point(prev, max_bin, ANGLES[max_idx]);
            }
        }
    }

    if (next_point != NULL && !pixel_in_contour[next_point->bin]) {
        int next_row;
        pixel_in_contour[next_point->bin] = 1;
        switch(next_point->angle) {
            case 0:
            case 180:
                next_row = row;
                break;
            case 1 ... 179:
                next_row = row - 1;
                break;
            case 181 ... 359:
                next_row = row + 1;
                break;
            default:
                next_row = row;
                break;
        }

        /*
         * If the next point is too close to the edge of the map, we still need to increment the counter, but we don't
         * want to try following the contour any further
         */
        if (next_row < nrows - 2 && next_row > 1 && next_point->bin > basebins[next_row] + 1 && next_point->bin < basebins[next_row + 1] - 2) {
            count += follow_contour(next_point, data, filtered_data, pixel_in_contour, next_row, nrows, basebins,
                                    nbins_in_row);
        } else {
            count++;
        }
    }

    return count;
}

/*
 * Function:  contour
 * --------------------
 * Creates and extends contours using previously detected edges and gradients to define the final edges.
 *
 * args:
 *      int *data: pointer to a boolean array representing the pixels status as an edge pixel
 *      int *filtered_data: point to an array containing the data that resulted from applying a median filter to
 *      the original data
 *      int *out_data: pointer an array to write the front values for each pixel. 1 for a front, 0 for not
 *      int nbins: the number of bins in the binning scheme
 *      int nrows: the number of rows in the binning scheme
 *      int *nbins_in_row: the number of bins in each row
 *      int *basebins: pointer to an array containing the index of the first bin of each row
 *
 */
void contour(int *data, int *filtered_data, int *out_data, int nbins, int nrows, const int *nbins_in_row, const int *basebins) {
    int *pixel_in_contour = malloc(sizeof(int) * nbins);
    memset(pixel_in_contour, 0, nbins * sizeof(int));
    Contour *head = NULL;
    Contour *current = NULL;
    for (int i = 2; i < nrows - 2; i++) {
        for (int j = basebins[i] + 2; j < basebins[i] + nbins_in_row[i] - 2; j++) {
            if (data[j] && !pixel_in_contour[j]) {
                pixel_in_contour[j] = 1;
                ContourPoint * point = new_contour_point(NULL, j, 0);
                int length = follow_contour(point, data, filtered_data, pixel_in_contour, i, nrows, basebins, nbins_in_row);
                if (head == NULL) {
                    current = new_contour(NULL, j);
                    current->length = length;
                    current->first_point = point;
                    head = current;
                } else {
                    current = new_contour(current, j);
                    current->length = length;
                    current->first_point = point;
                }
            }
        }
    }
    free(pixel_in_contour);
    while (head != NULL) {
        if (head->length < 15) {
            head = del_contour(head);
        } else {
            ContourPoint *point = head->first_point;
            while (point != NULL) {
                out_data[point->bin] = 1;
                ContourPoint *tmp = point;
                point = point->next;
                free(tmp);
            }
            Contour *tmp_contour = head;
            head = head->next;
            free(tmp_contour);
        }
    }
}