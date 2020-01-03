#include "contour.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include "helpers.h"

int mod(int a, int n) {
    return a - floor(a / n) * n;
}

const int ANGLES[9] = {135, 90, 45,
                       180, 360, 0,
                       225, 270, 315};

struct node {
    struct subnode *child;
    struct node *prev;
    struct node *next;
    int length;
};

struct subnode {
    int bin;
    int entryAngle;
    struct subnode *prev;
    struct subnode *next;
};

struct gradientVector{
    double x;
    double y;
};

void replaceFillValue(int *window, int fillValue) {
    int counter = 0;
    for (int i = 0; i < 9; i++) {
        if (window[i] == fillValue) {
            counter++;
        }
    }
    if (counter > 0) {
        int *validValues = (int *) malloc(sizeof(int) * 9 - counter);
        int validCount = 0;
        for (int i = 0; i < 9; i++) {
            if (window[i] != fillValue) {
                validValues[validCount] = window[i];
                validCount++;
            }
        }
        int mdn = median(validValues, 9 - counter);
        free(validValues);
        for (int i = 0; i < 9; i++) {
            if (window[i] == fillValue) {
                window[i] = mdn;
            }
        }
    }
}

/**
 * Applies a sobel operator to a bin
 * @param window pointer to an array of length 9 containing data values representing a 3x3 window
 * @return structure containing the direction and magnitude of the gradient
 */
struct gradientVector gradient(int *window, int fillValue) {
    replaceFillValue(window, fillValue);
    struct gradientVector g;
    g.x = (double) (window[5] - window[3]) / 2;
    g.y = (double) (window[7] - window[1]) / 2;
    return g;
}

/**
 * Calculates if the angle of the next turn represents a greater than 90 degrees turn over the course of the
 * previous 3 contour pixels
 * @param tail the node in the linked list representing the most recent pixel in the contour
 * @param nextTheta the angle between the last pixel and the next pixel in question
 * @return 1 if the change in direction is greather than 90, 0 if otherwise
 */
bool isSharpTurn(struct subnode *tail, int nextTheta) {
    int counter = 0;
    int dtheta = 0;
    struct subnode *tmp = tail;
    int prevTheta = 0;
    while (tail->prev != NULL && counter < 3) {
        prevTheta = tmp->entryAngle;
        dtheta = tmp->entryAngle - nextTheta;
        dtheta = mod(dtheta + 180, 360) - 180;
        dtheta = dtheta < -180 ? dtheta + 360 : dtheta;
        dtheta = abs(dtheta);
        if (dtheta > 90)
            return true;
        counter++;
    }
    return false;
}

/**
 * Calculates the ratio between the magnitude of the sum of the gradients and the sum of the magnitude of the gradients.
 * Takes a 5x5 window of values and calculates the gradient for each pixel in the 3x3 window in the center of the larger
 * window, and calculates the ratio.
 * @param window a 5x5 window containing the data for which the gradient is to be calculated
 * @param fillValue value that is used in place of missing or invalid values
 * @return the ratio between the magnitude of the sum of the gradients and the sum of the magnitude of the gradients
 */
double getGradientRatio(const int *window, int fillValue) {
    double sumMagnitude = 0;
    double sumX = 0;
    double sumY = 0;
    for (int i = 1; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
            int *dataWindow = malloc(9 * sizeof(int));
            for (int k = 0; k < 3; k++) {
                for (int m = 0; m < 3; m++) {
                    dataWindow[k * 3 + m] = window[(i - 1 + k) * 3 + (j - 1 + m)];
                }
            }
            struct gradientVector g = gradient(dataWindow, fillValue);
            sumMagnitude += sqrt(pow(g.x, 2) + pow(g.y, 2));
            sumX += g.x;
            sumY += g.y;
            free(dataWindow);
        }
    }
    return sqrt(pow(sumX, 2) + pow(sumY, 2)) / sumMagnitude;
}

/**
 * Adds the next pixel to the end of the provided contour
 * @param bin
 * @param row
 * @param bins
 * @param inData
 * @param filteredData
 * @param isInContour
 * @param nBinsInRow
 * @param basebins
 * @param fillValue
 * @param tail
 * @return
 */
