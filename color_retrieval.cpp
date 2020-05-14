#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

// retrival by colour
// confront three image and return the two most similar
Mat compareColorSpace(Mat img, Mat img2, Mat img3) {
	// extract RGB values from the img
	Vec3b img_color_space = img.at<Vec3b>(y, x);
	uchar img_blue 	= img_color_space.val[0];
	uchar img_green = img_color_space.val[1];
	uchar img_red 	= img_color_space.val[2];

	Vec3b img2_color_space = img2.at<Vec3b>(y, x);
	uchar img2_blue  = img2_color_space.val[0];
	uchar img2_green = img2_color_space.val[1];
	uchar img2_red 	 = img2_color_space.val[2];

	Vec3b img3_color_space = img3.at<Vec3b>(y, x);
	uchar img3_blue  = img3_color_space.val[0];
	uchar img3_green = img3_color_space.val[1];
	uchar img3_red 	 = img3_color_space.val[2];

	// confront the 3 images and return the most similar to the first image
	uchar dist_red_1_2   = abs(img_red, img2_red);
	uchar dist_green_1_2 = abs(img_green, img2_green);
	uchar dist_blue_1_2  = abs(img_blue, img2_blue);

	uchar color_distance_1_2 = abs(dist_red_1_2, dist_green_1_2, dist_blue_1_2);

	uchar dist_red_1_3   = abs(img_red, img3_red);
	uchar dist_green_1_3 = abs(img_green, img3_green);
	uchar dist_blue_1_3  = abs(img_blue, img3_blue);

	uchar color_distance_1_3 = abs(dist_red_1_3, dist_green_1_3, dist_blue_1_3);

	if (color_distance_1_2 < color_distance_1_3)
		return img2;
	else
		return img3;
}

int main (int argc, char** argv) {
	// read 3 images from database
	Mat img  = imread("image_database/img_017.JPG", IMREAD_COLOR);	// kiki 1
	Mat img2 = imread("image_database/img_001.JPG", IMREAD_COLOR);	// ball
	Mat img3 = imread("image_database/img_018.JPG", IMREAD_COLOR);	// kiki 2

	Mat match_img = compareColorSpace(img, img2, img3); // expect it return img3

	// dispalay images
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", img);

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", match_img);

    // wait for a keystroke in the window
    int k = waitKey(0);
    return 0;
}