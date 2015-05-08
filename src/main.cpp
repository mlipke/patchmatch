#include <iostream>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Mat random_flow(int width, int height) {
    Mat rf(width, height, CV_32SC2, Scalar(0,0));

    for (int i = 0; i < rf.cols; i++) {
        Vec2i *p = rf.ptr<Vec2i>(i);
        for (int j = 0; j < rf.rows; j++) {
            Vec2i c = p[j];

            c[0] = rand() % width;
            c[1] = rand() % height;

            rf.at<Vec2i>(i, j) = c;
        }
    }

    return rf;
}

int ssd(Mat *image_one, Mat *image_two, Point c, Point offset, int window_size) {
    int sum = 0, count = 0;

    for (int i = -window_size; i <= window_size; i++) {
        int x_one = c.x + i;
        int x_two = offset.x + i;

        if (!(x_one > image_one->cols || x_two > image_two->cols
           || x_one < 0               || x_two < 0)) {

            uchar *p_one = image_one->ptr<uchar>(x_one);
            uchar *p_two = image_two->ptr<uchar>(x_two);
            for (int j = -window_size; j <= window_size; j++) {
                int diff = 0;

                int y_one = c.y + j;
                int y_two = offset.y + j;

                if (!(y_one > image_one->rows || y_two > image_two->rows
                   || y_one < 0 || y_two < 0)) {
                    diff = p_one[c.y + j] - p_two[offset.y + j];
                    sum += diff * diff;

                    count += 1;
                }
            }
        }
    }

    return sum / count;
}

void patchmatch(Mat *image_one, Mat *image_two) {
    Mat random_flow;
}

int main(int argc, const char ** argv) {
    if (argc < 2)
        return 0;

    Mat image_one, image_two;
    image_one = imread(argv[2], CV_LOAD_IMAGE_GRAYSCALE);
    image_two = imread(argv[3], CV_LOAD_IMAGE_GRAYSCALE);

    int window_size = atoi(argv[1]);

    Mat rf = random_flow(5, 5);

    cout << "Random Flow: " << endl << rf << endl;

    return 0;
}

