#include "video_Iterator.h"
//#include "tbb/tick_count.h"

//HANDLE evRead;
//extern datatransfer p_temp;

void video_ietrator_( int argc_ , char** argv_ )
{
  IplImage *frame;
  /************************************/
  CvVideoWriter* Video_Writer;   //��Ƶ�洢
  /***********************************/
  CvRect* regions;
  CvSize size;
 
  int i, j;
  CObjectTracker *m_pObjectTracker;
  //char* video_file;                   /* input video file name */
  //����PID����
  //PID_ctl pid_x,pid_y;

  //evRead = CreateEvent (NULL ,FALSE ,FALSE ,NULL) ;   //�����¼�

  /*HANDLE threadHandle;
  threadHandle = CreateThread( NULL,                // Security attributes
								0,                   // Stack size
								yuntai_control,      // Thread function
								NULL,                // Data for thread func()
								0,    // Thread start mode  CREATE_SUSPENDED
								NULL
							   );*/

  //�򿪵�һ������ͷ
  CCameraDS camera;

  //��ʼ�����ļ��л�ȡ��Ƶ
  if(! camera.OpenCamera(0,0))
	  cerr<<"Can not open camera."<<endl;
    
	i = 0;
	while( 1)//frame = camera.QueryFrame())   
	{
		//��ֱ��ˮƽ�򼴴�ֱ��ˮƽ��ת��ά����
		//cvFlip( frame, NULL, 0 );
//		tbb::tick_count t0 = tbb::tick_count::now();
		frame = camera.QueryFrame();

		// allow user to select object to be tracked in the first frame
		if(i==0)	//the reference frame
		{
			size.height=frame->height;
			size.width=frame->width;
			get_regions(frame, &regions);//Ȧ������Ŀ������

			//��ʼ��PID
			/*pid_x.SetPoint = frame->width/2;
			pid_y.SetPoint = frame->height/2;
			pid_x.NextPoint = frame->width/2;
			pid_y.NextPoint = frame->height/2;*/

			j=0;	//index of objects

			m_pObjectTracker = new CObjectTracker(size.width, size.height, MD_RGB);
			m_pObjectTracker->ObjectTrackerInitObjectParameters( regions[j].x + regions[j].width/2,
																 regions[j].y+regions[j].height/2, 
																 regions[j].width, 
																 regions[j].height );
			/*********************************************************************************/
			Video_Writer = cvCreateVideoWriter( "out.avi",CV_FOURCC('X', 'V', 'I', 'D'), 5, size, 1 );   //��Ƶ�洢  CV_FOURCC('X', 'V', 'I', 'D')
			/*********************************************************************************/
		}
		else if(i>0)	//do tracking in the following sequences
		{
			// perform tracking on the frame, target position is drawn on frame
			m_pObjectTracker->ObjeckTrackerHandlerByUser(frame);//,pid_x,pid_y);
//			tbb::tick_count t1 = tbb::tick_count::now();
//			cout<<"t"<<(t1-t0).seconds()<<endl;;
			//p_temp.x = pid_x.PIDCalc();
			//p_temp.y = pid_y.PIDCalc();
			//SetEvent (evRead);    //�������̨�����¼�
			/****************************************************/
			cvWriteFrame( Video_Writer, frame );  //��Ƶ�洢
			/****************************************************/
		}
    
	cvNamedWindow( "Video", 1 );
	cvShowImage( "Video", frame );



	if( cvWaitKey(1) == 'q' )
		break;

	i++;	//count number of frames
	}//end while

	cout<<"total frame: i="<<i
		<<", total distance="<<m_pObjectTracker->GetTotalDist()
		<<", average distance="<<m_pObjectTracker->GetTotalDist()/i
		<<endl;

  /****************************************************/
  cvReleaseVideoWriter( &Video_Writer );  //��Ƶ�洢
  /****************************************************/

  delete m_pObjectTracker, m_pObjectTracker = 0;
}

