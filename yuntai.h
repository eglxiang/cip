//********************************************
// 文件名：yuntai.h
// 作者：kevincole
// 功能：定义Yuntai类，主要是实现对云台的控制
// 创建时间：2008.5.16
// 修改内容：
// 修改时间：
//********************************************

#ifndef YUNTAI_H
#define YUNTAI_H

#include "Serial.h"

//Yuntai类的定义
class Yuntai
{
public:
	Yuntai ();
	~Yuntai ();

	//向Set()函数传递目标坐标，其完成对云台的控制
	void Set( int x, int y );

private: 
	//初始化串口
	void InitCOM();
	//错误处理
	int ShowError (LONG lError, LPCTSTR lptszMessage);
	//在为提供配置文件时，以默认指令集初始化程序控制码
	void DefaultCodeSet();

	//旋转控制
	void Up() ;
	void Down() ;
	void Right() ;
	void Left() ;
	void Stop() ;
	//镜头控制
	void FarFocus() ;
	void CloseFocus() ;
	void ZoomIn() ;
	void ZoomOut() ;

	//串口类对象
	CSerial m_Serial; 
	unsigned char XY[4];
	//旋转控制指令集
	unsigned char StopCode[4];
	unsigned char UpCode[4];
	unsigned char DownCode[4];
	unsigned char LeftCode[4];
	unsigned char RightCode[4];

	//镜头控制指令集
	unsigned char FarFocusCode[4];
	unsigned char CloseFocusCode[4];
	unsigned char ZoomInCode[4];
	unsigned char ZoomOutCode[4];

};  //end of Yuntai class

#endif //end of yuntai.h