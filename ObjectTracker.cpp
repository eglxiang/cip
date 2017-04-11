#include "ObjectTracker.h"
#include "utils.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "omp.h"

#define  MEANSHIFT_ITARATION_NO 5
#define  DISTANCE_ITARATION_NO 1
#define  ALPHA 1
#define  EDGE_DETECT_TRESHOLD  32

//extern datatransfer p_temp;


CObjectTracker::CObjectTracker(INT32 imW,INT32 imH,IMAGE_TYPE eImageType)
{
 
  m_nImageWidth = imW;
  m_nImageHeight = imH;
  m_eIMAGE_TYPE = eImageType;
  m_cSkipValue = 0;

  for (UBYTE8 i=0;i<MAX_OBJECT_TRACK_NUMBER;i++)
  {
	  m_sTrackingObjectTable[i].Status = false;
      for(SINT16 j=0;j<HISTOGRAM_LENGTH;j++)
		  m_sTrackingObjectTable[i].initHistogram[j] = 0;
  }

  m_nFrameCtr = 0;	
  m_uTotalTime = 0;
  m_nMaxEstimationTime = 0;
  m_cActiveObject = 0;
	TotalDist=0.0;
	LastDist=0.0;

  switch (eImageType)
  {
   case MD_RGBA:
    m_cSkipValue = 4 ;
	break ;
   case MD_RGB:
	m_cSkipValue = 3 ;
   break ;
  };
};

CObjectTracker::~CObjectTracker()
{
}

//set RGB components wrt to (x.y)
void CObjectTracker::SetPixelValues(IplImage *r,IplImage *g,IplImage *b,ULONG_32 pixelValues,SINT16 x,SINT16 y)
{
//  *(frame+(y*m_nImageWidth+x)*m_cSkipValue+2) = UBYTE8(pixelValues & 0xFF);
//  *(frame+(y*m_nImageWidth+x)*m_cSkipValue+1) = UBYTE8((pixelValues >> 8) & 0xFF);
//  *(frame+(y*m_nImageWidth+x)*m_cSkipValue) = UBYTE8((pixelValues >> 16) & 0xFF); 
	//setpix32f
	setpix8c(r, y, x, UBYTE8(pixelValues & 0xFF));
	setpix8c(g, y, x, UBYTE8((pixelValues >> 8) & 0xFF));
	setpix8c(b, y, x, UBYTE8((pixelValues >> 16) & 0xFF));
}


//initializes the object parameters 
void CObjectTracker::ObjectTrackerInitObjectParameters(SINT16 x,SINT16 y,SINT16 Width,SINT16 Height)
{

   m_cActiveObject = 0;

   m_sTrackingObjectTable[m_cActiveObject].X = x;
   m_sTrackingObjectTable[m_cActiveObject].Y = y;
   m_sTrackingObjectTable[m_cActiveObject].W = Width;
   m_sTrackingObjectTable[m_cActiveObject].H = Height;

   m_sTrackingObjectTable[m_cActiveObject].vectorX = 0;
   m_sTrackingObjectTable[m_cActiveObject].vectorY = 0;

  
   m_sTrackingObjectTable[m_cActiveObject].Status = true;
   m_sTrackingObjectTable[m_cActiveObject].assignedAnObject = false;
}

