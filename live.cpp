
#include <stdio.h>

#include <cv.h>

#include <highgui.h>


#include "api.h"



IplImage *prevframe = NULL;
IplImage* hsv=NULL; 
IplImage* h_plane=NULL;
IplImage* s_plane=NULL ;
IplImage* v_plane=NULL ;
IplImage* afterCanny=NULL;
IplImage* frame=NULL;
IplImage* mask = NULL;
IplImage* eig_image=NULL;
IplImage* tmp_image=NULL;
IplImage* pyrA=NULL;
IplImage* pyrB=NULL;
IplImage* init_img=NULL;
int *coords = NULL;
int flagcount = 0;
bool *flag=NULL;
int *coordsToTrack = NULL;
int finalCoordNum =0 ;
int x_mean = 0 , y_mean = 0;
int *vel,*finalvel,*prevel;
const int MAX_CORNERS = 200;
#define SENSITIVITY 2 


IplImage *imgA = NULL , *imgB = NULL;


int _resCalculator(IplImage *img1 , IplImage *img2 ,int initial,int final)
{
 int res = 0;
 for(int k = -1*img2->widthStep ; k <= 1*img2->widthStep ; k+= img2->widthStep)
 {
   for(int j = -1 ; j <= 1 ; j++)
   {
   	 
     if(final+j+k > 0 && final+j+k < img1->imageSize)
	{

	 //printf("\n\nIt's getting stuck here (%d,%d)\n\n",k/imgA->widthStep,j);
     res += (int)(abs( img1->imageData[ initial + j + k]  - img2->imageData[final + j + k ]) );

    }
   }
 }
 return res;
}

int* OpticalFlowCalculator(IplImage *img1 , IplImage *img2 )
{

// Initialize, load two images from the file system, and
// allocate the images and other structures we will need for
// results.
	vel[0] = vel[1] = 0;
   cvCvtColor(img1,imgA,CV_BGR2GRAY);
   cvCvtColor(img2,imgB,CV_BGR2GRAY);
   CvSize     img_sz = cvGetSize( imgA );
   int widthStep = imgA->widthStep;
// The first thing we need to do is get the features
// we want to track.

  
//	printf("\n\nHELLO BHAVYA\n\n");
	//cvNamedWindow("INIT_IMAGE",CV_WINDOW_AUTOSIZE);
 	//cvShowImage("INIT_IMAGE",init_img);
	//cvWaitKey(0);
    //printf("\n\nafter image\n\n");

//	printf("\n\nHi bhavya \n\n");
    
    int corner_count = MAX_CORNERS;
    CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];
  	int res,min,velx,vely;
  	min = 100000;
  cvGoodFeaturesToTrack(init_img,eig_image,tmp_image,cornersA,&corner_count,0.01,5.0,0,3,0,0.04);
	int counter = corner_count;
  for(int i=0; i<corner_count ; i++)
  {
  		if( abs(x_mean - ( cornersA[i].x )) + abs( y_mean - (cornersA[i].y ) ) > 200 )

		{

	//		printf("\n%d\n",i);
		
  			for(int j=-10*imgA->widthStep ; j<10*imgA->widthStep;j+=imgA->widthStep )
  			{
  				for(int k=-10 ; k<10 ; k++ )
  				{
  					res = _resCalculator(imgA,imgB,cornersA[i].x + (cornersA[i].y*imgA->widthStep),cornersA[i].x + (cornersA[i].y*imgA->widthStep)+j+k);
  					if(res<min)
  					{
  		//				printf("The res is %d\n",res);
  						min = res;
  						velx = k;
  						vely = j/imgA->widthStep; 
  					}
  	//				printf("\nAt K no. %d \n",k);
  				}
  		//		printf("\nAt j no. %d\n ",j);

  			}
  		
  		
  		vel[0] += velx;
  		vel[1] += vely; 
  		//printf("hello");
  		}
  		else
  		{
  			counter--;
  		}
  		min = 100000;
  		
  }

  
  vel[0]/=counter;
  vel[1]/=counter;
  printf("The velocity passed is %d,%d\n\n",vel[0],vel[1]);
	//getchar();
  
  prevel[0] = vel[0];
  prevel[1] = vel[1];

  

  
  return vel;
 } 




