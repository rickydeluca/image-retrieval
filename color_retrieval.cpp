#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

Mat compareColorSpace(Mat img, String img_database_path) {
	// convert rgb to hsv
	Mat hsv_img;
	cvtColor(img, hsv_img, cv.COLOR_BGR2HSV);

	// quantify hsv histogram
	Mat hsv_planes[3];
	split(hsv_img, hsv_planes); // separete the h, s, v channels
	Mat h_plane = hsv_planes[0];
	Mat s_plane = hsv_planes[1];
	Mat v_plane = hsv_planes[2];

	Mat hsv_quantified[3];
	int x, y;
	for (x = 0; x < hsv_img.height; x++) {
		for (y = 0; y < hsv_img.width; y++) {
			Vec3f h_value = h_plane.at<Vec3f>(y, x);
			
			if (h_value >= 316 && h_value <= 360) {
				hsv_quantified[0].at<Vec3f>(y, x) = 0;
			}

			else if (h_value >= 1 && h_value <= 25) {
				hsv_quantified[0].at<Vec3f>(y, x) = 1;
			}

			else if (h_value >= 26 && h_value <= 40) {
				hsv_quantified[0].at<Vec3f>(y, x) = 2;
			}

			else if (h_value >= 41 && h_value <= 120) {
				hsv_quantified[0].at<Vec3f>(y, x) = 3;
			}

			else if (h_value >= 121 && h_value <= 190) {
				hsv_quantified[0].at<Vec3f>(y, x) = 4;
			}

			else if (h_value >= 191 && h_value <= 270) {
				hsv_quantified[0].at<Vec3f>(y, x) = 5;
			}

			else if (h_value >= 271 && h_value <= 295) {
				hsv_quantified[0].at<Vec3f>(y, x) = 6;
			}

			else if (h_value >= 296 && h_value <= 315) {
				hsv_quantified[0].at<Vec3f>(y, x) = 7;
			}

			Vec3f s_value = s_plane.at<Vec3f>(y, x);

			if (s_value >= 0 && s_value < 0.2) {
				hsv_quantified[1].at<Vec3f>(y, x) = 0;
			}

			else if (s_value >= 0.2 && s_value < 0.7) {
				hsv_quantified[1].at<Vec3f>(y, x) = 2;
			}

			else if (s_value >= 0.7 && s_value <= 1) {
				hsv_quantified[1].at<Vec3f>(y, x) = 3;
			}

			Vec3f v_value = v_plane.at<Vec3f>(y, x);

			if (v_value >= 0 && v_value < 0.2) {
				hsv_quantified[2].at<Vec3f>(y, x) = 0;
			}

			else if (v_value >= 0.2 && v_value < 0.7) {
				hsv_quantified[2].at<Vec3f>(y, x) = 2;
			}

			else if (v_value >= 0.7 && v_value <= 1) {
				hsv_quantified[2].at<Vec3f>(y, x) = 3;
			}

		} 
	}

	// scan the database convert each image 
}


int main (int argc, char** argv) {
	// read an image and display the red channel
	String img_database_path =  "image_database/";
	Mat img = imread("image_database/img_017.JPG", IMREAD_COLOR);

	// resize image
	Size size(800, 600);
	resize(img, img, size);

	Mat planes[3];
	split(img, planes);

	// convert rgb to hsv
	Mat hsv_img;
	cvtColor(img, hsv_img, COLOR_BGR2HSV);

	// dispalay images
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", planes[2]);

    // wait for a keystroke in the window
    int k = waitKey(0);
    return 0;
}