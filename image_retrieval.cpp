#include "image_retrieval.hpp"

using namespace std;
using namespace cv;

/** @funtion showSimShapes */
void showSimShapes(int (&idx_array)[N_SIM_IMGS]) {
    printf("\nShow top %d most similar images\n\n", N_SIM_IMGS);

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
    		imshow("Display Image", similar_img);
			k = waitKey(0);
		}

		else {
			printf("img_0%d.JPG\n", img_idx);

			similar_img_path = "./image_database/img_0" + to_string(img_idx) + ".JPG";
			Mat similar_img = imread(similar_img_path, IMREAD_COLOR);
			resize(similar_img, similar_img, size);

			namedWindow("Display Image", WINDOW_AUTOSIZE);
    		imshow("Display Image", similar_img);
			k = waitKey(0);
		}
    }
}

double ransacClean(vector<cv::DMatch>& matches, vector<cv::KeyPoint>& query_kp, vector<cv::KeyPoint>& db_img_kp) {
     vector<cv::DMatch> out_matches;
    
    // Convert keypoints into Point2f	
    std::vector<cv::Point2f> points1, points2;	
    for (std::vector<cv::DMatch>::const_iterator it= matches.begin();
            it!= matches.end(); ++it) {

            // Get the position of left keypoints
            float x= query_kp[it->queryIdx].pt.x;
            float y= query_kp[it->queryIdx].pt.y;
            points1.push_back(cv::Point2f(x,y));
            
            // Get the position of right keypoints
            x= db_img_kp[it->trainIdx].pt.x;
            y= db_img_kp[it->trainIdx].pt.y;
            points2.push_back(cv::Point2f(x,y));
    }

    // Compute F matrix using RANSAC
    std::vector<uchar> inliers(points1.size(),0);
    cv::Mat fundemental= cv::findFundamentalMat(cv::Mat(points1),cv::Mat(points2), inliers);

    // extract the surviving (inliers) matches
    std::vector<uchar>::const_iterator itIn= inliers.begin();
    std::vector<cv::DMatch>::const_iterator itM= matches.begin();

    // for all matches
    for ( ; itIn!= inliers.end(); ++itIn, ++itM) {

        if (*itIn) { // it is a valid match

            out_matches.push_back(*itM);
        }
    }

    return (double) out_matches.size();
}

/** @function compare_response */
bool compare_response(DMatch first, DMatch second) {
    if (first.distance < second.distance)
        return true;
    
    else 
        return false;
}

int retrieveSiftDescriptors(Mat query, double (&desc_dist_array)[DATABASE_SIZE]) {
    Mat database_img;                       // Store the database image
    Mat query_desc, db_img_desc;            // Store descriptors
    vector<KeyPoint> query_kp, db_img_kp;   // Store keypoint
    vector<vector<DMatch>> matches;         // Store the matches between descriptors
    vector<DMatch> good_matches;            // Matches that pass the ratio test
    
    const float ratio       = 0.75;         // Value for the ratio test
    double desc_dist        = 0;  
    double similarity_score = 0;
    int num_keypoints       = 0;

    // Resize query and convert it to grayscale
    Size size(800,600);
    resize(query, query, size);
    cvtColor(query, query, COLOR_BGR2GRAY);
    
    // Init SIFT detector
    Ptr<xfeatures2d::SIFT> sift = xfeatures2d::SIFT::create();

    // Detect and compute keypoints and descriptors of the query
    query_kp.clear();
    sift->detectAndCompute(query, noArray(), query_kp, query_desc);

    // Create BFMatcher object with default params: NORM_L2, crossCheck = false
    Ptr<BFMatcher> bf = BFMatcher::create();

    // Scan the database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    for (int i = 0; i < DATABASE_SIZE; i++) {
        // Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }

        // Resize database image and convert it to grayscale
        resize(database_img, database_img, size);
        cvtColor(database_img, database_img, COLOR_BGR2GRAY);
        
        // Detect and compute keypoints and descriptors of the database image
        db_img_kp.clear();
        sift->detectAndCompute(database_img, noArray(), db_img_kp, db_img_desc);

        // Match descriptors
        matches.clear();
        bf->knnMatch(query_desc, db_img_desc, matches, 2);

        // Apply ratio test
        good_matches.clear();
        for (int j = 0; j < matches.size(); j++) {
            if (matches[j][0].distance < ratio * matches[j][1].distance) {
                good_matches.push_back(matches[j][0]);
            }
        }
        
        // Refine using RANSAC
        double num_good_matches_refined = ransacClean(good_matches, query_kp, db_img_kp);

        printf("Num of SIFT matches with image %3d: %f\n", i+1, num_good_matches_refined);
        desc_dist_array[i] = 1 / num_good_matches_refined;
    }

    return 0;
}

