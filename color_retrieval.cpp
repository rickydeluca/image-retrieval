#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int compareColorSpace(Mat img, String img_database_path) {
	// convert rgb to hsv
	Mat hsv_img;
	cvtColor(img, hsv_img, COLOR_BGR2HSV);

	// split channels of the image
	vector<Mat> channels;
	split(hsv_img, channels);
	Mat h0 = channels[0];
	Mat s0 = channels[1];
	Mat v0 = channels[2];

	/*
	// quantify hsv histogram using inRange
	Mat img_threshold;

	// first set
	int low_h  = 157;
	int high_h = 179;

	int low_s  = 0;
	int high_s = 51;

	int low_v  = 0;
	int high_v = 51;

	inRange(hsv_img, Scalar(low_h, low_s, low_v), Scalar(high_h, high_s, high_v), img_threshold);

	return img_threshold;
	*/

	// read the images from database
	VideoCapture cap("/home/rickydeluca/Scrivania/università/tesi/image_retrieval/image_database/img_%3d.JPG"); // %3d means 001.JPG notation
	if (!cap.isOpened()) // check if succeeded
		return -1;

	Size size(800, 600);
	vector<Mat> database_channels;
	Mat h1, s1, v1;		// channels for database_img
	int dh, ds, dv; 	// distances from color spaces
	float gcs_distance;	// global color space distance
	for (;;) {
		Mat database_img;
		cap >> database_img;
		resize(database_img, database_img, size);				// resize the image
		cvtColor(database_img, database_img, COLOR_BGR2HSV);	// convert to hsv colorspace

		// split channels of the database_img
		split(database_img, database_channels);
		h1 = database_channels[0];
		s1 = database_channels[1];
		v1 = database_channels[2];

		// compute euclidian distance between img and database_img
		dh = min(abs(h1-h0), 179 - abs(h1-h0));
		ds = abs(s1-s0);
		dv = abs(v1-v0);

		gcs_distance = sqrt(dh*dh + ds*ds + dv*dv);

		// insert found distance in distance_array
	}

	// TODO
}


int main (int argc, char** argv) {
	String img_database_path =  "image_database/";
	Mat img = imread("image_database/img_016.JPG", IMREAD_COLOR); // happy kiki

	// resize the image
	Size size(800, 600);
	resize (img, img, size);

	printf("Calling compareColorSpace\n\n");
	Mat ret = compareColorSpace(img, img_database_path);

	// dispalay images
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", ret);

    // wait for a keystroke in the window
    int k = waitKey(0);
    return 0;

}