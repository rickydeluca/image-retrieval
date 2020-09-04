#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "image_retrieval.hpp"

// Global variables
double shape_dist_array[DATABASE_SIZE]  = {0};
double color_dist_array[DATABASE_SIZE]  = {0};
double sorted_array[DATABASE_SIZE]      = {0};
int idx_array[N_SIM_IMGS]               = {0};

int main (int argc, char** argv) {

}