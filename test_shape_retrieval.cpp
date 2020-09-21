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

// int thresh = 100;
// int max_thresh = 255;

Mat query, database_img;
// Mat canny_query, canny_database_img;
// Mat thresh_query, thresh_database_img;
// vector<vector<Point>> cont_query, cont_database_img;
// vector<Vec4i> hierarchy_query, hierarchy_database_img;

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
    double ret;

    // Load an image
    query = imread("./image_database/img_047.JPG", IMREAD_GRAYSCALE);

    if (!query.data) {
        return -1;
    }
    
    // Resize the image
    Size size(800, 600);
    resize (query, query, size);

    // Show the image 
    namedWindow("Display Image", WINDOW_AUTOSIZE);
    imshow("Display Image", query);
	int k = waitKey(0);

    // Binarize the image using thresholding
    threshold(query, query, 128, 255, THRESH_BINARY);
    
    /** Hu Moments computing - Automized by matchShapes
    // Calculate Moments
    Moments query_moments = moments(query, false);

    // Calculate Hu Moments
    double query_hu_moments[7];
    HuMoments(query_moments, query_hu_moments);

    // Log scale hu moments
    int j;
    for (j = 0; j < 7; j++) {
        query_hu_moments[j] = -1 * copysign(1.0, query_hu_moments[j]);
        log10(abs(query_hu_moments[j]));
    }
    */
    /** Detect edges using canny
    Canny(query, canny_query, thresh, thresh*2, 3);
    printf("Sono dopo Canny 1\n");
    
    // Find contours
    threshold(query, thresh_query, thresh, thresh*2, 0);
    findContours(thresh_query, cont_query, 2, 1);
    // printf("Sono dopo Find Contours 1\n");
    */

    // Read images from database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    // Store the distance between shapes
    double dist = 0.0;

    // Scan the database
    int i;
    for (i = 0; i < DATABASE_SIZE; i++) {
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }
        
        // Resize and convert to grayscale
        resize(database_img, database_img, size);		        // resize the image
		cvtColor(database_img, database_img, COLOR_BGR2GRAY);   // convert to grayscale

        // Show database image
        namedWindow("Display Image", WINDOW_AUTOSIZE);
    	imshow("Display Image", database_img);
		k = waitKey(1);

        // Binarize the image using thresholding
        threshold(database_img, database_img, 128, 255, THRESH_BINARY);

        /**
        // Calculate Moments
        Moments database_img_moments = moments(database_img, false);

        // Calculate Hu Moments
        double database_img_hu_moments[7];
        HuMoments(database_img_moments, database_img_hu_moments);

        // Log scale hu moments
        for (j = 0; j < 7; j++) {
            database_img_hu_moments[j] = -1 * copysign(1.0, database_img_hu_moments[j]);
            log10(abs(query_hu_moments[j]));
        }
        */

        /*
        // Detect edges using canny
        Canny(database_img, canny_database_img, thresh, thresh*2, 3);
        printf("Sono dopo Canny 2\n");
        
        // Find contours
        threshold(database_img, thresh_database_img, thresh, thresh*2, 0);
        findContours(thresh_database_img, cont_database_img, 2, 1);
        printf("Sono dopo Find Contours 2\n");

        // Compute distance between shapes
        // Ptr<ShapeContextDistanceExtractor> sd = createShapeContextDistanceExtractor();
        // Ptr<HausdorffDistanceExtractor> hd = createHausdorffDistanceExtractor();

        // dist = sd->computeDistance(query, database_img); //, CONTOURS_MATCH_I1, 0);
        */

        dist = matchShapes(query, database_img, CONTOURS_MATCH_I2, 0);
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