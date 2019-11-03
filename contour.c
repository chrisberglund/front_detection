//
// Created by Christopher Berglund on 11/1/19.
//

#include "contour.h"
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "helpers.h"

const int ANGLES[9] = {135, 90, 45, 180, 360, 0, 225, 270, 315};

struct node {
    struct subnode* child;
    struct node* prev;
    struct node* next;
    int length;
};

struct subnode {
    int bin;
    int entryAngle;
    struct subnode* prev;
    struct subnode* next;
};


/**
 * Performs a convolution with provided kernel on a given window of same size dimensions.
 * @param window pointer to an array of size * size length containing data values to perform convolution on
 * @param kernel pointer to an array of size * size length containing convolution kernel
 * @param size length of one dimension of the window
 * @return value resulting from convolution
 */
int convolution(const int *window, const int *kernel, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += window[i] * kernel[i];
    }
    return sum;
}

struct gradient {
    double x;
    double y;
};

/**
 * Applies a sobel operator to a bin
 * @param window pointer to an array of length 9 containing data values representing a 3x3 window
 * @return structure containing the direction and magnitude of the gradient
 */
struct gradient sobel(int *window) {
    int GX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int GY[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    struct gradient g;
    g.x = convolution(window, GX, 9);
    g.y = convolution(window, GY, 9);
    return g;
}


bool isSharpTurn(struct subnode *tail, int nextTheta) {
    int counter = 0;
    double dtheta = 0;
    struct subnode *tmp = tail;
    double prevTheta = 0;
    while (tail -> prev != NULL && counter < 3) {
        prevTheta = tmp -> entryAngle;
        dtheta = nextTheta - prevTheta;
        if (dtheta < 0 )
            dtheta += 360;
        if (dtheta > 90)
            return true;
        counter++;
    }
    return false;
}

double getGradientRatio(int *window) {
    struct gradient g = sobel(window);
    double sumMagnitude = 0; double sumX = 0; double sumY = 0;
    for (int i = 0; i < 9; i ++) {
        struct gradient g = sobel(window);
        sumMagnitude += sqrt(pow(g.x, 2) + pow(g.y, 2));
        sumX += g.x;
        sumY += g.y;
    }
    return sqrt(pow(sumX, 2) + pow(sumY, 2)) / sumMagnitude;

}

int followContour(int bin, int row, int *bins, const int *inData, const int *nBinsInRow, const int *basebins,
        int fillValue, struct subnode *tail) {
    int count = 1;
    int nextContourPixel = -1;
    int minDTheta = 180;
    int nextAngle = 0;
    int dtheta;
    int *binWindow = malloc(9 * sizeof(int));
    getWindow(bin, row, 3, bins, nBinsInRow, basebins, binWindow, fillValue, false);
    for (int i = 0; i < 9; i++) {
        if (i == 4)
            continue;
        if (inData[binWindow[i] - 1] > 0) {
            if (tail -> entryAngle == -999)
                dtheta = 0;
            else
                dtheta = tail -> entryAngle - ANGLES[i];
            if (dtheta < 0)
                dtheta += 360;
            if (dtheta < minDTheta) {
                minDTheta = dtheta;
                nextAngle = ANGLES[i];
                nextContourPixel = binWindow[i];
            }
        }
    }

    if (isSharpTurn(tail, minDTheta)) {
        nextContourPixel = -1;
    }
    double ratio;
    double maxRatio = 0;
    int maxIndex = 0;
    if (nextContourPixel == -1) {
        int *dataWindow = malloc(9 * sizeof(int));
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                switch (j) {
                    case 0:
                        getWindow(binWindow[i*3 + j], row - 1, 3, inData, nBinsInRow, basebins, dataWindow, fillValue, false);
                        break;
                    case 1:
                        getWindow(binWindow[i*3 + j], row, 3, inData, nBinsInRow, basebins, dataWindow, fillValue, false);
                        break;
                    case 2:
                        getWindow(binWindow[i*3 + j], row + 1, 3, inData, nBinsInRow, basebins, dataWindow, fillValue, false);
                        break;
                    default:
                        break;
                }
                ratio = getGradientRatio(dataWindow);
                if (ratio > maxRatio) {
                    maxRatio = ratio;
                    maxIndex = i * 3 + j;
                }
            }
        }
        if (maxRatio > 0.7) {
            nextAngle = ANGLES[maxIndex];
            nextContourPixel = binWindow[maxIndex];
        }
    }
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
    if (nextContourPixel != -1) {
        struct subnode *tmp = malloc(sizeof(struct subnode));
        tail -> next = tmp;
        tmp -> prev = tail;
        tmp -> entryAngle = ANGLES[maxIndex];
        tmp -> bin = nextContourPixel;
        tmp -> next = NULL;
        count += followContour(nextContourPixel, nextRow, bins, inData, nBinsInRow, basebins, fillValue, tail);
    }
    free(binWindow);
    return count;
}

void clearSubList(struct node*head) {
    struct subnode *subhead = head -> child;
    struct subnode *tmp = NULL;
    while (subhead != NULL) {
        tmp = subhead -> next;
        free(subhead);
        subhead = tmp;
    }
}

void trim(struct node *head) {
    struct node *current = head;
    struct node *next;
    struct node *prev;
    while (current != NULL) {
        if (current -> length < 15) {
            clearSubList(current);
            prev = current -> prev;
            next = current -> next;
            prev -> next = next;
            next -> prev = prev;
            free(current);
            current = next;
        } else {
            current = current -> next;
        }
    }
}

void contour(int *bins, int *inData, int *outData, int ndata, int nrows, const int *nBinsInRow,
             const int *basebins, int fillValue) {
    int row = 0;
    struct node *head = NULL;
    struct node *tail = NULL;
    struct node *tmp = NULL;
    head = (struct node*) malloc(sizeof(struct node));
    tail = head;
    for (int i = 0; i < ndata; i++) {
        if (row < 2 || row > nrows - 3) {
            continue;
        }

        if (inData[i]) {
            tmp = (struct node*) malloc(sizeof(struct node));
            tail -> next = tmp;
            tmp -> prev = tail;
            tail = tmp;
            tail -> child = (struct subnode*) malloc(sizeof(struct subnode));
            tail -> child -> prev  = NULL;
            tail -> child -> entryAngle = -999;
            tail -> child -> bin = i + 1;
            tail -> length  = followContour(i + 1, row, bins, inData, nBinsInRow, basebins, fillValue, tail -> child);
        }
        if (i == basebins[row] + nBinsInRow[row] - 1) {
            row++;
        }
    }
    struct node *current = head;
    struct subnode *child = NULL;
    struct subnode *tmpchild = NULL;
    trim(head);
    while (current != NULL) {
        child = current -> child;
        while (child != NULL) {
            outData[(child -> bin) - 1] = 1;
            tmpchild = child -> next;
            free(child);
            child = tmpchild;
        }
        tmp = current -> next;
        free(current);
        current = tmp;
    }
}