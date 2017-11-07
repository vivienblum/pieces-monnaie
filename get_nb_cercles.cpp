#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



#define SEUIL 50  


using namespace std;
using namespace cv;



int main(int argc, char** argv){



	Mat imageIn = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	
	
	
	if(! imageIn.data ) {
        	cout <<  "Could not open or find the image" << std::endl ;
        	return -1;
   	}
	else {
		Mat imageOutX, imageOutY, imageOut;
		int nbCercles = 0;
		int kernel_size = 3;
		int scale = 1;
		int delta = 0;
		int ddepth = CV_16S;
		
		/*Detection de contour*/
		//Application d'un flou gaussien pour limiter le bruit
		GaussianBlur( imageIn, imageIn, Size(5,5), 0, 0, BORDER_DEFAULT );
		
		//Application d'un seuil de gris pour lisser
		threshold(imageIn, imageIn, SEUIL, 255, THRESH_BINARY);

		//Apply the laplcaian operator
		Laplacian( imageIn, imageOut, CV_16S, kernel_size, scale, delta, BORDER_DEFAULT );

		//Filtre de Sobel sur X
		//Sobel(imageOut, imageOutX, CV_32F, 1 , 0 , 3, 1, 0, BORDER_DEFAULT); 

		//Filtre de Sobel sur Y
		//Sobel(imageOut, imageOutY, CV_32F, 0 , 1 , 3, 1, 0, BORDER_DEFAULT);

		//Addition des 2 matrices pour calculer la magnitude du gradient
		//addWeighted( imageOutX, 0.5, imageOutY, 0.5, 0, imageOut ); 

		


		/*Parcours des pixels de contour*/
		
		convertScaleAbs( imageOut, imageOut );
		imshow( "laplacian", imageOut );
	
		int b = 0, nbPixelContour = 0;
		for( int y = 0; y < imageOut.rows; y++ ) {
			for( int x = 0; x < imageOut.cols; x++ ) {
				//printf("%d\n", imageOut.at<uchar>(y,x));
		       		if (imageOut.at<uchar>(y,x) > 0) {
		       			nbPixelContour++;
					// TODO trouver le cercle qui passe par ce point
					// TODO voter pour ce cercle
				}
		        	else {
		            		b++;
				}
		        }
		}
		printf("Pixel de contour : %d\n", nbPixelContour);
		printf("Autres : %d\n", b);

		/*Parcours des cercles (avec leurs votes)*/
		// TODO faire boucle qui vérifie si le cercle a reçu assez de vote
		printf("Cercles : %d\n", nbCercles);		
		
		//Affichage de l'image
		//imshow( "Gradient Sobel", imageOut );        

		waitKey(0); 
	}
	
//	for( int y = 0; y < imageIn.rows; y++ ) {
//        for( int x = 0; x < imageIn.cols; x++ ) {
//        	if (imageIn.at<uchar>(y,x) < SEUIL)
//         	new_image.at<uchar>(y,x) = 0;
//        	else
//            	new_image.at<uchar>(y,x) = 255;
//        }
//    }
       
	
	       	

	
	return 0;
}

