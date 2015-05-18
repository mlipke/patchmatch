//
// Created by matt on 11.05.15.
//

#ifndef PATCHMATCH_MAIN_H
#define PATCHMATCH_MAIN_H

#include <opencv2/highgui/highgui.hpp>

using namespace cv;

Mat random_flow(int width, int height);
Vec2i random_offset(Point p, int radius, int max_x, int max_y);
bool boundaries(Point c, int max_x, int max_y);
int min(int a, int b, int c);

int ssd(Mat *image_one, Mat *image_two, Point c, Vec2i offset, int window_size);
void propagate(Mat *image_one, Mat *image_two, Mat *flow, int direction, int window_size);
void random_search(Mat *image_one, Mat *image_two, Mat *flow, Point p, int window_size, int radius, int current_ssd);

void patchmatch(Mat *image_one, Mat *image_two, Mat *flow, int window_size, int iterations);


#endif //PATCHMATCH_MAIN_H
