#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <math.h>			// matrices operations
#include <bits/stdc++.h>	// sorting arrays

using namespace cv;
using namespace std;

double compareColorSpace(Mat img, String img_database_path, int (&most_similar_imgs_idx_array)[5]) {
	// convert rgb to hsv
	Mat hsv_img;
	cvtColor(img, hsv_img, COLOR_BGR2HSV);

	// split channels of the image
	vector<Mat> channels;
	split(hsv_img, channels);
	Mat h0 = channels[0];
	Mat s0 = channels[1];
	Mat v0 = channels[2];

	// read the images from database
	VideoCapture cap("/home/rickydeluca/Scrivania/università/tesi/image_retrieval/image_database/img_%3d.JPG"); // %3d means 001.JPG notation
	if (!cap.isOpened()) 			// check if succeeded
		return -1;

	Size size(800, 600);
	vector<Mat> database_channels;
	Mat h1, s1, v1;					// channels for database_img
	double dh, ds, dv; 				// distances from color spaces
	double gcs_distance;			// global color space distance
	float distance_array[56] = {0};	// array to store all euclidean distance
	
	int i;
	for (i = 0; i < 56; i++) {
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
		// dh = norm(h1-h0);
		// ds = norm(s1-s0);
		// dv = norm(v1-v0);

		dh = norm(abs(h1 - h0), NORM_L2);
		ds = norm(abs(s1 - s0), NORM_L2);
		dv = norm(abs(v1 - v0), NORM_L2);

		gcs_distance = sqrt(dh + ds + dv);
		printf("Computed distance %d: %f\n", i, dh);

		// insert found distance in distance_array
		distance_array[i] = gcs_distance;
	}

	// sort the arrey in ascending order
	float sorted_array[56] = {0};
	for (i = 0; i < 56; i++) {
		sorted_array[i] = distance_array[i];
	}

	int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
	sort(sorted_array, sorted_array + n);

	// take most similar images index
	int j;
	for (i = 0; i < 5; i++) {
		for (j = 0; j < 56; j++) {
			if (sorted_array[i] == distance_array[j])
				most_similar_imgs_idx_array[i] = j+1;
		}
	}

	
	return 0;
}


int main (int argc, char** argv) {
	const int SIMILAR_IMGS_ARRAY_SIZE = 5;
	String img_database_path =  "image_database/";
	Mat img = imread("image_database/img_015.JPG", IMREAD_COLOR); // skate

	// resize the image
	Size size(800, 600);
	resize (img, img, size);

	// display target image
	printf("Showing the target image:\n\n");

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", img);
	int k = waitKey(0);

	printf("Calling compareColorSpace\n\n");
	int most_similar_imgs_idx_array[SIMILAR_IMGS_ARRAY_SIZE] = {0};
	double ret = compareColorSpace(img, img_database_path, most_similar_imgs_idx_array);

	printf("Showing top 5 most similar images\n\n");
	int i;
	int img_idx;
	String similar_img_path = "";
	for (i = 0; i < SIMILAR_IMGS_ARRAY_SIZE; i++) {
		img_idx = most_similar_imgs_idx_array[i];
		similar_img_path = "";
		if (img_idx <= 9) { 
			printf("img_00%d.JPG\n", img_idx);

			similar_img_path = img_database_path + "img_00" + to_string(img_idx) + ".JPG";
			Mat similar_img = imread(similar_img_path, IMREAD_COLOR);
			resize(similar_img, similar_img, size);

			namedWindow("Display Image", WINDOW_AUTOSIZE);
    		imshow("Display window", similar_img);
			k = waitKey(0);
		}
		else {
			printf("img_0%d.JPG\n", img_idx);

			similar_img_path = img_database_path + "img_0" + to_string(img_idx) + ".JPG";
			Mat similar_img = imread(similar_img_path, IMREAD_COLOR);
			resize(similar_img, similar_img, size);

			namedWindow("Display Image", WINDOW_AUTOSIZE);
    		imshow("Display window", similar_img);
			k = waitKey(0);
		}
	}
	
		
	// wait for a keystroke in the window
    k = waitKey(0);

	return 0;
}