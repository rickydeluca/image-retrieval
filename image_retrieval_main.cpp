#include "image_retrieval.hpp"

using namespace cv;
using namespace std;

// Global variables
// double shape_dist_array[DATABASE_SIZE]  = {0};
// double color_dist_array[DATABASE_SIZE]  = {0};
// double desc_dist_array[DATABASE_SIZE]   = {0};
double dist_array[DATABASE_SIZE]        = {0};
double sorted_array[DATABASE_SIZE]      = {0};
int idx_array[N_SIM_IMGS]               = {0};

int main (int argc, char** argv) {
    int ret = 0;    // Function retvalue
    int i = 0;      // Iter
    int retrieval_method = 0;

    // Select retrieval method
    printf("Select the retrieval method\n"
            "\t1 - color histogram\n"
            "\t2 - shapes distance\n"
            "\t3 - ORB descriptors\n\n");
    
    scanf("%d", &retrieval_method);

    if (retrieval_method < 1 || retrieval_method > 3 ) {
        cout << "Plese insert a number between 1 and 3";
        return 0;
    }

    // Load query image
    string db_path = "./image_database/img_0";
    Mat query = imread(db_path + argv[1] + ".JPG");
    if (!query.data) {
        printf("ERROR: Cannot read the query image.\n");
        return -1;
    }

    // Show the image 
    printf("\nShowing the query image...\n\n");

    Mat resized_query;
    Size size(800,600);
    resize(query, resized_query, size);

    namedWindow("Display Query", WINDOW_AUTOSIZE);
    imshow("Display Query", resized_query);
	int k = waitKey(0);

    // Compute colors distance
    if (retrieval_method == 1) {
        ret = retrieveColors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve colors.\n");
            return -1;
        }
        
        printf("\n");
    }

    // Compute shape distance
    else if (retrieval_method == 2) { 
        ret = retrieveShapes(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve shapes.\n");
            return -1;
        }

        printf("\n");
    }

    // Compute descriptor distance
    else {
        ret = retrieveOrbDescriptors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve descriptors.\n");
            return -1;
        }

        printf("\n");
    }

    /* Compute the average distance
    printf("Retrieving the most similar images...\n");
    for (i = 0; i < DATABASE_SIZE; i++) {
        // avg_dist_array[i] = shape_dist_array[i] * (color_dist_array[i]);
        avg_dist_array[i] = desc_dist_array[i];
    } */

    // Sort the avg distance array
    memcpy(sorted_array, dist_array, sizeof(double)*DATABASE_SIZE);

    int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
	sort(sorted_array, sorted_array + n);

    // Find indexes of the most similars images to the query
    int j;
	for (i = 0; i < N_SIM_IMGS; i++) {
		for (j = 0; j < DATABASE_SIZE; j++) {
			if (sorted_array[i] == dist_array[j])
				idx_array[i] = j+1;
		}
	}

    // Show most similar shapes
    showSimShapes(idx_array);

    k = waitKey(0);
    destroyAllWindows();

    return 0;
}