//This file contains all the image processing functions
//This also has the Optical-Flow function made by Bhavya Agarwal



#ifndef _TRACKING__CV___H_
#define _TRACKING__CV___H_



#include <stdio.h>

#include <cv.h>
#include <highgui.h>



//Global Variables
//    I knw this is not a good practise, but, 
//    bhavya does not knw to manage code, and 
//    we are suffering due to his ignorance.
//    hehehhehehehe

/*
IplImage *prevframe = NULL;
IplImage* hsv=NULL; 
IplImage* h_plane=NULL;
IplImage* s_plane=NULL ;
IplImage* v_plane=NULL ;
IplImage* afterCanny=NULL;
IplImage* frame=NULL;
IplImage* mask = NULL;
int *coords = NULL;
int flagcount = 0;
bool *flag;
int *coordsToTrack = NULL;
int finalCoordNum =0 ;
int x_mean = 0 , y_mean = 0;
int *vel,*finalvel;

IplImage *imgA = NULL , *imgB = NULL;
*/



void init(IplImage * img);


//Function Declarations Follow

int* OpticalFlowCalculator(IplImage *img1 , IplImage *img2 );


//img --> the Ipl frame
//arySize --> the size of co-oridante array, which is the list of contours
void getMaskHSV( IplImage * img, int * arySize, int *xArry, int * yArry ,int count);


#endif