void init(IplImage * img)
{
 hsv = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 ); 
 h_plane = cvCreateImage( cvGetSize(img), 8, 1 );
 s_plane = cvCreateImage( cvGetSize(img), 8, 1 );
 v_plane = cvCreateImage( cvGetSize(img), 8, 1 );
 afterCanny = cvCreateImage( cvGetSize(img),  8, 1 );
 imgA = cvCreateImage( cvGetSize(img),  8, 1 );
 imgB = cvCreateImage( cvGetSize(img),  8, 1 );
 mask = cvCreateImage( cvGetSize(img), 8, 1 );
 flag = (bool *)malloc(sizeof(bool ) * mask->imageSize) ;
 vel = (int*)malloc( sizeof(int) * 2 );
 prevel = (int*)malloc( sizeof(int) * 2 );
 prevel[0] = 0;
 prevel[1] = 0;
 finalvel = (int*) malloc( sizeof(int) * 2 );
 eig_image = cvCreateImage(cvSize(100,100), IPL_DEPTH_32F, 1 );
 tmp_image = cvCreateImage( cvSize(100,100), IPL_DEPTH_32F, 1 );
 pyrA = cvCreateImage( cvSize( img->width+8, img->height/3 ), IPL_DEPTH_32F, 1 );
 pyrB = cvCreateImage( cvSize( img->width+8, img->height/3 ), IPL_DEPTH_32F, 1 );
 init_img = cvCreateImage( cvSize(100,100), 8, 1 );
}

//img --> the Ipl frame
//arySize --> the size of co-oridante array, which is the list of contours
void getMaskHSV( IplImage * img, int * arySize, int *xArry, int * yArry ,int count)
{
	
	if(count == 0)
	{
	
	printf( "==========================================\n");
	
	int width = img->width;
	int height = img->height;
	int widthStep = img->widthStep;
	
	
	cvCvtColor( img, hsv, CV_BGR2HSV );


	
	IplImage* planes[] = { h_plane, s_plane };
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );


	cvSet( mask, cvScalar(255) );
	int widthStep_mono = mask->widthStep;
	

	int baseInd = -widthStep_mono;
	for( int h=0 ; h<height ; h++ )
	{
		baseInd += widthStep_mono;
		for(register int w=0 ; w < widthStep ; w++ )
		{
			if( (h_plane->imageData[ baseInd + w ] > (uchar)90   &&
			     h_plane->imageData[ baseInd + w ] < (uchar)110) && 
				s_plane->imageData[ baseInd + w ] < (uchar)0 )
			{
				mask->imageData[ baseInd + w  ] = (uchar)0;
			}
		}
	}
	for( int i = 0 ; i < mask->imageSize ; i++)
	{
		if(mask->imageData[i] == (uchar)0 )
		{
			flag[i] = true;
			flagcount ++ ;
		}
		else
		{
			flag[i] = false;
		}
	}
		
		printf("flagcount after first image is - %d \n\n",flagcount);
		return;
	}
	
	if(count<=10 && count >1)
	
	{
		for(int i = 0 ; i < mask->imageSize ; i++)
		{
			if( mask->imageData[i] != (uchar)0 && flag[i] == true)
			{
				printf("In the elimination loop");
				flag[i] = false;
				flagcount --;
			}
		}
		
		printf("At the count no. %d the flagcount is  -  %d\n\n",count , flagcount);		
		return;
	}
	
	
	//cvNamedWindow("maskinit",CV_WINDOW_AUTOSIZE);
        //cvShowImage("maskinit",mask);  
	
	//cvWaitKey(0);
	
	if(count == 11)
	{
	
	int j = 0,counter = 0;
	coords = (int *)malloc(sizeof(int ) * flagcount);
	for(int i = 0 ; i < mask->imageSize ; i++)
	{
		if(flag[i] == true)
		{
			//printf("The coords are %d\n\n",i);
			coords[j] = i;
			j++;
			counter++;
		}
		if(j >= flagcount)
		{
			//printf("The coords are over \n\n");
		}
	}
		cvSet(mask,cvScalar(255));
       		       		
       		for(int i=0 ; i < flagcount; i++)
       		{
       			//printf("The value of coords[i] is %d for frame 11\n ",coords[i]);
       			mask->imageData[coords[i]] = (uchar)0;
       		}
       		
       		
       		//printf("COUNT 11 HAS BEEN RECIEVED and the flagcount is %d\n\n",flagcount);
       		
       		for(int i = 0 ;i < flagcount ; i++)
       {
       		x_mean+=(int)( (coords[i]%(mask->widthStep) ) );
       		y_mean+=(int)( (coords[i]/(mask->widthStep) ) );
       		
       }
       
       
        
       x_mean/=flagcount;
       y_mean/=flagcount; 
       
       int dis,decrementx = 0,decrementy = 0,decrementnum = 0;
       
       for(int i = 0 ;i < flagcount ; i++)
       {
       		dis = abs( x_mean - coords[i]%mask->widthStep ) + abs( y_mean - coords[i]/mask->widthStep );
       		//printf("dis is equal to %d and flag is %d\n",dis,(int)flag[i]);
       		if( dis > 0 && flag[i] == true)
       		{
       //			printf("FUCK\n\n");
       			mask->imageData[coords[i]] = (uchar)255;
       			decrementx += coords[i]%mask->widthStep;
       			decrementy += coords[i]/mask->widthStep;
       			decrementnum++;
       			
       		}
       }
          
       
       
       x_mean *= flagcount;
       y_mean *= flagcount;
       x_mean = (x_mean - decrementx)/( flagcount - decrementnum );
       y_mean = (y_mean - decrementy)/( flagcount - decrementnum );
       
     //  printf("The final mean is at (%d,%d) \n\n",x_mean,y_mean);
       
       finalCoordNum = (flagcount - decrementnum);
       coordsToTrack = (int *)malloc(sizeof(int ) * finalCoordNum);
       int jadon = 0;
       for(int i = 0 ; i < mask->imageSize ; i++)
       {
       		if(jadon >= finalCoordNum)
       		{
       			break;
       		} 
       		if( mask->imageData[i] == (uchar)0 )
       		{
       			coordsToTrack[jadon] = i;
       			jadon++;
       		}
       		
       }
       //cvCircle(mask,cvPoint(x_mean,y_mean),50,cvScalar(128));        
      // cvNamedWindow("mask",CV_WINDOW_AUTOSIZE);
       //cvShowImage("mask",mask);       
       
       //cvWaitKey(10);
	
       		
	return;
	}
	
}
 
