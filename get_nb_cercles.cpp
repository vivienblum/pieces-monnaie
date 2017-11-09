#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

#define SEUIL 50    
#define SEUIL_VOTE 14
#define SEUIL_MAG 220
#define SEUIL_RAYON 10
#define PI 3.14159265
#define DELTA_DROITE 0.5
#define DELTA_CERCLE 50
#define DELTA_CENTRE 300
#define DELTA_RAYON 2
#define MAX_RAYON 800

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

int getRayon(int x1, int y1, int x2, int y2) {
	return (int) sqrt((x2 - x1)*(x2 - x1) + (y2 -y1)*(y2 -y1));
}

bool belongsToDoite(double pente, double ord, int i, int j) {
	return (j <= pente*i + ord + DELTA_DROITE && j >= pente*i + ord - DELTA_DROITE);
}

bool belongsToCircle(int x, int y, int r, int i, int j) {
	double equationCercle = (x - i)*(x - i) + (y - j)*(y - j);
	return (r*r > equationCercle - DELTA_CERCLE && r*r < equationCercle + DELTA_CERCLE);
}

int main(int argc, char** argv){
	
	Mat imageIn = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
	
	if(! imageIn.data ) {
        	cout <<  "Could not open or find the image" << std::endl ;
        	return -1;
   	}
	else {
		Mat imageOutX, imageOutY, imageOut, votes = Mat::zeros(imageIn.size(), imageIn.type()), cercles = Mat::zeros(imageIn.size(), imageIn.type());
		int height = imageIn.rows;
		int width = imageIn.cols;
		int depth = 800;
		//printf("%d, %d, %d\n", height, width, depth);
		//vector<vector<vector<int> > > cercles (height, vector<vector<int> >(width, vector <int>((int) depth, 0)));
		vector<vector<vector<int> > > votesCercles (height,vector<vector<int> >(width,vector <int>(1000,12)));
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
		int limitFirst = 1;
		for( int y = 0; y < angle.rows && first < limitFirst; y++ ) {
			for( int x = 0; x < angle.cols && first < limitFirst; x++ ) {
				if (mag.at<uchar>(y,x) > SEUIL_MAG) {
					int angleVal = angle.at<uchar>(y,x);
					double pente = getPente(x, y, angleVal);
					double ord = getOrdOrigine(x, y, pente);
					//printf("Angle : %d, mag : %d\n", angle.at<uchar>(y,x), mag.at<uchar>(y,x));
					/*printf("Pente : %f\n", pente);
					printf("Ord : %f\n", ord);*/
					nbPixelContour++;
					first++;
				
					for( int j = 0; j < imageOut.rows; j++ ) {
						for( int i = 0; i < imageOut.cols; i++ ) {
							
							//Pour chaque point de la droite
							//if (j <= pente*i + ord + DELTA_DROITE && j >= pente*i + ord - DELTA_DROITE) {
							if (belongsToDoite(pente, ord, i, j)) {
								if (votes.at<uchar>(j,i) < 255) {
									int rayon = getRayon(x, y, i, j);
									if (rayon > SEUIL_RAYON && rayon < MAX_RAYON) {
										//printf("Rayon : %d\n", rayon);
										votesCercles[j][i][rayon] ++;
										cercles.at<uchar>(j,i) = 255;
									}
									
								}
								//On vérifie s'il n'existe pas déjà un cercle
								/*int xCercle = i;
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
								}*/
								
							}
						}
					}
				}
			}
		}
		/*Parcours des cercles (avec leurs votes)*/
		/*for( int y = 0; y < votes.rows; y++ ) {
			for( int x = 0; x < votes.cols ; x++ ) {
				if (votes.at<uchar>(y,x) >= SEUIL_VOTE) {
					//votes.at<uchar>(y,x) = 255;
					nbCercles++;
				}
				else {
					//votes.at<uchar>(y,x) = 0;
				}
			}
		}*/
		
		for(int unsigned y = 0; y < votesCercles.size(); y++){
			for(int unsigned x = 0; x < votesCercles[y].size(); x++){
				for(int unsigned r = 0; r < votesCercles[y][x].size(); r++){
					if (votesCercles[y][x][r] >= SEUIL_VOTE) {
						nbCercles++;
						//printf("Cerlces : x0 : %d, y0 : %d, Rayon : %d, votes : %d\n", x, y, r, votesCercles[y][x][r]);
						for( int j = 0; j < cercles.rows; j++ ) {
							for( int i = 0; i < cercles.cols; i++ ) {
								if (belongsToCircle(x, y, r, i, j)) {
									cercles.at<uchar>(i,j) = 255;
								}
							}
						}
					}
				} 
			} 
		} 
		/*for( int j = 0; j < cercles.rows; j++ ) {
			for( int i = 0; i < cercles.cols; i++ ) {
				if (belongsToCircle(200, 200, 50, i, j)) {
					cercles.at<uchar>(j,i) = 255;
				}
			}
		}*/
		printf("Pixels de contour : %d\n", nbPixelContour);

		// TODO faire boucle qui vérifie si le cercle a reçu assez de vote
		printf("Cercles : %d\n", nbCercles);		
		
		//Affichage de l'image
		//imshow( "out", imageOut );       
		//imshow( "votes", votes );       
		imshow( "Cercles", cercles );       

		waitKey(0); 
	} 
	
	return 0;
}

