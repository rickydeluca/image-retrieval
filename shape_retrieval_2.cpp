#include "opencv2/shape.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

const int DATABASE_SIZE = 62;
const int N_SIM_IMGS = 3;

Mat query, database_img;

float dist_array[DATABASE_SIZE] = {0};
float sorted_array[DATABASE_SIZE] = {0};
int idx_array[N_SIM_IMGS] = {0};

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

/** @function main */
int main( int argc, char** argv ) {
    // Load an image
    query = imread( "./image_database/img_004.JPG" );

    if (!query.data) {
        return -1;
    }
    
    // Resize the image
    Size size(800, 600);
    resize (query, query, size, 0, 0, INTER_LINEAR_EXACT);

    // Convert the image to grayscale
    cvtColor(query, query, COLOR_BGR2GRAY);

    // Show database image 
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display window", query);
	int k = waitKey(0);

    // Read images from database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    // Store the distance between shapes
    float dist = 0.0;

    // Scan the database
    int i;
    for (i = 0; i < DATABASE_SIZE; i++) {
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }

        resize(database_img, database_img, size, 0, 0, INTER_LINEAR_EXACT);		// resize the image
		cvtColor(database_img, database_img, COLOR_BGR2GRAY);                   // convert to grayscale

        // Show database image
        namedWindow("Display Image", WINDOW_AUTOSIZE);
    	imshow("Display window", database_img);
		k = waitKey(1);

        // Compute distance between shapes
        dist = matchShapes(query, database_img, CONTOURS_MATCH_I1, 0);
        printf("Shape distance from image %3d: %f\n", i+1, dist);

        // Insert distances in the array
        dist_array[i] = dist;
    }

    // Deep copy of the distance array
    memcpy(sorted_array, dist_array, sizeof(float)*DATABASE_SIZE);

    // Sort the new distance array
    int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
	sort(sorted_array, sorted_array + n);

    // Find index of the most similars shapes to query shape
	int j;
	for (i = 0; i < N_SIM_IMGS; i++) {
		for (j = 0; j < DATABASE_SIZE; j++) {
			if (sorted_array[i] == dist_array[j])
				idx_array[i] = j+1;
		}
	}

    // Show most similar shapes
    showSimShapes();

    return 0;
}