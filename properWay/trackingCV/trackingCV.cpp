#include "trackingCV.h"



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





int _resCalculator(IplImage *img1 , IplImage *img2 ,int initial,int final)
{
	int res = 0;
	for(int k = -1*img2->widthStep ; k <= 1*img2->widthStep ; k+= img2->widthStep)
	{
		for(int j = -1 ; j <= 1 ; j++)
		{
			if(final+j+k > 0 && final+j+k < img1->imageSize)
			res += (int)(abs( img1->imageData[ initial ]  - img2->imageData[final + j + k ]) );
		}
	}
	return res;
}


int* OpticalFlowCalculator(IplImage *img1 , IplImage *img2 )
{
	int res,min;
	
	
	
	//printf( "in func opticalFlowCalc: memaddr %X %X\n\n", vel, finalvel );
	
	
	finalvel[0] = 0;
	finalvel[1]= 0;
	//IplImage *imgA = NULL , *imgB = NULL;
	cvCvtColor( img1, imgA, CV_BGR2GRAY );
        cvCvtColor( img2, imgB, CV_BGR2GRAY );
        min = 100000;
	for(int i = 0 ; i < finalCoordNum ; i++)
	{
		for(int j = -5 ; j <= 5 ; j++)
		{
			for(int k = -5*imgA->widthStep ; k <= 5*imgA->widthStep ; k+= imgA->widthStep)
			{
				if( coordsToTrack[i] >= 10*imgA->widthStep  )
				{
						res = _resCalculator(imgA,imgB,coordsToTrack[i],coordsToTrack[i]+j+k);
						//printf("The res value for pixel pos %d is %d with j = %d and k = %d \n ",i,res,j,k);
						if(res<min)
						{
							
							min = res;
							vel[0] = j;
							vel[1] = k/imgA->widthStep;
						//	printf("Velocity is converging to (%d,%d) with res = %d \n",vel[0],vel[1],res);
						}		
				}
			}
		}
		
		coordsToTrack[i]  += (vel[0]) + (vel[1]*imgA->widthStep);
		finalvel[0] += vel[0];
		finalvel[1] += vel[1];
		//printf("Intermediate velocity is (%d,%d)\n",vel[0],vel[1]);
		vel[0] = vel[1] = 0;
		min = 100000;
	}
	
	finalvel[0]/=(finalCoordNum/50);
	finalvel[1]/=(finalCoordNum/50);
	//printf("Intermediate velocity is (%d,%d)\n",finalvel[0],finalvel[1]);
	return finalvel;
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
	finalvel = (int*) malloc( sizeof(int) * 2 );
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

/*
	cvSaveImage( "h_plane.png", h_plane );
	cvSaveImage( "s_plane.png", s_plane );
	exit(1);
*/
	
	cvSet( mask, cvScalar(255) );
	int widthStep_mono = mask->widthStep;
	
	
	int baseInd = -widthStep_mono;
	for( int h=0 ; h<height ; h++ )
	{
		baseInd += widthStep_mono;
		for( int w=0 ; w < widthStep ; w++ )
		{
			if( (h_plane->imageData[ baseInd + w ] > (uchar)90  && h_plane->imageData[ baseInd + w ] < (uchar)110)     && 
				s_plane->imageData[ baseInd + w ] < (uchar)0   )
			{
				//printf( "saturation : %d\n", (int)s_plane->imageData[ baseInd + w ] );
				mask->imageData[ baseInd + w  ] = (uchar)0;
			}
		}
	}
	for(int i = 0 ; i < mask->imageSize ; i++)
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
       
       printf("The final mean is at (%d,%d) \n\n",x_mean,y_mean);
       
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
 