/** @function retrieveDescriptors */
int retrieveOrbDescriptors(Mat query, double (&desc_dist_array)[DATABASE_SIZE]) {
    Mat database_img;                       // Store the database image
    Mat query_desc, db_img_desc;            // Store descriptors
    Mat homography;                         // Store homography
    
    vector<KeyPoint> query_kp, db_img_kp;   // Store keypoint
    vector<DMatch> matches;                 // Store the matches between descriptors
    vector<DMatch> good_matches;
    vector<DMatch>::iterator m_it;          // Iterator for a vector of DMatch

    const int MAX_FEATURES          = 500;
    const float GOOD_MATCH_PERCENT  = 0.75f;

    double desc_dist                = 0;

    // Resize the query and convert it to grayscale
    Size size(800,600);
    resize(query, query, size);
    cvtColor(query, query, COLOR_BGR2GRAY);

    // Create detector for ORB descriptors
    Ptr<ORB> orb = ORB::create(MAX_FEATURES);

    // Detect and compute keypoints and descriptors of the query
    query_kp.clear();
    orb->detectAndCompute(query, Mat(), query_kp, query_desc);

    // Create BFMatcher object
    Ptr<BFMatcher> bf = BFMatcher::create(NORM_HAMMING, true);
    // Ptr<DescriptorMatcher> bf = DescriptorMatcher::create("BruteForce-Hamming");

    // Scan the database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    for (int i = 0; i < DATABASE_SIZE; i++) {
        // Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }

        // Resize database image and convert it to grayscale
        resize(database_img, database_img, size);
        cvtColor(database_img, database_img, COLOR_BGR2GRAY);

        // Detect and compute keypoints and descriptors of the database image
        db_img_kp.clear();
        orb->detectAndCompute(database_img, Mat(), db_img_kp, db_img_desc);
        
        // Match features
        matches.clear();
        bf->match(query_desc, db_img_desc, matches, Mat());

        // Sort matches by score
        std::sort(matches.begin(), matches.end());

        // Remove not so good matches
        const int num_good_matches = matches.size() * GOOD_MATCH_PERCENT;
        matches.erase(matches.begin() + num_good_matches, matches.end());

        // Refine using RANSAC
        double num_good_matches_refined = ransacClean(matches, query_kp, db_img_kp);

        printf("Num of ORB good matches with image %3d: %f\n", i+1, num_good_matches_refined);
        desc_dist_array[i] = 1 / (double) num_good_matches_refined;
    }

    return 0;
}


/** @function retrieveShapes */
int retrieveShapes(Mat query, double (&shape_dist_array)[DATABASE_SIZE]) {
    int i = 0;          // Iter
    double dist = 0.0;  // Store the distance between shapes
    Mat database_img;   // Store the images read from the database

    // Convert to grayscale
    cvtColor(query, query, COLOR_BGR2GRAY);

    // Binarize the image using thresholding
    threshold(query, query, 128, 255, THRESH_BINARY);

    // Read images from database
    VideoCapture cap("./image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							  // check if succeeded
		return -1;

    // Scan the database
    for (i = 0; i < DATABASE_SIZE; i++) {
        // Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }
        
        // Convert to grayscale
		cvtColor(database_img, database_img, COLOR_BGR2GRAY);

        // Binarize the image using thresholding
        threshold(database_img, database_img, 128, 255, THRESH_BINARY);

        // Comparing shapes
        dist = matchShapes(query, database_img, CONTOURS_MATCH_I2, 0);
        printf("Shape distance from image %3d: %f\n", i+1, dist);

        // Insert distances in the array
        shape_dist_array[i] = dist;
    }

    return 0;
}


/** @function retrieveColors */
int retrieveColors(Mat query, double (&color_dist_array)[DATABASE_SIZE]) {
    int i = 0;          // Iter
    Mat database_img;   // Store the images read from the database

    // Convert to HSV colorspace
    cvtColor(query, query, COLOR_BGR2HSV);

    // Split query channels
	vector<Mat> channels;
	split(query, channels);
	Mat h0 = channels[0];
	Mat s0 = channels[1];
	Mat v0 = channels[2];

    // Read images from database
	VideoCapture cap("image_database/img_%3d.JPG"); // %3d means 00x.JPG notation
	if (!cap.isOpened()) 							// check if succeeded
		return -1;

    // Declare vars to store the database image channels
    vector<Mat> database_channels;
	Mat h1, s1, v1;			

    // Store color spaces distances
    double dh, ds, dv;  // Single color space distance
    double dist = 0.0;  // global color spaces distance

    for (i = 0; i < DATABASE_SIZE; i++) {
		// Read next image from database
        cap >> database_img;
        if (!database_img.data) {
            return -1;
        }
        
        // Convert to HSV color space
		cvtColor(database_img, database_img, COLOR_BGR2HSV);

		// Split database_img channels
		split(database_img, database_channels);
		h1 = database_channels[0];
		s1 = database_channels[1];
		v1 = database_channels[2];

        // Compute distance for each channel
		dh = norm(abs(h1 - h0), NORM_L2);
		ds = norm(abs(s1 - s0), NORM_L2);
		dv = norm(abs(v1 - v0), NORM_L2);

        // Get global color space distance
		dist = norm(dh + ds + dv, NORM_L2);
		printf("Color distance from %3d: %f\n", i+1, dist);

		// insert found distance in distance_array
		color_dist_array[i] = dist;
	}

    return 0;
}