void CObjectTracker::ObjeckTrackerHandlerByUser(IplImage *frame)//,PID_ctl &pid_x,PID_ctl &pid_y)
{
   m_cActiveObject = 0;

   if (m_sTrackingObjectTable[m_cActiveObject].Status)
   {
	   if (!m_sTrackingObjectTable[m_cActiveObject].assignedAnObject)
	   {
		   FindHistogram( frame,
						  m_sTrackingObjectTable[m_cActiveObject].initHistogram);

           m_sTrackingObjectTable[m_cActiveObject].assignedAnObject = true;
	   }
	   else
	   {
		   FindNextLocation(frame);    

		   DrawObjectBox(frame);

		   yun_tai.Set( m_sTrackingObjectTable[m_cActiveObject].X , m_sTrackingObjectTable[m_cActiveObject].Y  );
		   //yun_tai.Set( m_sTrackingObjectTable[m_cActiveObject].X - m_nImageWidth/2 , m_sTrackingObjectTable[m_cActiveObject].Y - m_nImageHeight/2 );
	   }
   }

}
//Extracts the histogram of box
void CObjectTracker::FindHistogram(IplImage *frame, FLOAT32 (*histogram))
{
  SINT16  i = 0;
  SINT16  x = 0;
  SINT16  y = 0;
  UBYTE8  E = 0;
  double border[4096];
  UBYTE8  qR = 0,qG = 0,qB = 0;
  UINT32 numberOfPixel = 0;
  UINT32 numberOfPixel2=0;
  IplImage* r, * g, * b;
//IplImage *hsv,*hue;

    for(i=0;i<16;i++)
	{
	border[i]=0;
	}
	
	r = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	g = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	b = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	cvCvtPixToPlane( frame, b, g, r, NULL );	//divide color image into separate planes r, g, b. The exact sequence doesn't matter.
 	r->origin=frame->origin;
	g->origin=frame->origin;
	b->origin=frame->origin;
	   for (i=0;i<HISTOGRAM_LENGTH;i++) //reset all histogram
		   histogram[i] = 0.0;


	   
  //计算选择区域四周,当选定区域中的某些特征与四周的相同时则去掉，用以去除背景干扰 
  for (y = max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 - 5 , 0 );  //起始点y-5 , 0
	   y <= max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );     //起始点y , 0
	   y++ )
	   #pragma omp parallel for private(qR,qG,qB)
	  for (x = max( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );  //起始点x , 0
		  x <= min( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2 ,m_nImageWidth-1 );  //终止点x , 图像宽度－1
		  x++)
	  {
	     	qR = (UBYTE8)pixval8c( r, y, x )/16;//quantize R component  //访问该点像素，并量化
			qG = (UBYTE8)pixval8c( g, y, x )/16;//quantize G component
			qB = (UBYTE8)pixval8c( b, y, x )/16;//quantize B component
            border[(256*qR+16*qG+qB)]+=1;
			numberOfPixel2++;
		
	  }


	  
  for (y = min(m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2 , m_nImageHeight-1 );   //终止点y , 图像宽度－1
	   y <= min(m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2+5 , m_nImageHeight-1); //终止点y+5 , 图像宽度－1
	   y++ )
	   #pragma omp parallel for private(qR,qG,qB)
	  for ( x = max(m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );             //起始点x , 0
		    x <= min(m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2 , m_nImageWidth-1 ); //终止点x , 图像宽度－1
		    x++)
	  {
	     	qR = (UBYTE8)pixval8c( r, y, x )/16;//quantize R component
			qG = (UBYTE8)pixval8c( g, y, x )/16;//quantize G component
			qB = (UBYTE8)pixval8c( b, y, x )/16;//quantize B component

			border[(256*qR+16*qG+qB)]+=1;
			numberOfPixel2++;
	     
	  
	  }
	  
  for (y = max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );                 //起始点y , 0
	   y <= min( m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2 , m_nImageHeight-1 ); //终止点y , 图像宽度－1
	   y++)
	   #pragma omp parallel for private(qR,qG,qB)
	  for (x = max( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2-5 , 0 );           //起始点x-5 , 0
		   x<=max( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );              //起始点x , 0
		   x++)
		  {
			  qR = (UBYTE8)pixval8c( r, y, x )/16;//quantize R component
			  qG = (UBYTE8)pixval8c( g, y, x )/16;//quantize G component
			  qB = (UBYTE8)pixval8c( b, y, x )/16;//quantize B component

			  border[(256*qR+16*qG+qB)]+=1;
			  numberOfPixel2++;
			
		  }
	 
  for (y = max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );                       //起始点y , 0
	   y <= min( m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2 , m_nImageHeight-1 );       //终止点y , 图像宽度－1
	   y++)
	   #pragma omp parallel for private(qR,qG,qB)
	  for ( x = min( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2 , m_nImageWidth-1 );    //终止点x , 图像宽度－1
			x <= min( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2+5 , m_nImageWidth-1 ); //终止点x+5 , 图像宽度－1
			x++)
			{
			  qR = (UBYTE8)pixval8c( r, y, x )/16;//quantize R component
			  qG = (UBYTE8)pixval8c( g, y, x )/16;//quantize G component
			  qB = (UBYTE8)pixval8c( b, y, x )/16;//quantize B component

			  border[(256*qR+16*qG+qB)]+=1;//
			  numberOfPixel2++;
			
		  }

	  #pragma omp parallel for
  for(i=0;i<4096;i++)
  {
      border[i]=border[i]/numberOfPixel2;
  }
	
  
	//for all the pixels in the region
  for (y = max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );                    //起始点y , 0
	   y <= min( m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2 , m_nImageHeight-1 );    //终止点y , 图像宽度－1
	   y++)
	   #pragma omp parallel for private(qR,qG,qB)
	  for (x = max ( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );               //起始点x , 0
		   x <= min( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2 , m_nImageWidth-1 ); //终止点x , 图像宽度－1
		   x++)
			{
				
				qR = (UBYTE8)pixval8c( r, y, x )/16;//quantize R component
				qG = (UBYTE8)pixval8c( g, y, x )/16;//quantize G component
				qB = (UBYTE8)pixval8c( b, y, x )/16;//quantize B component

				if( border[(256*qR+16*qG+qB)] < 0.1 )
				{
					histogram[(256*qR+16*qG+qB)] += 1;	//HISTOGRAM_LENGTH=4096
					numberOfPixel++;
				}
				
			}
		
	  #pragma omp parallel for
  for (i=0;i<HISTOGRAM_LENGTH;i++) //normalize
	   histogram[i] = histogram[i]/numberOfPixel;

	cvReleaseImage(&r);
	cvReleaseImage(&g);
	cvReleaseImage(&b);

}
 //Draw box around object
