#include "image_retrieval.hpp"

using namespace cv;
using namespace std;

// Global variables
double dist_array[DATABASE_SIZE]        = {0};      // Store the computed distance between images
double sorted_array[DATABASE_SIZE]      = {0};      // The dist_array sorted
int desc_idx_array[DATABASE_SIZE]       = {0};      // Image indexes of the most similar descriptors
int color_shape_idx_array[N_SIM_IMGS]   = {0};      // Image indexes of the most similar colors histograms or shapes

int main (int argc, char** argv) {
    int ret = 0;    // Function retvalue
    int retrieval_method = 0;

    // Select retrieval method
    printf("Select the retrieval method\n"
            "\t1 - Color histogram\n"
            "\t2 - Shapes distance\n"
            "\t3 - ORB descriptors\n"
            "\t4 - SIFT descriptors\n\n");
    
    scanf("%d", &retrieval_method);

    if (retrieval_method < 1 || retrieval_method > 4 ) {
        cout << "Plese insert a number between 1 and 4";
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
    else {
        ret = retrieveSiftDescriptors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve descriptors.\n");
            return -1;
        }

        printf("\n");
    }

    
    // Sort the distance array
    memcpy(sorted_array, dist_array, sizeof(double)*DATABASE_SIZE);

    int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
    sort(sorted_array, sorted_array + n);
        
    // Find most similiar image indexes
    vector<int> sim_images_idx;

    for (int i = 0; i < N_SIM_IMGS; i++) {
        for (int j = 0; j < DATABASE_SIZE; j++) {
            if (sorted_array[i] == dist_array[j])
                sim_images_idx.push_back(j+1);
        }
    }
    
    // Show them
    showSimImages(sim_images_idx, N_SIM_IMGS);


    /*
    // Show best good matches
    else {
        vector<int> sim_images_idx;

        for (int j = 0; j < DATABASE_SIZE; j++) {
            if (dist_array[j] < 0.04) {
                sim_images_idx.push_back(j+1);
            }
        }

        const int num_sim_images = sim_images_idx.size();

        showSimImages(sim_images_idx, num_sim_images);

        sim_images_idx.clear();
    }
    */

    sim_images_idx.clear();
    k = waitKey(0);
    destroyAllWindows();

    return 0;
}