int followContour(int bin, int row, int *bins, const int *inData, const int *filteredData, bool *isInContour,
                  const int *nBinsInRow, const int *basebins, int fillValue, struct subnode *tail) {
    int count = 1;
    int nextContourPixel = -1;
    int minDTheta = 180;
    int nextAngle = 0;
    int dtheta = 0;
    int *binWindow = malloc(9 * sizeof(int));
    getWindow(bin, row, 3, bins, nBinsInRow, basebins, binWindow, fillValue, false);
    for (int i = 0; i < 9; i++) {
        if (i == 4)
            continue;
        if (inData[binWindow[i] - 1] > 0) {
            if (tail->entryAngle == -999) {
                dtheta = 0;
            } else {
                dtheta = tail->entryAngle - ANGLES[i];
                dtheta = mod(dtheta + 180, 360) - 180;
                dtheta = dtheta < -180 ? dtheta + 360 : dtheta;
                dtheta = abs(dtheta);
            }
            if (dtheta < minDTheta) {
                minDTheta = dtheta;
                nextAngle = ANGLES[i];
                nextContourPixel = binWindow[i];
            }
        }
    }
    if (isSharpTurn(tail, nextAngle)) {
        nextContourPixel = -1;
    }
    double ratio = 0;

    if (nextContourPixel == -1) {
        int *dataWindow = malloc(25 * sizeof(int));
        getWindow(bin, row, 5, filteredData, nBinsInRow, basebins, dataWindow, fillValue, false);
        ratio = getGradientRatio(dataWindow, fillValue);
        free(dataWindow);
        if (ratio > 0.7) {
            double maxProduct = -1;
            double product;
            struct gradientVector gradient1;
            int maxIndex = -1;
            int *smallWindow = malloc(9 * sizeof(int));
            getWindow(bin, row, 3, filteredData, nBinsInRow, basebins, smallWindow, fillValue, false);
            struct gradientVector gradient0 = gradient(smallWindow, fillValue);
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (i == 1 && j == 1) {
                        continue;
                    }
                    getWindow(binWindow[i * 3 + j], row + j - 1, 3, filteredData,
                              nBinsInRow, basebins, smallWindow, fillValue, false);
                    gradient1 = gradient(smallWindow, fillValue);
                    product = (gradient0.x * gradient1.x) + (gradient0.y * gradient1.y);
                    if (product > maxProduct) {
                        maxProduct = product;
                        maxIndex = i * 3 + j;
                    }
                }
            }
            if (maxProduct > 0) {
                nextAngle = ANGLES[maxIndex];
                nextContourPixel = binWindow[maxIndex];
            }
            free(smallWindow);
        }
    }

    free(binWindow);
    int nextRow;
    if (nextAngle == 0 || nextAngle == 180) {
        nextRow = row;
    } else if (nextAngle > 0 && nextAngle < 180) {
        nextRow = row - 1;
    } else if (nextAngle > 180) {
        nextRow = row + 1;
    } else {
        nextRow = row;
    }

    if (nextContourPixel != -1 && !isInContour[nextContourPixel - 1]) {
        isInContour[nextContourPixel - 1] = true;
        struct subnode *tmp = malloc(sizeof(struct subnode));
        tail->next = tmp;
        tmp->prev = tail;
        tmp->entryAngle = nextAngle;
        tmp->bin = nextContourPixel;
        tmp->next = NULL;
        count += followContour(nextContourPixel, nextRow, bins, inData, filteredData, isInContour, nBinsInRow, basebins,
                               fillValue, tmp);
    }

    return count;
}

void clearSubList(struct node *head) {
    struct subnode *subhead = head->child;
    struct subnode *tmp = NULL;
    head->child = NULL;
    while (subhead != NULL) {
        tmp = subhead->next;
        free(subhead);
        subhead = tmp;
    }
}

void trim(struct node *head) {
    struct node *current = head;
    struct node *next;
    struct node *prev;
    while (current != NULL) {
        if (current->length < 4) {
            clearSubList(current);
            prev = current->prev;
            next = current->next;
            if (prev != NULL) {
                prev->next = next;
                if (next != NULL) {
                    next->prev = prev;
                }
            }
            if (current == head) {
                head = NULL;
                free(head);
            } else
                free(current);
            current = next;
        } else {
            current = current->next;
        }
    }
}

/**
 * Implements a contour following algorithm. Goes through each bin, and if the bin has been previously detected as
 * an edge, then follow neighboring bin
 * @param bins
 * @param inData
 * @param outData
 * @param ndata
 * @param nrows
 * @param nBinsInRow
 * @param basebins
 * @param fillValue
 */
void contour(int *bins, int *inData, int *filteredData, int *outData, int ndata, int nrows, const int *nBinsInRow,
             const int *basebins, int fillValue) {
    int row = 0;
    struct node *head = NULL;
    struct node *tail = NULL;
    struct node *tmp = NULL;
    head = (struct node *) malloc(sizeof(struct node));
    tail = head;
    head->child = NULL;
    head->next = NULL;
    head->length = 0;
    head->prev = NULL;
    bool *isInContour = (bool *) malloc(sizeof(bool) * ndata);
    for (int i = 0; i < ndata; i++) {
        isInContour[i] = false;
    }
    for (int i = 0; i < ndata; i++) {
        if (row < 2 || row > nrows - 3) {
            if (i == basebins[row] + nBinsInRow[row] - 1) {
                row++;
            }
            continue;
        }

        if (inData[i] > 0 && !isInContour[i]) {
            isInContour[i] = true;
            if (tail->child != NULL) {
                tmp = (struct node *) malloc(sizeof(struct node));
                tail->next = tmp;
                tmp->prev = tail;
                tail = tmp;
            }
            tail->child = (struct subnode *) malloc(sizeof(struct subnode));
            tail->next = NULL;
            tail->child->prev = NULL;
            tail->child->next = NULL;
            tail->child->entryAngle = -999;
            tail->child->bin = i + 1;
            tail->length = followContour(i + 1, row, bins, inData, filteredData, isInContour, nBinsInRow, basebins,
                                         fillValue, tail->child);
        }
        if (i == basebins[row] + nBinsInRow[row] - 1) {
            row++;
        }
    }
    free(isInContour);
    struct node *current = head;
    struct subnode *child = NULL;
    struct subnode *tmpchild = NULL;
    trim(head);
    while (current != NULL) {
        child = current->child;
        while (child != NULL) {
            outData[(child->bin) - 1] = 1;
            tmpchild = child->next;
            free(child);
            child = tmpchild;
        }
        tmp = current->next;
        free(current);
        current = tmp;
    }
}