void CObjectTracker::DrawObjectBox(IplImage *frame)
{
	CvPoint xx,yy,center;
	CvSize axes;

	xx.x =  (int) min ( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2+1 , m_nImageWidth-1 );

	yy.x = (int) max ( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );

	xx.y =  (int) min ( m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2+1 , m_nImageHeight-1 );

	yy.y = (int) max ( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );

	center.x = m_sTrackingObjectTable[m_cActiveObject].X;
	center.y = m_sTrackingObjectTable[m_cActiveObject].Y;
	axes.width = m_sTrackingObjectTable[m_cActiveObject].W/2;
	axes.height = m_sTrackingObjectTable[m_cActiveObject].H/2;

	//cvRectangle( frame,xx,yy,CV_RGB(0,255,255),1, 8, 0 );
	cvEllipse ( frame,center,axes,0,0,360,CV_RGB(0,255,255) );
}
// Computes weights and drives the new location of object in the next frame
void CObjectTracker::FindWightsAndCOM(IplImage *frame, FLOAT32 (*histogram))
{
  SINT16  i = 0;
  SINT16  x = 0;
  SINT16  y = 0;
  FLOAT32  sumOfWeights = 0;
  UBYTE8  qR = 0,qG = 0,qB = 0;
  FLOAT32   newX = 0.0;
  FLOAT32   newY = 0.0;
  IplImage* r, * g, * b;

  FLOAT32 *weights = new FLOAT32[HISTOGRAM_LENGTH];

  for ( i = 0 ; i < HISTOGRAM_LENGTH ; i++ )
  {
	  if (histogram[i] >0.0 )
		  weights[i] = m_sTrackingObjectTable[m_cActiveObject].initHistogram[i] / histogram[i];	//qu/pu(y0)
	  else
		  weights[i] = 0.0;
  }

	r = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	g = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	b = cvCreateImage( cvGetSize(frame), frame->depth, 1 );
	cvCvtPixToPlane( frame, b, g, r, NULL );	//divide color image into separate planes r, g, b. The exact sequence doesn't matter.
 	r->origin=frame->origin;
	g->origin=frame->origin;
	b->origin=frame->origin;

	
  for ( y = max( m_sTrackingObjectTable[m_cActiveObject].Y - m_sTrackingObjectTable[m_cActiveObject].H/2 , 0 );                    //起始点y , 0
	    y <= min( m_sTrackingObjectTable[m_cActiveObject].Y + m_sTrackingObjectTable[m_cActiveObject].H/2 , m_nImageHeight-1 );    //终止点y , 图像宽度－1
	    y++ )
		#pragma omp parallel for private(qR,qG,qB)
	  for ( x = max( m_sTrackingObjectTable[m_cActiveObject].X - m_sTrackingObjectTable[m_cActiveObject].W/2 , 0 );                //起始点x , 0
		    x <= min( m_sTrackingObjectTable[m_cActiveObject].X + m_sTrackingObjectTable[m_cActiveObject].W/2 , m_nImageWidth-1 ); //终止点x , 图像宽度－1
		    x++ )
	  {
			qR = (UBYTE8)pixval8c( r, y, x )/16;
			qG = (UBYTE8)pixval8c( g, y, x )/16;
			qB = (UBYTE8)pixval8c( b, y, x )/16;

		  newX += ( weights[(256*qR+16*qG+qB)] * x );
		  newY += ( weights[(256*qR+16*qG+qB)] * y );

		  sumOfWeights += weights[(256*qR+16*qG+qB)];
		}

	  //if( LastDist<0.75 || LastDist>0.85 ) ///////////////////////////////////////  如果出现遮挡则不更新麽表坐标
	  {                ///////////////////////////////////////
   if ( sumOfWeights>0 )
   {
	   m_sTrackingObjectTable[m_cActiveObject].X = SINT16((newX/sumOfWeights) + 0.5);	//update location
	   m_sTrackingObjectTable[m_cActiveObject].Y = SINT16((newY/sumOfWeights) + 0.5);
   }
	  }                ///////////////////////////////////////

	cvReleaseImage(&r);
	cvReleaseImage(&g);
	cvReleaseImage(&b);

   delete[] weights, weights = NULL;
}
// Returns the distance between two histograms.
FLOAT32 CObjectTracker::FindDistance(FLOAT32 (*histogram))
{
  SINT16  i = 0;
  FLOAT32 distance = 0;

  
  for(i=0;i<HISTOGRAM_LENGTH;i++)
	  distance += FLOAT32(sqrt(DOUBLE64(m_sTrackingObjectTable[m_cActiveObject].initHistogram[i]
                  *histogram[i])));

  return(sqrt(1-distance));
}
//An alternative distance measurement 
FLOAT32 CObjectTracker::CompareHistogram(UBYTE8 (*histogram))
{
  SINT16  i	 = 0;
  FLOAT32 distance = 0.0;
  FLOAT32 difference = 0.0;

  
  for (i=0;i<HISTOGRAM_LENGTH;i++)
  {
	  difference = FLOAT32(m_sTrackingObjectTable[m_cActiveObject].initHistogram[i]
                         -histogram[i]);

	  if (difference>0)
		  distance += difference;
	  else
  		  distance -= difference;
  }
  return(distance);
}

