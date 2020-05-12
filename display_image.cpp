#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
	// read the file
	Mat img;
	Mat img_resized;
	Size size(800, 600);
	img = imread("image_database/img_001.JPG", IMREAD_COLOR);	// reading first image of the database
	resize(img, img_resized, size);	// resize the image

	// check for invalid input
	if (img_resized.empty()) {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    // create a window and show the image inside it
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", img_resized);

    // wait for a keystroke in the window
    int k = waitKey(0);
    return 0;
}