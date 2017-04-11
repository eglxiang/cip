//********************************************
// �ļ�����yuntai.h
// ���ߣ�kevincole
// ���ܣ�����Yuntai�࣬��Ҫ��ʵ�ֶ���̨�Ŀ���
// ����ʱ�䣺2008.5.16
// �޸����ݣ�
// �޸�ʱ�䣺
//********************************************

#ifndef YUNTAI_H
#define YUNTAI_H

#include "Serial.h"

//Yuntai��Ķ���
class Yuntai
{
public:
	Yuntai ();
	~Yuntai ();

	//��Set()��������Ŀ�����꣬����ɶ���̨�Ŀ���
	void Set( int x, int y );

private: 
	//��ʼ������
	void InitCOM();
	//������
	int ShowError (LONG lError, LPCTSTR lptszMessage);
	//��Ϊ�ṩ�����ļ�ʱ����Ĭ��ָ���ʼ�����������
	void DefaultCodeSet();

	//��ת����
	void Up() ;
	void Down() ;
	void Right() ;
	void Left() ;
	void Stop() ;
	//��ͷ����
	void FarFocus() ;
	void CloseFocus() ;
	void ZoomIn() ;
	void ZoomOut() ;

	//���������
	CSerial m_Serial; 
	unsigned char XY[4];
	//��ת����ָ�
	unsigned char StopCode[4];
	unsigned char UpCode[4];
	unsigned char DownCode[4];
	unsigned char LeftCode[4];
	unsigned char RightCode[4];

	//��ͷ����ָ�
	unsigned char FarFocusCode[4];
	unsigned char CloseFocusCode[4];
	unsigned char ZoomInCode[4];
	unsigned char ZoomOutCode[4];

};  //end of Yuntai class

#endif //end of yuntai.h