// Alpha blending: used to update initial histogram by the current histogram 
void CObjectTracker::UpdateInitialHistogram(FLOAT32 (*histogram))
{
  SINT16  i = 0;
  
  for (i=0; i<HISTOGRAM_LENGTH; i++)
	  m_sTrackingObjectTable[m_cActiveObject].initHistogram[i] = ALPHA*m_sTrackingObjectTable[m_cActiveObject].initHistogram[i]
                                                            +(1-ALPHA)*histogram[i];

}
// Mean-shift iteration 
void CObjectTracker::FindNextLocation(IplImage *frame)
{
	int i, j;
	SINT16 scale[3]={-3, 3, 0};
	FLOAT32 dist, optdist;
	SINT16 h, w, optX, optY;

	//try no-scaling
	FindNextFixScale(frame);
	optdist=LastDist;
	optX=m_sTrackingObjectTable[m_cActiveObject].X;
	optY=m_sTrackingObjectTable[m_cActiveObject].Y;

	//try one of the 9 possible scaling
	i=2;
	j=2;
	h=m_sTrackingObjectTable[m_cActiveObject].H;
	w=m_sTrackingObjectTable[m_cActiveObject].W;
	if(h+scale[i]>10 && w+scale[j]>10 && h+scale[i]<m_nImageHeight/2 && w+scale[j]<m_nImageWidth/2)
	{
		m_sTrackingObjectTable[m_cActiveObject].H=h+scale[i];
		m_sTrackingObjectTable[m_cActiveObject].W=w+scale[j];
		FindNextFixScale(frame);
		if( (dist=LastDist) < optdist )	//scaling is better
		{
			optdist=dist;;
		}
		else	//no scaling is better
		{
			m_sTrackingObjectTable[m_cActiveObject].X=optX;
			m_sTrackingObjectTable[m_cActiveObject].Y=optY;
			m_sTrackingObjectTable[m_cActiveObject].H=h;
			m_sTrackingObjectTable[m_cActiveObject].W=w;
		}
	};
	TotalDist+=optdist;	//the latest distance
}

void CObjectTracker::FindNextFixScale(IplImage *frame)
{
  UBYTE8  iteration = 0;
  SINT16 optX, optY;

  FLOAT32 *currentHistogram = new FLOAT32[HISTOGRAM_LENGTH];
  FLOAT32 dist, optdist=1.0;

  for (iteration=0; iteration<MEANSHIFT_ITARATION_NO; iteration++)
  {
	  FindHistogram(frame,currentHistogram); //current frame histogram, use the last frame location as starting point
   
      FindWightsAndCOM(frame,currentHistogram);//derive weights and new location
   
      //FindHistogram(frame,currentHistogram);   //uptade histogram
   
      //UpdateInitialHistogram(currentHistogram);//uptade initial histogram
	  if( ((dist=FindDistance(currentHistogram)) < optdist) || iteration==0 )
	  {
		  optdist=dist;
		  optX=m_sTrackingObjectTable[m_cActiveObject].X;
		  optY=m_sTrackingObjectTable[m_cActiveObject].Y;
//	  	  printf("%f->", dist);
	  }
	  else	//bad iteration, then find a better start point for next iteration
	  {
		m_sTrackingObjectTable[m_cActiveObject].X=(m_sTrackingObjectTable[m_cActiveObject].X+optX)/2;
		m_sTrackingObjectTable[m_cActiveObject].Y=(m_sTrackingObjectTable[m_cActiveObject].Y+optY)/2;
	  }
  }//end for
  m_sTrackingObjectTable[m_cActiveObject].X=optX;
  m_sTrackingObjectTable[m_cActiveObject].Y=optY;
  LastDist=optdist;	//the latest distance
  //UpdateInitialHistogram(currentHistogram);  //uptade target's initial histogram
  printf("%f\n", LastDist);
  delete[] currentHistogram, currentHistogram = NULL;
}
 
float CObjectTracker::GetTotalDist(void)
{
	return(TotalDist);
}