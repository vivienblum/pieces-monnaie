#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

#define SEUIL 50    
#define SEUIL_VOTE 33
#define SEUIL_MAG 10
#define SEUIL_RAYON 10
#define PI 3.14159265
#define DELTA_DROITE 0.2
#define DELTA_CERCLE 50
#define DELTA_CENTRE 2
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
		/*int first = 0;
		int limitFirst = 100000000;
		for( int y = 0; y < angle.rows && first < limitFirst; y++ ) {
			for( int x = 0; x < angle.cols && first < limitFirst; x++ ) {
				//Si c'est un point de contour
				if (mag.at<uchar>(y,x) > SEUIL_MAG) {
					int angleVal = angle.at<uchar>(y,x);
					double pente = getPente(x, y, angleVal);
					double ord = getOrdOrigine(x, y, pente);
					nbPixelContour++;
					first++;
				
					int firstDroite = 0;
					int limitDroite = 1;
					for( int j = 0; j < imageOut.rows; j++ ) {
						for( int i = 0; i < imageOut.cols; i++ ) {
							//Pour chaque point de la droite
							//if (j <= pente*i + ord + DELTA_DROITE && j >= pente*i + ord - DELTA_DROITE) {
							if (belongsToDoite(pente, ord, i, j)) {
								int rayon = getRayon(x, y, i, j);
								if (rayon > SEUIL_RAYON && rayon < MAX_RAYON) {
									//printf("Rayon : %d\n", rayon);
									//votesCercles[j][i][rayon] ++;
									//cercles.at<uchar>(j,i) = 255;
									int xCercle = i;
									int yCercle = j;
									int rayonCercle = rayon;
									int max = votesCercles[j][i][rayon];
									//On vérifie s'il n'existe pas déjà un cercle
									//printf("L: départ : %d, arrivée: %d \n", j - DELTA_CENTRE, j+DELTA_CENTRE);
									for(int unsigned l = j - DELTA_CENTRE; l < j + DELTA_CENTRE && l < votesCercles.size() && l >= 0; l++){
										for(int unsigned k = i - DELTA_CENTRE; k < i + DELTA_CENTRE && k < votesCercles[l].size() && k >= 0; k++){
											for(int unsigned r = rayon - DELTA_RAYON; r < rayon + DELTA_RAYON && r < votesCercles[l][k].size() && r >= 0; r++){
												if (votesCercles[l][k][r] > max) {
													xCercle = k;
													yCercle = l;
													rayonCercle = r;
												}
											}
										}
									}
									votesCercles[yCercle][xCercle][rayonCercle]++;
								}
							}
						}
					}
				}
			}
		}*/
		/*Parcours des pixels de contour*/
		/*for( int y1 = 0; y1 < imageOut.rows; y1++ ) {
			for( int x1 = 0; x1 < imageOut.cols; x1++ ) {
				if (imageOut.at<uchar>(y1,x1) > 0) {
					for( int y2 = 0; y2 < imageOut.rows; y2++ ) {
						for( int x2 = 0; x2 < imageOut.cols; x2++ ) {
							if (imageOut.at<uchar>(y2,x2) > 0) {
								for( int y3 = 0; y3 < imageOut.rows; y3++ ) {
									for( int x3 = 0; x3 < imageOut.cols; x3++ ) {
										if (imageOut.at<uchar>(y3,x3) > 0) {
										//On vote pour le cercle qui passe par ces 3 points
										}
									}
								}
							}
						}
					}
				}
			}
		}*/
		//On regarde les votes
		/*for(int unsigned y = 0; y < votesCercles.size(); y++){
			for(int unsigned x = 0; x < votesCercles[y].size(); x++){
				for(int unsigned r = 0; r < votesCercles[y][x].size(); r++){
					if (votesCercles[y][x][r] >= SEUIL_VOTE) {
						nbCercles++;
						//printf("Cerlces : x0 : %d, y0 : %d, Rayon : %d, votes : %d\n", x, y, r, votesCercles[y][x][r]);
						//On trace les cercles
						for( int j = 0; j < cercles.rows; j++ ) {
							for( int i = 0; i < cercles.cols; i++ ) {
								if (belongsToCircle(x, y, r, i, j)) {
									cercles.at<uchar>(j,i) = 255;
								}
							}
						}
					}
				} 
			} 
		}*/
		printf("Pixels de contour : %d\n", nbPixelContour);
		printf("Cercles : %d\n", nbCercles);		
		
		//Affichage de l'image
		//imshow( "out", imageOut );       
		//imshow( "votes", votes );       
		imshow( "Cercles", cercles );       

		waitKey(0); 
	} 
	
	return 0;
}

