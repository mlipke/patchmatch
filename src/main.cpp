#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace std;
using namespace cv;

Mat random_flow(Mat *image) {
    Mat rf(image->cols, image->rows, CV_32SC2, Scalar(0,0));

    for (int i = 0; i < rf.cols; i++) {
        Vec2s *p = rf.ptr<Vec2s>(i);
        for (int j = 0; j < rf.rows; j++) {
            Vec2s c = p[j];
        }
    }

    return rf;
}

int ssd(Mat *image_one, Mat *image_two, Point c, Point offset, int window_size) {
    int sum = 0, count = 0;

    for (int i = -window_size; i <= window_size; i++) {
        int x_one = c.x + i;
        int x_two = offset.x + i;

        if (!(x_one > image_one->cols || x_two > image_two->cols)) {

            uchar *p_one = image_one->ptr<uchar>(x_one);
            uchar *p_two = image_two->ptr<uchar>(x_two);
            for (int j = -window_size; j <= window_size; j++) {
                int diff = 0;

                int y_one = c.y + j;
                int y_two = offset.y + j;

                if (!(y_one > image_one->rows || y_two > image_two->rows)) {
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

    int s = ssd(&image_one, &image_two, Point(0,0), Point(40,40), window_size);

    cout << "Sum: " << s << endl;

    return 0;
}