int main( int argc, char **argv )
{
 int count = 0;

 CvCapture *capture = 0;
 int key = 0;
 frame=0;

 /* initialize camera */
 capture = cvCaptureFromCAM( 0 );
 /* always check */
 if ( !capture )
 {
   fprintf( stderr, "Cannot open initialize webcam!\n" );
   return 1;
 }
    
 /* create a window for the video */
// cvNamedWindow( "result", CV_WINDOW_AUTOSIZE );
 frame = cvQueryFrame( capture );
 init(frame); 
 cmd_init();  //for mouse movements
 
 int *velocity;
 velocity = (int *)malloc(sizeof(int)*2);

 while(1) 
 {
	  frame = cvQueryFrame( capture );		  /* get a frame */
	  //velocity = NULL;

	  if (!frame)
		break;				  /* always check */
		//printf("count is %d\n",count);
	  if (count < 12)
		getMaskHSV( frame, NULL, NULL, NULL, count );
	  if (count >= 12)
	  {
	   // printf("MAnu chodu\n\n");
		velocity = OpticalFlowCalculator(frame,prevframe);

		if(count>15)
		{
			x_mean += velocity[0];
			y_mean += velocity[1];
			
			cmd_move( 2.5*(x_mean), 1.875*(y_mean ), -1 );	
			
		
	  //  printf( "%X\n\n", vel );
	//	  printf("The velocity at frame no. %d  with mean (%d,%d) \n\n",count,x_mean,y_mean);
		/*  cvWaitKey(0); 
		  
		 // display current frame
		 cvShowImage( "result", frame );
	 key = cvWaitKey( 10 ); */
		}
	  }
	  
	  
	  prevframe = cvCloneImage(frame);
	  count++;
 }

 
/* free memory */
// cvDestroyWindow( "result" );
 cvReleaseImage( &h_plane );
 cvReleaseImage( &frame );
 cvReleaseImage( &prevframe );
 cvReleaseImage( &s_plane );
 cvReleaseImage( &v_plane );
 cvReleaseImage( &hsv );
 cvReleaseImage( &mask );
 cvReleaseImage( &afterCanny );
 cvReleaseImage( &h_plane );
 cvReleaseImage( &eig_image);
 cvReleaseImage( &tmp_image);
 cvReleaseImage( &pyrA);
 cvReleaseImage( &pyrB);
 
 cvReleaseCapture( &capture );
 free(coordsToTrack);
 cvReleaseImage( &imgB );
 cvReleaseImage( &imgA );
 free(coords);
 free(flag);

 //cmd_move( 500, 10, -1 );

 return 0;
}

