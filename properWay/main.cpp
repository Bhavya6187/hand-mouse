//Please note, some of the variables used here are global variables
//These global variables are defined in "trackingCV/trackingCV.h"


//Standard Headers
#include <stdio.h>
#include <stdlib.h>


//OpenCV Headers
#include <cv.h>
#include <highgui.h>


//Headers by us
#include "trackingCV/trackingCV.h"



//Global Variables

extern IplImage *prevframe = NULL;
extern IplImage* hsv=NULL; 
extern IplImage* h_plane=NULL;
extern IplImage* s_plane=NULL ;
extern IplImage* v_plane=NULL ;
extern IplImage* afterCanny=NULL;
extern IplImage* frame=NULL;
extern IplImage* mask = NULL;
extern int *coords = NULL;
extern int flagcount = 0;
extern bool *flag;
extern int *coordsToTrack = NULL;
extern int finalCoordNum =0 ;
extern int x_mean = 0 , y_mean = 0;
extern int *vel,*finalvel;

extern IplImage *imgA = NULL ;
extern IplImage *imgB = NULL;




int main( int argc, char **argv )
{
    
    int count = 0;
       
    CvCapture *capture = 0;
       int       key = 0;
       frame=0;

    /* initialize camera */

    capture = cvCaptureFromCAM( 0 );

    /* always check */

    if ( !capture ) {

        fprintf( stderr, "Cannot open initialize webcam!\n" );

        return 1;

    }
    
    

   /* create a window for the video */

        cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
	frame = cvQueryFrame( capture );
	init(frame); 
	
	
        while( key != 'q' ) 
        {
        
       // printf("in frame no. - %d\n\n",count);
        /* get a frame */
        frame = cvQueryFrame( capture );

	
 
        /* always check */

        if( !frame ) break;
       	getMaskHSV( frame, NULL, NULL, NULL,count );
       	
        
       int *velocity=NULL;
       if(count >= 11)
       {
     	 velocity = OpticalFlowCalculator (frame,prevframe) ;
     	// printf( "%X\n\n", vel );
       printf("The velocity is (%d,%d) at frame no. %d   \n\n",velocity[0],velocity[1],count);
       }
       prevframe = cvCloneImage(frame);
             
             
       //
      // cvWaitKey(0); 
        /* display current frame */
        cvShowImage( "result", frame );
        
	key = cvWaitKey( 10 );
        count++;

    }

 

    /* free memory */

    cvDestroyWindow( "result" );
    cvReleaseImage( &h_plane );
    cvReleaseImage( &frame );
    cvReleaseImage( &prevframe );
    cvReleaseImage( &s_plane );
    cvReleaseImage( &v_plane );
    cvReleaseImage( &hsv );
    cvReleaseImage( &mask );
    cvReleaseImage( &afterCanny );
    cvReleaseImage( &h_plane );

    cvReleaseCapture( &capture );
    free(coordsToTrack);
    cvReleaseImage( &imgB );
    cvReleaseImage( &imgA );
    free(coords);
    free(flag);

 

    return 0;

}

