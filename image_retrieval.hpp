#define DATABASE_SIZE   62
#define N_SIM_IMGS      3

void showSimShapes();

int retrieveShapes(Mat query, double (&shape_dist_array)[DATABASE_SIZE]);

int retrieveColors(Mat query, double (&color_dist_array)[DATABASE_SIZE]);