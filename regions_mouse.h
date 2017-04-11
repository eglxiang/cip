#ifndef _REGIONS_MOUSE_H
#define _REGIONS_MOUSE_H

#include "defs.h"
//#include "RecogObjt.h"
#define MAX_OBJECTS 10	// maximum number of objects to be tracked

/********************************* Structures ********************************/

typedef struct params {
  CvPoint loc1[MAX_OBJECTS];
  CvPoint loc2[MAX_OBJECTS];
  IplImage* objects[MAX_OBJECTS];
  char* win_name;
  IplImage* orig_img;
  IplImage* cur_img;
  int n;
} params;

/***************************** Function Prototypes ***************************/
int get_regions( IplImage*, CvRect** );
void mouse( int, int, int, int, void* );

#endif
