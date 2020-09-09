#include "image_retrieval.hpp"

using namespace cv;
using namespace std;

// Global variables
double shape_dist_array[DATABASE_SIZE]  = {0};
double color_dist_array[DATABASE_SIZE]  = {0};
double desc_dist_array[DATABASE_SIZE]   = {0};
double avg_dist_array[DATABASE_SIZE]    = {0};
double sorted_array[DATABASE_SIZE]      = {0};
int idx_array[N_SIM_IMGS]               = {0};

int main (int argc, char** argv) {
    int ret = 0;    // Function retvalue
    int i = 0;      // Iter

    // Load query image
    Mat query = imread("./image_database/img_004.JPG");
    if (!query.data) {
        printf("ERROR: Cannot read the query image.\n");
        return -1;
    }

    // Show the image 
    printf("Showing the query image...\n\n");

    Mat resized_query;
    Size size(800,600);
    resize(query, resized_query, size);

    namedWindow("Display Query", WINDOW_AUTOSIZE);
    imshow("Display Query", resized_query);
	int k = waitKey(0);

    /* Compute colors distance
    ret = retrieveColors(query, color_dist_array);
    if (ret < 0) {
        printf("ERROR: Cannot retrieve colors.\n");
        return -1;
    }
    
    printf("\n");
    */
   
    /* Compute shape distance
    ret = retrieveShapes(query, shape_dist_array);
    if (ret < 0) {
        printf("ERROR: Cannot retrieve shapes.\n");
        return -1;
    }

    printf("\n");
    */

    // Compute descriptor distance
    ret = retrieveDescriptors(query, desc_dist_array);
    if (ret < 0) {
        printf("ERROR: Cannot retrieve descriptors.\n");
        return -1;
    }

    printf("\n");

    // Compute the average distance
    printf("Retrieving the most similar images...\n");
    for (i = 0; i < DATABASE_SIZE; i++) {
        // avg_dist_array[i] = shape_dist_array[i] * (color_dist_array[i]);
        avg_dist_array[i] = desc_dist_array[i];
    }

    // Sort the avg distance array
    memcpy(sorted_array, avg_dist_array, sizeof(double)*DATABASE_SIZE);

    int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
	sort(sorted_array, sorted_array + n);

    // Find indexes of the most similars images to the query
    int j;
	for (i = 0; i < N_SIM_IMGS; i++) {
		for (j = 0; j < DATABASE_SIZE; j++) {
			if (sorted_array[i] == avg_dist_array[j])
				idx_array[i] = j+1;
		}
	}

    // Show most similar shapes
    showSimShapes(idx_array);

    k = waitKey(0);
    destroyAllWindows();

    return 0;
}