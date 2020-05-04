#include "unity.h"
#include <stdlib.h>
#include "contour.h"
#include "helpers.h"


void setUp(void) {
}

void tearDown(void) {
}

void test_contour_gradient_ratio(void) {
    int arr[25] = { 50,  83, 100, 248, 118,
                    110,  67,  95, 168, 149,
                    195,  58,  21, 204, 215,
                    118,  84, 154,  22, 211,
                    113,  64, 126,  97, 235};
    TEST_ASSERT_EQUAL_DOUBLE(0.3690171758724715, gradient_ratio(arr));
}

void test_contour_new_contour_point(void) {
    ContourPoint prev = {4, 0, NULL, NULL};
    ContourPoint *new_point = new_contour_point(&prev, 12, 0);
    TEST_ASSERT_EQUAL_PTR(&prev, new_point->prev);
    TEST_ASSERT_EQUAL_INT(12, new_point->bin);
    TEST_ASSERT_EQUAL_INT(0, new_point->angle);
    TEST_ASSERT_NULL(new_point->next);
    free(new_point);
}

void test_contour_del_contour(void) {
    Contour *c1 = malloc(sizeof(Contour));
    c1->prev = NULL;
    c1->length = 12;
    c1->first_point = NULL;
    Contour *c2 = malloc(sizeof(Contour));
    c1->next = c2;
    c2->prev = c1;
    c2->length = 10;
    ContourPoint *point1 = malloc(sizeof(ContourPoint));
    point1->bin = 10;
    point1->prev = NULL;
    point1->angle = 0;

    ContourPoint *point2 = malloc(sizeof(ContourPoint));
    point2->bin = 10;
    point2->prev = point1;
    point2->angle = 0;
    point1->next = point2;

    ContourPoint *point3 = malloc(sizeof(ContourPoint));
    point3->bin = 10;
    point3->prev = point2;
    point3->angle = 0;
    point2->next = point3;

    ContourPoint *point4 = malloc(sizeof(ContourPoint));
    point4->bin = 10;
    point4->prev = point3;
    point4->angle = 0;
    point4->next = NULL;

    c2->first_point = point1;

    Contour *c3 = malloc(sizeof(Contour));
    c2->next = c3;
    c3->prev = c2;
    c3->length = 4;
    c3->first_point=NULL;

    c2 = del_contour(c2);
    TEST_ASSERT_EQUAL_PTR(c2, c3);
    TEST_ASSERT_EQUAL_PTR(c1->next, c3);
    TEST_ASSERT_EQUAL_PTR(c3->prev, c1);
}


void test_contour_find_best_front(void) {
    int data[81] = {
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0
    };
    int basebins[9];
    int nbins_in_row[9];
    for (int i = 0; i < 9; i++) {
        basebins[i] = i * 9;
        nbins_in_row[i] = 9;
    }
    ContourPoint point = {13, 1, NULL, NULL};
    ContourPoint *point2 = find_best_front(&point, data, 1, basebins, nbins_in_row);

    TEST_ASSERT_EQUAL_INT(22, point2->bin);
    TEST_ASSERT_EQUAL_INT(270, point2->angle);

    ContourPoint *point3 = find_best_front(point2, data, 2, basebins, nbins_in_row);
    TEST_ASSERT_EQUAL_INT(31, point3->bin);
    TEST_ASSERT_EQUAL_INT(270, point3->angle);

    ContourPoint *point4 = find_best_front(point3, data, 3, basebins, nbins_in_row);

    TEST_ASSERT_EQUAL_INT(39, point4->bin);
    TEST_ASSERT_EQUAL_INT(225, point4->angle);

    ContourPoint *point5 = find_best_front(point4, data, 4, basebins, nbins_in_row);

    TEST_ASSERT_EQUAL_INT(38, point5->bin);
    TEST_ASSERT_EQUAL_INT(180, point5->angle);

    ContourPoint *point6 = find_best_front(point5, data, 4, basebins, nbins_in_row);
    TEST_ASSERT_NULL(point6);

    while (point2->next != NULL) {
        ContourPoint *tmp = point2->next;
        free(point2);
        point2 = tmp;
    }
}

void test_contour_follow_contour(void) {
    int data[81] = {
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 0, 1, 0, 0, 0, 0,
            0, 0, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 0, 0, 0, 0
    };
    int basebins[9];
    int nbins_in_row[9];
    for (int i = 0; i < 9; i++) {
        basebins[i] = i * 9;
        nbins_in_row[i] = 9;
    }
    int filtered_data[81] = {
            100, 100, 100, 100, 100, 100, 100, 100, 100,
            100, 45, 100, 100, 100, 100, 100, 100, 100,
            30,  90,  200, 200, 200, 100, 100, 100, 100,
            30,  90,  200, 200, 200, 100, 100, 100, 100,
            30,  90, 126, 200, 200, 100, 100, 100, 100,
            90, 90, 100, 200, 200, 100, 100, 100, 100,
            100, 100, 100, 200, 200, 100, 100, 100, 100,
            100, 100, 100, 100, 100, 100, 100, 100, 100,
            100, 100, 100, 100, 100, 100, 100, 100, 100
    };
    int pixel_in_contour[81] = { 0 };
    pixel_in_contour[13] = 1;
    ContourPoint point = {13, 1, NULL, NULL};
    int count = follow_contour(&point, data, filtered_data, pixel_in_contour, 1, 9, basebins, nbins_in_row);

    ContourPoint *pt = point.next;
    ContourPoint *tmp;
    int c = 0;
    while (tmp->next != NULL) {
        tmp = pt->next;
        free(pt);
        pt = tmp;
        c++;
    }
    TEST_ASSERT_EQUAL_INT(6, count);
    TEST_ASSERT_EQUAL_INT(1, pixel_in_contour[28]);
    TEST_ASSERT_EQUAL_INT(28, pt->bin);
    free(pt);
}
/*
void test_contour_NeedToImplement(void)
{
    TEST_IGNORE_MESSAGE("Need to Implement contour");
}*/
