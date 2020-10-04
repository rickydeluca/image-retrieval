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
            "\t4 - SIFT descriptors\n"
            "\t5 - SIFT + ORB\n"
            "\t6 - SIFT + Color\n\n");
    
    scanf("%d", &retrieval_method);

    if (retrieval_method < 1 || retrieval_method > 6 ) {
        cout << "Plese insert a number between 1 and 6";
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
    else if (retrieval_method == 4) {
        ret = retrieveSiftDescriptors(query, dist_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve descriptors.\n");
            return -1;
        }

        printf("\n");
    }

    // SIFT + ORB
    else if (retrieval_method == 5) {
        double sift_array[DATABASE_SIZE] = {0};
        double orb_array[DATABASE_SIZE]  = {0};

        ret = retrieveSiftDescriptors(query, sift_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve SIFT descriptors.\n");
            return -1;
        }

        ret = retrieveOrbDescriptors(query, orb_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve ORB descriptors.\n");
            return -1;
        }

        for (int i = 0; i < DATABASE_SIZE; i++) {
            dist_array[i] = 0.2*sift_array[i] + 0.8*orb_array[i];
        }

        printf("\n");
    }

    // SIFT + Color
    else {
        double sift_array[DATABASE_SIZE]    = {0};
        double color_array[DATABASE_SIZE]   = {0};

        ret = retrieveSiftDescriptors(query, sift_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve SIFT descriptors.\n");
            return -1;
        }

        ret = retrieveColors(query, color_array);
        if (ret < 0) {
            printf("ERROR: Cannot retrieve ORB descriptors.\n");
            return -1;
        }

        for (int i = 0; i < DATABASE_SIZE; i++) {
            dist_array[i] = 0.6*sift_array[i] + 0.4*color_array[i];
        }

        printf("\n");
    }
    
    // Sort the distance array in descending order. Higer value, higher match
    memcpy(sorted_array, dist_array, sizeof(double)*DATABASE_SIZE);

    int n = sizeof(sorted_array) / sizeof(sorted_array[0]);
    sort(sorted_array, sorted_array + n, greater<double>());
        
    // Find most similiar image indexes
    vector<int> sim_images_idx;

    for (int i = 0; i < N_SIM_IMGS; i++) {
        for (int j = 0; j < DATABASE_SIZE; j++) {
            if (sorted_array[i] == dist_array[j])
                sim_images_idx.push_back(j);
        }
    }
    
    // Show them
    showSimImages(sim_images_idx, dist_array, N_SIM_IMGS);
    sim_images_idx.clear();

    // Print the normalized distance array values on file
    FILE *fp;

    if ((fp = fopen("data.txt", "wt")) == NULL) {
        printf("Errore nell'apertura del file\n\n");
		exit(1);
    }

    for (int i = 0; i < DATABASE_SIZE; i++) {
        fprintf(fp, "%f, ", dist_array[i]);
    }

    fclose(fp);

    k = waitKey(0);
    destroyAllWindows();

    return 0;
}