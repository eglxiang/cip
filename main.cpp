//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to HardyAI@OpenCV China
// Last modification: April 18, 2008
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// ʹ��˵����
//   1. ��CameraDS.h CameraDS.cpp�Լ�Ŀ¼DirectShow���Ƶ������Ŀ��
//   2. �˵� Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      ����Ϊ DirectShow/Include
//   3. �˵� Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      ����Ϊ DirectShow/Lib
//////////////////////////////////////////////////////////////////////

#include "camerads.h"
#include <highgui.h>
#include <stdio.h>

int main()
{
	int cam_count;

	//��ȡ����ͷ��Ŀ
	//cam_count = CCameraDS::CameraCount();
	//printf("There are %d cameras.\n", cam_count);

	//if(cam_count==0)
		//return -1;

	//�򿪵�һ������ͷ
	CCameraDS camera;
	
	if(! camera.OpenCamera(0,0))
	{
		fprintf(stderr, "Can not open camera.\n");
		return -1;
	}
	
	
	cvNamedWindow("camera");
	while(1)
	{
		//��ȡһ֡
		IplImage *pFrame = camera.QueryFrame();

		//��ʾ
		cvShowImage("camera", pFrame);

		if (cvWaitKey(20) == 'q')
			break;
	}
	camera.CloseCamera(); //�ɲ����ô˺�����CCameraDS����ʱ���Զ��ر�����ͷ

	cvDestroyWindow("camera");
	return 0;
}