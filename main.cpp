//////////////////////////////////////////////////////////////////////
// Video Capture using DirectShow
// Author: Shiqi Yu (shiqi.yu@gmail.com)
// Thanks to HardyAI@OpenCV China
// Last modification: April 18, 2008
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// 使用说明：
//   1. 将CameraDS.h CameraDS.cpp以及目录DirectShow复制到你的项目中
//   2. 菜单 Project->Settings->Settings for:(All configurations)->C/C++->Category(Preprocessor)->Additional include directories
//      设置为 DirectShow/Include
//   3. 菜单 Project->Settings->Settings for:(All configurations)->Link->Category(Input)->Additional library directories
//      设置为 DirectShow/Lib
//////////////////////////////////////////////////////////////////////

#include "camerads.h"
#include <highgui.h>
#include <stdio.h>

int main()
{
	int cam_count;

	//获取摄像头数目
	//cam_count = CCameraDS::CameraCount();
	//printf("There are %d cameras.\n", cam_count);

	//if(cam_count==0)
		//return -1;

	//打开第一个摄像头
	CCameraDS camera;
	
	if(! camera.OpenCamera(0,0))
	{
		fprintf(stderr, "Can not open camera.\n");
		return -1;
	}
	
	
	cvNamedWindow("camera");
	while(1)
	{
		//获取一帧
		IplImage *pFrame = camera.QueryFrame();

		//显示
		cvShowImage("camera", pFrame);

		if (cvWaitKey(20) == 'q')
			break;
	}
	camera.CloseCamera(); //可不调用此函数，CCameraDS析构时会自动关闭摄像头

	cvDestroyWindow("camera");
	return 0;
}