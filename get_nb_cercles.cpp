#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



#define SEUIL 50    
#define SEUIL_VOTE 57
#define PI 3.14159265
#define DELTA_DROITE 0.5
#define DELTA_CENTRE 3

using namespace std;
using namespace cv;

double getPente(int x, int y, int angle) {
	double angleRad = angle*PI/180;
	//printf("First: x : %d, y : %d\n", x, y);
	//printf("Secon: x : %f, y : %f\n", 100*(x+ cos(angle)), 100*(y+ sin(angle)));
	//printf("Rad : %f\n", cos(angleRad)/sin(angleRad));
	//printf("Rad : %f\n", cos(angleRad)/sin(angleRad));
	//printf("cos : %f, sin : %f\n", cos(angleRad), sin(angleRad));
	//printf("Angle : %d, y : %d, y+cos : %f\n", angle, y, (y+ sin(angleRad))*1000000);
	//printf("Angle : %d, y : %d, y+cos : %f\n", angle, y, (y+ sin(angleRad))*1000000);
	//return (y + 100000+(y+ sin(angleRad)))/(x + 100000+(x+ cos(angleRad)));
	return sin(angleRad)/cos(angleRad);
}

double getOrdOrigine(int x, int y, double pente) {
	return y - pente*x;
}

int main(int argc, char** argv){
	
	Mat imageIn = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	
	if(! imageIn.data ) {
        	cout <<  "Could not open or find the image" << std::endl ;
        	return -1;
   	}
	else {
		Mat imageOutX, imageOutY, imageOut;//, cercles(Mat::zeros(imageIn.size(), CV_8UC3));
		Mat new_image = Mat::zeros( imageIn.size(), imageIn.type() );
		//int sizes[] = { imageIn.cols, imageIn.rows, (int) sqrt(imageIn.cols*imageIn.cols + imageIn.rows*imageIn.rows) };
		//Mat cercles;
		//printf("%d, %d, %d \n", imageIn.rows, imageIn.cols, (int) sqrt(imageIn.cols*imageIn.cols + imageIn.rows*imageIn.rows));
		//Mat cercles(3, sizes, CV_64F);
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
		//Laplacian( imageIn, imageOut, CV_16S, kernel_size, scale, delta, BORDER_DEFAULT );
		//convertScaleAbs( imageOut, imageOut );		

		
		//Filtre de Sobel sur X
		Sobel(imageIn, imageOutX, CV_32F, 1 , 0 , 3, 1, 0, BORDER_DEFAULT); 

		//Filtre de Sobel sur Y
		Sobel(imageIn, imageOutY, CV_32F, 0 , 1 , 3, 1, 0, BORDER_DEFAULT);

		//Addition des 2 matrices pour calculer la magnitude du gradient
		addWeighted( imageOutX, 0.5, imageOutY, 0.5, 0, imageOut ); 

		//Gradient
		Mat mag, angle; 
		cartToPolar(imageOutX, imageOutY, mag, angle, 1); 

		//Test
		/*Canny(imageIn, imageOut, 20, 100);
		vector<Point2i> points;
		findNonZero(imageOut, points);*/
		//Mat output(Mat::zeros(imageOut.size(), CV_8UC3));
		//Mat::zeros(imageOut.size());
		/*for (int i = 0; i < points.size(); i++) {
				output.at<Vec3b>(i) = Vec3b(127, 255, 127);
		 }
		for (auto const& p : points) {
				output.at<Vec3b>(p) = Vec3b(127, 255, 127);
			}
		imshow( "test", output );*/


		/*Parcours des pixels de contour*/
		int b = 0, nbPixelContour = 0;
		int first = 0;
		int limitFirst = 100000;
		for( int y = 0; y < angle.rows && first < limitFirst; y++ ) {
			for( int x = 0; x < angle.cols && first < limitFirst; x++ ) {
				//printf("%d\n", imageOut.at<uchar>(y,x));
				if (mag.at<uchar>(y,x) > 0) {
					int angleVal = angle.at<uchar>(y,x);
					double pente = getPente(x, y, angleVal);
					double ord = getOrdOrigine(x, y, pente);
					nbPixelContour++;
					//printf("x : %d, y : %d\n", x, y);
					//printf("Angle : %d\n", angle.at<uchar>(y,x));
					//printf("Pente : %f\n", pente);
					//printf("Ord : %f\n", ord);
					//printf("Mag : %d\n", mag.at<uchar>(y,x));
					first++;
				
					for( int j = 0; j < imageOut.rows; j++ ) {
						for( int i = 0; i < imageOut.cols; i++ ) {
							if (j <= pente*i + ord + DELTA_DROITE && j >= pente*i + ord - DELTA_DROITE) {
								//printf("Point\n");
								/*for( int l = j - DELTA_CENTRE; l < j + DELTA_CENTRE && l < new_image.rows; l++ ) {
									for( int k = - DELTA_CENTRE; k < j + DELTA_CENTRE && k < new_image.cols; k++ ) {
										if (new_image.at<uchar>(l,k) < 255) {
											new_image.at<uchar>(l,k)++;
											//new_image.at<uchar>(j,i) = 255;
										}
									}
								}*/
								if (new_image.at<uchar>(j,i) < 255) {
									new_image.at<uchar>(j,i)++;
									//new_image.at<uchar>(j,i) = 255;
								}
								
							}
						}
					}
					
					// TODO trouver le cercle qui passe par ce point
					// TODO voter pour ce cercle
				}
				else {
					b++;
				}
			}
		}
		for( int y = 0; y < new_image.rows && first < limitFirst; y++ ) {
			for( int x = 0; x < new_image.cols && first < limitFirst; x++ ) {
				if (new_image.at<uchar>(y,x) > SEUIL_VOTE) {
					new_image.at<uchar>(y,x) = 255;
					nbCercles++;
				}
				else {
					new_image.at<uchar>(y,x) = 0;
				}
			}
		}
		printf("Pixels de contour : %d\n", nbPixelContour);
		printf("Autres : %d\n", b);

		/*Parcours des cercles (avec leurs votes)*/
		// TODO faire boucle qui vérifie si le cercle a reçu assez de vote
		printf("Cercles : %d\n", nbCercles);		
		
		//Affichage de l'image
		imshow( "laplacian", new_image );       
		//imshow( "laplacian", new_image );       

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

