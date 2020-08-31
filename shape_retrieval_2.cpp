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

Mat query, database_img;
vector<Point> contQuery, contDatabaseImg;

/**
 * @function simpleContour
 */
static vector<Point> simpleContour( const Mat& currentQuery, int n=300 )
{
    vector<vector<Point> > _contoursQuery;
    vector <Point> contoursQuery;
    findContours(currentQuery, _contoursQuery, RETR_LIST, CHAIN_APPROX_NONE);
    for (size_t border=0; border<_contoursQuery.size(); border++)
    {
        for (size_t p=0; p<_contoursQuery[border].size(); p++)
        {
            contoursQuery.push_back( _contoursQuery[border][p] );
        }
    }
    // In case actual number of points is less than n
    int dummy=0;
    for (int add=(int)contoursQuery.size()-1; add<n; add++)
    {
        contoursQuery.push_back(contoursQuery[dummy++]); // Adding dummy values
    }
    // Uniformly sampling
    cv::randShuffle(contoursQuery);
    vector<Point> cont;
    for (int i=0; i<n; i++)
    {
        cont.push_back(contoursQuery[i]);
    }
    return cont;
}

/** @function main */
int main( int argc, char** argv ) {
    // Load an image
    query = imread( "./image_database/img_014.JPG" );

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

    // Declare object for use the function computeDistance
    cv::Ptr <cv::ShapeContextDistanceExtractor> mysc = cv::createShapeContextDistanceExtractor();
    
    contQuery = simpleContour(query);
    
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
		k = waitKey(2);

        // For each image in the database compute the shape distance with the query image
        contDatabaseImg = simpleContour(database_img);
        
        dist = mysc->computeDistance(contDatabaseImg, contQuery);
        printf("Shape distance from image %3d: %f\n", i+1, dist);
    }

    return 0;
}