#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



#define SEUIL 50  


using namespace std;
using namespace cv;



int main(int argc, char** argv){



	Mat imageIn = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	Mat new_image = Mat::zeros( imageIn.size(), imageIn.type() );
	
	
	if(! imageIn.data )                              // Check for invalid input
    	{
        	cout <<  "Could not open or find the image" << std::endl ;
        	return -1;
   	}
	
	for( int y = 0; y < imageIn.rows; y++ ) {
        for( int x = 0; x < imageIn.cols; x++ ) {
        	if (imageIn.at<uchar>(y,x) < SEUIL)
            	new_image.at<uchar>(y,x) = 0;
        	else
            	new_image.at<uchar>(y,x) = 255;
        }
    }
       
	
	imshow( "Display window", new_image );             
	waitKey(0);       	

	
	return 0;
}

