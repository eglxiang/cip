#include "regions_mouse.h"

/*
  Allows the user to interactively select object regions.

  @param frame the frame of video in which objects are to be selected
  @param regions a pointer to an array to be filled with rectangles
    defining object regions

  @return Returns the number of objects selected by the user
*/
int get_regions( IplImage* frame, CvRect** regions )
{
  char* win_name = "First frame";
  params p;
  CvRect* r;
  int i, x1, y1, x2, y2, w, h;
  
  /* use mouse callback to allow user to define object regions */
  p.win_name = win_name;
  p.orig_img = (IplImage *)cvClone( frame );
  p.cur_img = NULL;
  p.n = 0;
  cvNamedWindow( win_name, 1 );
  cvShowImage( win_name, frame );
  cvSetMouseCallback( win_name, &mouse, &p );
  cvWaitKey( 0 );
  cvDestroyWindow( win_name );
  cvReleaseImage( &(p.orig_img) );
  if( p.cur_img )
    cvReleaseImage( &(p.cur_img) );

  /* extract regions defined by user; store as an array of rectangles */
  if( p.n == 0 )
    {
      *regions = NULL;
      return 0;
    }
  r = (CvRect *)malloc( p.n * sizeof( CvRect ) );
  for( i = 0; i < p.n; i++ )	//for each rectangle round the object
    {
      x1 = MIN( p.loc1[i].x, p.loc2[i].x );
      x2 = MAX( p.loc1[i].x, p.loc2[i].x );
      y1 = MIN( p.loc1[i].y, p.loc2[i].y );
      y2 = MAX( p.loc1[i].y, p.loc2[i].y );
      w = x2 - x1;
      h = y2 - y1;

      /* ensure odd width and height */
      w = ( w % 2 )? w : w+1;
      h = ( h % 2 )? h : h+1;
      r[i] = cvRect( x1, y1, w, h );	//define one of the rects
    }
  *regions = r;
  return p.n;
}



/*
  Mouse callback function that allows user to specify the initial object
  regions.  Parameters are as specified in OpenCV documentation.
*/
void mouse( int event, int x, int y, int flags, void* param )
{
  params* p = (params*)param;
  CvPoint* loc;
  int n;
  IplImage* tmp;
  static int pressed = FALSE;

  int height=p->orig_img->height;
  
  /* on left button press, remember first corner of rectangle around object */
  if( event == CV_EVENT_LBUTTONDOWN )
    {
      n = p->n;
      if( n == MAX_OBJECTS )
	return;
      loc = p->loc1;
      loc[n].x = x;
      loc[n].y = y;/////////////////////////////////height-y;
      pressed = TRUE;
    }

  /* on left button up, finalize the rectangle and draw it in black */
  else if( event == CV_EVENT_LBUTTONUP )
    {
      n = p->n;
      if( n == MAX_OBJECTS )
	return;
      loc = p->loc2;
      loc[n].x = x;
      loc[n].y = y;//////////////////////////height-y;
      cvReleaseImage( &(p->cur_img) );
      p->cur_img = NULL;
      cvRectangle( p->orig_img, p->loc1[n], loc[n], CV_RGB(255,0,0), 1, 8, 0 );
      cvShowImage( p->win_name, p->orig_img );
      pressed = FALSE;
      p->n++;
    }

  /* on mouse move with left button down, draw rectangle as defined in white */
  else if( event == CV_EVENT_MOUSEMOVE  &&  flags & CV_EVENT_FLAG_LBUTTON )
    {
      n = p->n;
      if( n == MAX_OBJECTS )
	return;
      tmp = (IplImage *)cvClone( p->orig_img );
      loc = p->loc1;
      cvRectangle( tmp, loc[n], cvPoint(x, y), CV_RGB(255,0,0), 1, 8, 0 );/////////////////////cvPoint(x, height-y)
      cvShowImage( p->win_name, tmp );
      if( p->cur_img )
	cvReleaseImage( &(p->cur_img) );
      p->cur_img = tmp;
    }
}


