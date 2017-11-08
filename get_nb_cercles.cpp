#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>



#define SEUIL 50    
#define SEUIL_VOTE 57
#define SEUIL_MAG 220
#define PI 3.14159265
#define DELTA_DROITE 0.5
#define DELTA_CENTRE 300

using namespace std;
using namespace cv;

double getPente(int x, int y, int angle) {
	double angleRad = angle*PI/180;
	return sin(angleRad)/cos(angleRad);
	//return (y + y + sin(angleRad))/(x + x + cos(angleRad));
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
		Mat imageOutX, imageOutY, imageOut, votes = Mat::zeros(imageIn.size(), imageIn.type());
		int height = imageIn.rows;
		int width = imageIn.cols;
		int depth = sqrt(height*height + width*width);
		//printf("%d, %d, %d\n", height, width, depth);
		//vector<vector<vector<int> > > cercles (height, vector<vector<int> >(width, vector <int>((int) depth, 0)));
		//vector<vector<vector<int> > > vec (height,vector<vector<int> >(width,vector <int>(1000,12)));
		int nbCercles = 0;
		int nbPixelContour = 0;
		
		/*Detection de contour*/
		//Application d'un flou gaussien pour limiter le bruit
		GaussianBlur( imageIn, imageIn, Size(5,5), 0, 0, BORDER_DEFAULT );
		
		//Application d'un seuil de gris pour lisser
		threshold(imageIn, imageIn, SEUIL, 255, THRESH_BINARY);

		//Apply the laplcaian operator
		//Laplacian( imageIn, imageOut, CV_16S, 3, 1, 0, BORDER_DEFAULT );
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

		/*Parcours des pixels de contour*/
		int first = 0;
		int limitFirst = 10000;
		for( int y = 0; y < angle.rows && first < limitFirst; y++ ) {
			for( int x = 0; x < angle.cols && first < limitFirst; x++ ) {
				if (mag.at<uchar>(y,x) > SEUIL_MAG) {
					int angleVal = angle.at<uchar>(y,x);
					double pente = getPente(x, y, angleVal);
					double ord = getOrdOrigine(x, y, pente);
					printf("Angle : %d, mag : %d\n", angle.at<uchar>(y,x), mag.at<uchar>(y,x));
					/*printf("Pente : %f\n", pente);
					printf("Ord : %f\n", ord);*/
					nbPixelContour++;
					first++;
				
					for( int j = 0; j < imageOut.rows; j++ ) {
						for( int i = 0; i < imageOut.cols; i++ ) {
							
							//Pour chaque point de la droite
							if (j <= pente*i + ord + DELTA_DROITE && j >= pente*i + ord - DELTA_DROITE) {
								if (votes.at<uchar>(j,i) < 255) {
									votes.at<uchar>(j,i) = 255;
								}
								//On vérifie s'il n'existe pas déjà un cercle
								int xCercle = i;
								int yCercle = j;
								int max = votes.at<uchar>(j,i);
								for( int l = j - DELTA_CENTRE; l < j + DELTA_CENTRE && l < votes.rows && l >= 0; l++ ) {
									for( int k = - DELTA_CENTRE; k < j + DELTA_CENTRE && k < votes.cols && k >= 0; k++ ) {
										if (votes.at<uchar>(l,k) > max) {
											xCercle = k;
											yCercle = l;
										}
										//if (votes.at<uchar>(l,k) < 255) {
											//votes.at<uchar>(l,k)++;
										//	votes.at<uchar>(j,i) = 255;
										//}
									}
								}
								if (votes.at<uchar>(yCercle,xCercle) < 255) {
									//votes.at<uchar>(yCercle,xCercle)++;
								}
								
							}
						}
					}
				}
			}
		}
		/*Parcours des cercles (avec leurs votes)*/
		for( int y = 0; y < votes.rows; y++ ) {
			for( int x = 0; x < votes.cols ; x++ ) {
				if (votes.at<uchar>(y,x) > SEUIL_VOTE) {
					//votes.at<uchar>(y,x) = 255;
					nbCercles++;
				}
				else {
					//votes.at<uchar>(y,x) = 0;
				}
			}
		}
		printf("Pixels de contour : %d\n", nbPixelContour);

		// TODO faire boucle qui vérifie si le cercle a reçu assez de vote
		printf("Cercles : %d\n", nbCercles);		
		
		//Affichage de l'image
		//imshow( "out", imageOut );       
		imshow( "laplacian", votes );       

		waitKey(0); 
	} 
	
	return 0;
}

