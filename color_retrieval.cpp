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

const int w_size = 900;
const int h_size = 600;

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

	// sort the array in ascending order
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

// compare the color space segmented in block - a different weight is assigned to each block
int compareColorSpaceBlocks(Mat img, String img_database_path, int (&most_similar_imgs_idx_array)[5]) {
	// convert rgb to hsv
	cvtColor(img, img, COLOR_BGR2HSV);

	// read  images from database
	VideoCapture cap("image_database/img_%3d.JPG"); // %3d means 001.JPG notation
	if (!cap.isOpened())	// check if succeeded
		return -1;

	Mat database_img;
	Size size(w_size, h_size);

	vector<Mat> channels;
	Mat h0, s0, v0;								// channels for input img

	vector<Mat> database_channels;
	Mat h1, s1, v1;								// channels for database_img

	double dh, ds, dv; 							// distances from color spaces
	double block_distance;						// color space distance between block
	
	// array to store the color distance between each block
	int n_tot_blocks = N_BLOCKS_ROW * N_BLOCKS_COL;
	float block_distance_array[n_tot_blocks] = {0};

	// array to store the weighted distance between imgs
	double wcs_distance;						// weighted color space distance
	float distance_array[DATABASE_SIZE] = {0};	// array to store all weighted distance

	// create rect to crop the img
	int w_block = w_size / N_BLOCKS_COL;
	int h_block = h_size / N_BLOCKS_ROW;
	// Rect roi = new Rect(0, 0, w_block, h_block);

	Mat crop_img, crop_database_img;
	int rb, cb; 	// block iterator
	int i;			// database iterator
	int b_idx;		// idx in the array to store the block distance
	for (i = 0; i < DATABASE_SIZE; i++) {
		b_idx = 0;	 
		for (rb = 0; rb < N_BLOCKS_ROW; rb++) {
			for (cb = 0; cb < N_BLOCKS_COL; cb++) {
				cap >> database_img;
				resize(database_img, database_img, size);				// resize the image
				cvtColor(database_img, database_img, COLOR_BGR2HSV);	// convert to hsv colorspace

				// crop imgs
				int w_start = cb * w_block;
				int h_start = rb * h_block;
				int w_end 	= ((cb + 1) * w_block);
				int h_end	= ((rb + 1) * h_block);
				printf("Rect dimensions: %d, %d, %d, %d\n", w_start, w_end, h_start, h_end);
				Rect roi(w_start, h_start, w_block, h_block);

				printf("Crop img...\n");
				crop_img = img(roi);
				crop_database_img = database_img(roi);

				// split imgs channel
				printf("Split channels...\n");
				split(crop_img, channels);
				Mat h0 = channels[0];
				Mat s0 = channels[1];
				Mat v0 = channels[2];

				split(crop_database_img, database_channels);
				h1 = database_channels[0];
				s1 = database_channels[1];
				v1 = database_channels[2];

				printf("Compute distance...\n");
				dh = norm(abs(h1 - h0), NORM_L2);
				ds = norm(abs(s1 - s0), NORM_L2);
				dv = norm(abs(v1 - v0), NORM_L2);

				printf("Store distance into array...\n");
				block_distance = norm(dh + ds + dv, NORM_L2);
				printf("Distance from img %d, block num %d: %f\n\n", i, b_idx, block_distance);

				// insert found distance in distance_array with weight
				// if the block contains more information then decrease the distance
				if (b_idx == 0 || b_idx == 2 || b_idx == 6 || b_idx == 0)
					block_distance_array[b_idx] = block_distance;
				else if (b_idx == 1 || b_idx == 7)
					block_distance_array[b_idx] = 0.75 * block_distance;
				else if (b_idx == 3 || b_idx == 5)
					block_distance_array[b_idx] = 0.50 * block_distance;
				else 
					block_distance_array[b_idx] = 0.25 * block_distance;
				
				++b_idx;
			}
		}
		printf("\n\n");
		// compute the weighet average of the blocks
		wcs_distance = 0;
		b_idx = 0;
		for (b_idx = 0; b_idx < n_tot_blocks; b_idx++) {
			wcs_distance += block_distance_array[b_idx];
		}

		wcs_distance = wcs_distance / n_tot_blocks;

		// assing the distance between target img and img in position "i" at the index "i" of the array
		distance_array[i] = wcs_distance;
	}

	// sort the array in ascending order
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

	printf("\n\n");
	return 0;
}

int main (int argc, char** argv) {
	String img_database_path =  "image_database/";
	Mat img = imread("image_database/img_015.JPG", IMREAD_COLOR); // happy kiki :)

	// resize the image
	Size size(w_size, h_size);
	resize (img, img, size);

	// display target image
	printf("Target image:\n\n");

    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", img);
	int k = waitKey(0);

	//*** TEST CROP IMG ***
	Rect r(0, 0, 900, 600);
	Mat crop_img = img(r);
	namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", crop_img);
	k = waitKey(0);
	return 0;
	
	// *** TEST GLOBAL COLOR SPACE ***
	printf("Compare global color space...\n\n");
	int most_similar_imgs_idx_array[N_SIM_IMGS] = {0};
	int ret = compareColorSpace(img, img_database_path, most_similar_imgs_idx_array);

	printf("Show top 5 most similar images\n\n");
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
	

	// *** TEST BLOCK COLOR SPACE ***
	/*
	printf("Compare global color space...\n\n");
	int most_similar_imgs_idx_array[N_SIM_IMGS] = {0};
	int ret = compareColorSpaceBlocks(img, img_database_path, most_similar_imgs_idx_array);

	printf("Show top 5 most similar images\n\n");
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
	*/

	// wait for a keystroke in the window
    k = waitKey(0);

	return 0;
}