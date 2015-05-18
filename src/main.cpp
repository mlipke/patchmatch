#include <iostream>
#include <limits>
#include <opencv2/highgui/highgui.hpp>

#include "main.h"

using namespace std;
using namespace cv;

Mat random_flow(int width, int height) {
    Mat rf(height, width, CV_32SC2, Scalar(0,0));

    for (int i = 0; i < rf.rows; i++) {
        Vec2i *p = rf.ptr<Vec2i>(i);
        for (int j = 0; j < rf.cols; j++) {
            Vec2i c = p[j];

            c[0] = (rand() % (width * 2)) - width;
            c[1] = (rand() % (height * 2)) - height;

            rf.at<Vec2i>(i, j) = c;
        }
    }

    return rf;
}

int ssd(Mat *image_one, Mat *image_two, Point c, Vec2i offset, int window_size) {
    int sum = 0, count = 0;

    int width = image_one->cols, height = image_one->rows;

    if (boundaries(c, width, height) || boundaries(Point(c.x + offset[0], c.y + offset[1]), width, height))
        return numeric_limits<int>::max();

    for (int i = -window_size; i <= window_size; i++) {
        int y_one = c.y + i;
        int y_two = offset[1] + i;

        if (y_one < image_one->rows && y_two < image_two->rows
           && y_one > 0 && y_two > 0) {

            Vec3b *p_one = image_one->ptr<Vec3b>(y_one);
            Vec3b *p_two = image_two->ptr<Vec3b>(y_two);
            for (int j = -window_size; j <= window_size; j++) {
                int b_diff = 0;
                int g_diff = 0;
                int r_diff = 0;

                int x_one = c.x + j;
                int x_two = offset[0] + j;

                if (y_one < image_one->cols && y_two < image_two->cols
                    && y_one > 0 && y_two > 0) {

                    b_diff = p_one[x_one][0] - p_two[x_two][0];
                    g_diff = p_one[x_one][1] - p_two[x_two][1];
                    r_diff = p_one[x_one][2] - p_two[x_two][2];

                    sum += b_diff * b_diff + g_diff * g_diff + r_diff * r_diff;

                    count += 1;
                }
            }
        }
    }

    if (count == 0 || sum == 0) {
        return 0;
    } else {
        return sum / count;
    }
}

void propagate(Mat *image_one, Mat *image_two, Mat *flow, int direction, int window_size) {
    int width = image_one->cols;
    int height = image_two->rows;

    int it_x, it_y;
    for (int i = 0; i < height; i++) {
        if (direction == 0) { it_y = i; } else { it_y = height - 1 - i; }

        Vec2i *c_ptr = flow->ptr<Vec2i>(it_y);
        for (int j = 0; j < width; j++) {
            if (direction == 0) { it_x = j; } else { it_x = width - 1 - j; }
            Vec2i c_offset = c_ptr[it_x];

            Vec2i n_one_offset, n_two_offset;
            Point c = Point(it_x, it_y), n_one_c, n_two_c;

            if (direction == 0) {
                n_one_c = Point(it_x + 1, it_y);
                n_two_c = Point(it_x, it_y + 1);
            } else {
                if (it_x > 1 && it_y > 1) {
                    n_one_c = Point(it_x - 1, it_y);
                    n_two_c = Point(it_x, it_y - 1);
                } else {
                    n_one_c = c;
                    n_two_c = c;
                }
            }

            n_one_offset = flow->at<Vec2i>(n_one_c);
            n_two_offset = flow->at<Vec2i>(n_two_c);

            int ssd_c = ssd(image_one, image_two, c, c_offset, window_size);
            int ssd_one = ssd(image_one, image_two, c, n_one_offset, window_size);
            int ssd_two = ssd(image_one, image_two, c, n_two_offset, window_size);

            int ssd_min = min(ssd_c, ssd_one, ssd_two);

            if (ssd_min == ssd_c)
                flow->at<Vec2i>(c) = c_offset;

            if (ssd_min == ssd_one)
                flow->at<Vec2i>(c) = n_one_offset;

            if (ssd_min == ssd_two)
                flow->at<Vec2i>(c) = n_two_offset;

            //random_search(image_one, image_two, flow, c, window_size, 64, ssd_min);
        }
    }
}

void random_search(Mat *image_one, Mat *image_two, Mat *flow, Point p, int window_size, int radius, int current_ssd) {
    Vec2i r_offset = random_offset(p, radius, image_one->cols, image_one->rows);

    int r_ssd = ssd(image_one, image_two, p, r_offset, window_size);

    if (r_ssd < current_ssd) {
        flow->at<Vec2i>(p) = r_offset;
        current_ssd = r_ssd;
    }

    if (radius > window_size) {
        random_search(image_one, image_two, flow, p, window_size, radius / 2, current_ssd);
    }
}

Vec2i random_offset(Point p, int radius, int max_x, int max_y) {
    int rand_x = rand() % radius - radius / 2;
    int rand_y = rand() % radius - radius / 2;

    if (rand_x < 0) rand_x = 0;
    if (rand_y < 0) rand_y = 0;

    if (rand_x > max_x) rand_x = max_x;
    if (rand_y > max_y) rand_y = max_y;

    return Vec2i(rand_x, rand_y);
}

bool boundaries(Point c, int max_x, int max_y) {
    return c.x < 0 || c.y < 0 || c.x > max_x || c.y > max_y;
}


int min(int a, int b, int c) {
    int s = a;

    if (b < s) s = b;
    if (c < s) s = c;

    return s;
}

void patchmatch(Mat *image_one, Mat *image_two, Mat *flow, int window_size, int iterations) {
    for (int i = 0; i < iterations; i++) {
        int direction;
        if (i % 2 == 0) { direction = 0; } else { direction = 1; }

        propagate(image_one, image_two, flow, direction, window_size);

        cout << i + 1 << " iteration(s) complete" << endl;
    }
}

void pyramid(Mat *image_one, Mat *image_two, int window_size) {

}

Mat warp_image(Mat *image, Mat *flow) {
    Mat warped_image(image->rows, image->cols, CV_8UC3, Scalar(0));

    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Vec2i location = flow->at<Vec2i>(i, j);

            Vec3b value = image->at<Vec3b>(i + location[1], j + location[0]);
            warped_image.at<Vec3b>(i, j) = value;
        }
    }

    return warped_image;
}


int main(int argc, const char ** argv) {
    if (argc < 2)
        return 0;

    Mat image_one, image_two;
    image_one = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    image_two = imread(argv[3], CV_LOAD_IMAGE_COLOR);

    int window_size = atoi(argv[1]);

    Mat rf = random_flow(image_one.cols, image_one.rows);
    patchmatch(&image_one, &image_two, &rf, window_size, 1);

    imwrite("warped_image.png", warp_image(&image_one, &rf));

    return 0;
}

