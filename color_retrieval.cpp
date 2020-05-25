#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/types.hpp>	// rect to crop img

#include <iostream>
#include <math.h>			// matrices operations
#include <bits/stdc++.h>	// sorting arrays

using namespace cv;
using namespace std;

const int DATABASE_SIZE = 56;
const int N_SIM_IMGS	= 5;
const int N_BLOCKS_ROW	= 3;
const int N_BLOCKS_COL  = 3;

// compare global color space between an input image and the images into a database
int compareColorSpace(Mat img, String img_database_path, int (&most_similar_imgs_idx_array)[5]) {
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
	VideoCapture cap("image_database/img_%3d.JPG"); // %3d means 001.JPG notation
	if (!cap.isOpened()) 							// check if succeeded
		return -1;

	Mat database_img;
	Size size(900, 600);
	
	vector<Mat> database_channels;
	Mat h1, s1, v1;								// channels for database_img
	double dh, ds, dv; 							// distances from color spaces
	
	double gcs_distance;						// global color space distance
	float distance_array[DATABASE_SIZE] = {0};	// array to store all euclidean distance

	int i;
	for (i = 0; i < DATABASE_SIZE; i++) {
		cap >> database_img;
		resize(database_img, database_img, size);				// resize the image
		cvtColor(database_img, database_img, COLOR_BGR2HSV);	// convert to hsv colorspace

		// split channels of the database_img
		split(database_img, database_channels);
		h1 = database_channels[0];
		s1 = database_channels[1];
		v1 = database_channels[2];

		dh = norm(abs(h1 - h0), NORM_L2);
		ds = norm(abs(s1 - s0), NORM_L2);
		dv = norm(abs(v1 - v0), NORM_L2);

		gcs_distance = norm(dh + ds + dv, NORM_L2);
		printf("Computed distance %d: %f\n", i, dh);

		// insert found distance in distance_array
		distance_array[i] = gcs_distance;
	}

	// sort the arrey in ascending order
	float sorted_array[DATABASE_SIZE] = {0};
	for (i = 0; i < DATABASE_SIZE; i++) {
		sorted_array[i] = distance_array[i];
	}

	int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
	sort(sorted_array, sorted_array + n);

	// take most similar images index
	int j;
	for (i = 0; i < N_SIM_IMGS; i++) {
		for (j = 0; j < DATABASE_SIZE; j++) {
			if (sorted_array[i] == distance_array[j])
				most_similar_imgs_idx_array[i] = j+1;
		}
	}

	return 0;
}
/*
// compare the color space segmented in block - a different weight is assigned to each block
int compareColorSpaceBlocks(Mat img, String img_database_path, int (&most_similar_imgs_idx_array)[5]) {
	// convert rgb to hsv
	cvtColor(img, img, COLOR_BGR2HSV);

	// read  images from database
	VideoCapture cap("image_database/img_%3d.JPG"); // %3d means 001.JPG notation
	if (!cap.isOpened())	// check if succeeded
		return -1;

	Mat database_img;
	Size size(900, 600);

	vector<Mat> channels;
	Mat h0, s0, v0;								// channels for input img

	vector<Mat> database_channels;
	Mat h1, s1, v1;								// channels for database_img

	double dh, ds, dv; 							// distances from color spaces
	double block_distance;						// color space distance between block
	
	// array to store the color distance between each block
	int n_tot_blocks = N_BLOCKS_ROW * N_BLOCKS_COL;
	float block_distance_array[n_tot_blocks] = {0};
	
	// create rect to crop the img
	int w_block = 900 / N_BLOCKS_COL;
	int h_block = 600 / N_BLOCKS_ROW;
	Rect roi; // = new Rect(0, 0, w_block, h_block);

	Mat crop_img, crop_database_img;
	int block = 0;
	int r_point, c_point;
	int last_r_point = 600 - h_block;
	int last_c_point = 900 - w_block;
	int i;
	for (i = 0; i < DATABASE_SIZE; i++) {
		for (r_point = 0; r_point < last_r_point; r_point += h_block) {
			for (c_point = 0; c_point < last_c_point; c_point += w_block) {
				cap >> database_img;
				resize(database_img, database_img, size);				// resize the image
				cvtColor(database_img, database_img, COLOR_BGR2HSV);	// convert to hsv colorspace

				// crop imgs
				roi = new Rect(r_point, c_point, w_block, h_block);
				crop_img = img(roi);
				crop_database_img = database_img(roi);

				// split imgs channel
				split(crop_img, channels);
				Mat h0 = channels[0];
				Mat s0 = channels[1];
				Mat v0 = channels[2];

				split(crop_database_img, database_channels);
				h1 = database_channels[0];
				s1 = database_channels[1];
				v1 = database_channels[2];

				dh = norm(abs(h1 - h0), NORM_L2);
				ds = norm(abs(s1 - s0), NORM_L2);
				dv = norm(abs(v1 - v0), NORM_L2);

				block_distance = norm(dh + ds + dv, NORM_L2);
				printf("Computed distance %d of block %d: %f\n", i, block, dh);

				// insert found distance in distance_array
				distance_array[i] = gcs_distance;
			}
		}
	}
}
*/
int main (int argc, char** argv) {
	String img_database_path =  "image_database/";
	Mat img = imread("image_database/img_015.JPG", IMREAD_COLOR); // happy kiki :)

	// resize the image
	Size size(900, 600);
	resize (img, img, size);

	// display target image
	printf("Target image:\n\n");

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", img);
	int k = waitKey(0);

	/*** TEST CROP IMG ***
	Rect r(0, 0, 100, 100);
	Mat crop_img = img(r);
	namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", crop_img);
	k = waitKey(0);
	return 0;
	***/
	
	printf("Compare color space...\n\n");
	int most_similar_imgs_idx_array[N_SIM_IMGS] = {0};
	int ret = compareColorSpace(img, img_database_path, most_similar_imgs_idx_array);

	printf("Showing top 5 most similar images\n\n");
	int i;
	int img_idx;
	String similar_img_path = "";
	for (i = 0; i < N_SIM_IMGS; i++) {
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