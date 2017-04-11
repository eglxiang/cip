#include "defs.h"
#include "yuntai.h"

#if !defined(OBEJCTTRACKER_H_INCLUDED_)
#define OBEJCTTRACKER_H_INCLUDED_


#define  HISTOGRAM_LENGTH			8192
#define  MAX_OBJECT_TRACK_NUMBER	1 


typedef unsigned char   UBYTE8;
typedef char            BYTE8;
typedef short           SINT16;
typedef unsigned int    UINT32;
typedef int             INT32;
typedef unsigned long   ULONG_32;
typedef long            LONG_32;
typedef float           FLOAT32;
typedef double          DOUBLE64;
typedef long double     LDOUBLE80;
typedef bool		    BBOOL;

enum IMAGE_TYPE {MD_RGB, MD_RGBA, MD_YUV, MD_WAVELET};


class CObjectTracker
{
private:

	struct OBJECT_SPECS
	{
		BBOOL   Status;
		BBOOL   assignedAnObject;
		SINT16  X;
		SINT16  Y;
		SINT16  W;
		SINT16  H;
		SINT16  vectorX;
		SINT16  vectorY;
		FLOAT32  initHistogram[HISTOGRAM_LENGTH];
	};

	OBJECT_SPECS m_sTrackingObjectTable[MAX_OBJECT_TRACK_NUMBER+1];
	INT32	 m_nImageWidth;
	INT32	 m_nImageHeight;
	IMAGE_TYPE m_eIMAGE_TYPE;
	UBYTE8  m_cSkipValue;
	SINT16	 m_nFrameCtr;
	UBYTE8  m_cActiveObject;

	ULONG_32 m_uTotalTime;
	SINT16   m_nMaxEstimationTime;
	FLOAT32  TotalDist;	//record total distance to evaluate performance
	FLOAT32  LastDist;	//record the latest distance

	//ÔÆÌ¨¿ØÖÆ
	Yuntai yun_tai;

	void FindHistogram(IplImage *frame,FLOAT32 (*histogram));
	void DrawObjectBox(IplImage *frame);	//set to public
	void FindWightsAndCOM(IplImage *frame,FLOAT32 (*histogram));
	FLOAT32 FindDistance(FLOAT32 (*histogram));
	void UpdateInitialHistogram(FLOAT32 (*histogram));
	void FindNextLocation(IplImage *frame);
	void FindNextFixScale(IplImage *frame);
	void CObjectTracker::SetPixelValues(IplImage *r,IplImage *g,IplImage *b,ULONG_32 pixelValues,SINT16 x,SINT16 y);
	FLOAT32 CompareHistogram(UBYTE8 (*histogram));


public:

	CObjectTracker(INT32 imW,INT32 imH,IMAGE_TYPE eImageType);
	virtual ~CObjectTracker() ;
	void CObjectTracker::ObjeckTrackerHandlerByUser(IplImage *frame);//,PID_ctl &pid_x,PID_ctl &pid_y);
	float CObjectTracker::GetTotalDist(void);
	void ObjectTrackerInitObjectParameters(SINT16 x,SINT16 y,SINT16 Width,SINT16 Height);
};//end of track object class
//---------------------------------------------------------------------------
#endif
