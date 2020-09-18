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
            "\t1 - Color histogram\n"
            "\t2 - Shapes distance\n"
            "\t3 - ORB descriptors\n"
            "\t4 - SIFT descriptors\n"
            "\t5 - SIFT + ORB descriptors\n\n");
    
    scanf("%d", &retrieval_method);

    if (retrieval_method < 1 || retrieval_method > 5 ) {
        cout << "Plese insert a number between 1 and 5";
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

    // Compute ORB descriptor distance
    else if (retrieval_method == 3) {
        ret = retrieveOrbDescriptors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve descriptors.\n");
            return -1;
        }

        printf("\n");
    }

    // Compute SIFT descriptor distance
    else if (retrieval_method == 4){
        ret = retrieveSiftDescriptors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve descriptors.\n");
            return -1;
        }

        printf("\n");
    }

    else {
        // Declare two different distance array for ORB and SIFT descriptors
        double orb_dist_array[DATABASE_SIZE]    = {0};
        double sift_dist_array[DATABASE_SIZE]   = {0};

        ret = retrieveOrbDescriptors(query, orb_dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve ORB descriptors.\n");
            return -1;
        }

        printf("\n");

        ret = retrieveSiftDescriptors(query, sift_dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve SIFT descriptors.\n");
            return -1;
        }

        printf("\n");

        // Compute euclidian distance between arrays
        for (int j = 0; j < DATABASE_SIZE; j++) {
            dist_array[j] =  orb_dist_array[j] * sift_dist_array[j];
        }

    }

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