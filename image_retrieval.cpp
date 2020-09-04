// Combine color and shape retrieval

#include "opencv2/shape.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

// Constants
const int DATABASE_SIZE = 62;
const int N_SIM_IMGS = 3;

/** @funtion showSimShapes */
void showSimShapes() {
    printf("Show top %d most similar images\n\n", N_SIM_IMGS);

	int i, k;
	int img_idx;
	String similar_img_path = "";
    Size size(800,600);

	for (i = 0; i < N_SIM_IMGS; i++) {
		img_idx = idx_array[i];
		similar_img_path = "";
		if (img_idx <= 9) { 
			printf("img_00%d.JPG\n", img_idx);

			similar_img_path = "./image_database/img_00" + to_string(img_idx) + ".JPG";
			Mat similar_img = imread(similar_img_path, IMREAD_COLOR);
			resize(similar_img, similar_img, size);

			namedWindow("Display Image", WINDOW_AUTOSIZE);
    		imshow("Display window", similar_img);
			k = waitKey(0);
		}

		else {
			printf("img_0%d.JPG\n", img_idx);

			similar_img_path = "./image_database/img_0" + to_string(img_idx) + ".JPG";
			Mat similar_img = imread(similar_img_path, IMREAD_COLOR);
			resize(similar_img, similar_img, size);

			namedWindow("Display Image", WINDOW_AUTOSIZE);
    		imshow("Display window", similar_img);
			k = waitKey(0);
		}
    }
}

/** @function retrieveShapes */
int retrieveShapes(Mat query, double (&shape_dist_array)[DATABASE_SIZE]) {
    int i = 0;          // Iter
    double dist = 0.0;  // Store the distance between shapes
    Mat database_img;   // Store the images read from the database

    // Resize the query image
    Size size(800,600);
    resize (query, query, size);

    // Convert to grayscale
    cvtColor(query, query, COLOR_BGR2GRAY);

    // Binarize the image using thresholding
    threshold(query, query, 128, 255, THRESH_BINARY);

    // Read images from database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    // Scan the database
    for (i = 0; i < DATABASE_SIZE; i++) {
        // Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }
        
        // Resize and convert to grayscale
        resize(database_img, database_img, size);		        // resize the image
		cvtColor(database_img, database_img, COLOR_BGR2GRAY);   // convert to grayscale

        // Binarize the image using thresholding
        threshold(database_img, database_img, 128, 255, THRESH_BINARY);

        // Comparing shapes
        dist = matchShapes(query, database_img, CONTOURS_MATCH_I2, 0);
            // printf("Shape distance from image %3d: %f\n", i+1, dist);

        // Insert distances in the array
        shape_dist_array[i] = dist;
    }

    return 0;
}

/** @function retrieveColors */
int retrieveColors(Mat query, double (&color_dist_array)[DATABASE_SIZE]) {
    int i = 0;          // Iter
    Mat database_img;   // Store the images read from the database

    // Resize the query image
    Size size(800,600);
    resize (query, query, size);

    // Convert to HSV colorspace
    cvtColor(query, query, COLOR_BGR2HSV);

    // Split query channels
	vector<Mat> channels;
	split(hsv_img, channels);
	Mat h0 = channels[0];
	Mat s0 = channels[1];
	Mat v0 = channels[2];

    // Read images from database
	VideoCapture cap("image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							// check if succeeded
		return -1;

    // Declare vars to store the database image channels
    vector<Mat> database_channels;
	Mat h1, s1, v1;			

    // Store color spaces distances
    double dh, ds, dv;  // Single color space distance
    double dist = 0.0;  // global color spaces distance

    for (i = 0; i < DATABASE_SIZE; i++) {
		// Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }
        
        // Resize and convert to HSV color space
        resize(database_img, database_img, size);		        // resize the image
		cvtColor(database_img, database_img, COLOR_BGR2HSV);   // convert to grayscale

		// Split database_img channels
		split(database_img, database_channels);
		h1 = database_channels[0];
		s1 = database_channels[1];
		v1 = database_channels[2];

        // Compute distance for each channel
		dh = norm(abs(h1 - h0), NORM_L2);
		ds = norm(abs(s1 - s0), NORM_L2);
		dv = norm(abs(v1 - v0), NORM_L2);

        // Get global color space distance
		dist = norm(dh + ds + dv, NORM_L2);
		printf("Computed distance %d: %f\n", i, dh);

		// insert found distance in distance_array
		color_dist_array[i] = dist;
	}

    return 0;
}