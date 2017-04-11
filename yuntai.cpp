//********************************************
// 文件名：yuntai.cpp
// 作者：kevincole
// 功能：Yuntai类的实现，主要是实现对云台的控制
// 创建时间：2008.5.16
// 修改内容：
// 修改时间：
//********************************************

#define STRICT
#include <tchar.h>
#include <windows.h>
#include "yuntai.h"

//Construct
Yuntai::Yuntai()
{
	//初始化串口
	InitCOM();
}

//错误处理
int Yuntai::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}

//初始化串口
void Yuntai::InitCOM()
{
	LONG    lLastError = ERROR_SUCCESS;

	//尝试打开串口(COM1)
	lLastError = m_Serial.Open(_T("COM1"));;
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to open COM-port"));

	// 设置串口(9600,N81) 使用硬件握手
    lLastError = m_Serial.Setup(CSerial::EBaud4800,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to set COM-port setting"));

	// 创建握手
    lLastError = m_Serial.SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to set COM-port handshaking"));

	//使用默认指令集初始化各控制码;
	DefaultCodeSet();
}

//默认指令集初始化程序控制码
void Yuntai::DefaultCodeSet()
{
	StopCode[0] = 0x0f;
	StopCode[1] = 0xff;
	StopCode[2] = 0xff;
	StopCode[3] = 0xff;

	UpCode[0] = 0x1f;
	UpCode[1] = 0xff;
	UpCode[2] = 0xff;
	UpCode[3] = 0xff;

	DownCode[0] = 0x2f;
	DownCode[1] = 0xff; 
	DownCode[2] = 0xff;
	DownCode[3] = 0xff;

	LeftCode[0] = 0x3f;
	LeftCode[1] = 0xff;
	LeftCode[2] = 0xff;
	LeftCode[3] = 0xff;

	RightCode[0] = 0x4f;
	RightCode[1] = 0xff;
	RightCode[2] = 0xff;
	RightCode[3] = 0xff;

	FarFocusCode[0] = 0x5f;
	FarFocusCode[1] = 0xff;
	FarFocusCode[2] = 0xff;
	FarFocusCode[3] = 0xff;

	CloseFocusCode[0] = 0x6f;
	CloseFocusCode[1] = 0xff;
	CloseFocusCode[2] = 0xff;
	CloseFocusCode[3] = 0xff;

	ZoomInCode[0] = 0x7f;
	ZoomInCode[1] = 0xff;
	ZoomInCode[2] = 0xff;
	ZoomInCode[3] = 0xff;


	ZoomOutCode[0] = 0x8f;
	ZoomOutCode[1] = 0xff;
	ZoomOutCode[2] = 0xff;
	ZoomOutCode[3] = 0xff;

	XY[0] = 0;
	XY[1] = 0;
	XY[2] = 0;
	XY[3] = 0;
}

Yuntai::~Yuntai()
{
	// 关闭串口
	m_Serial.Close();
}

//向Set()函数传递目标坐标，其完成对云台的控制
void Yuntai::Set( int x, int y )
{
	XY[0] = x /10;
	XY[1] = x%10;
	XY[2] = y/10;
	XY[3] = y%10;
	m_Serial.Write(&XY,4);
}

//旋转控制向上
void Yuntai::Up() 
{
	m_Serial.Write( &(UpCode), 4) ;
}

//旋转控制向下
void Yuntai::Down() 
{
	m_Serial.Write( &(DownCode), 4 );
}

//旋转控制向左
void Yuntai::Left() 
{
	m_Serial.Write( &(LeftCode), 4);
}

//旋转控制向右
void Yuntai::Right() 
{
	m_Serial.Write( &(RightCode), 4 );
}

//镜头控制远聚焦
void Yuntai::FarFocus() 
{
	m_Serial.Write ( &(FarFocusCode),4);
}

//镜头控制近聚焦
void Yuntai::CloseFocus() 
{
	m_Serial.Write( &(CloseFocusCode),4);
}

//镜头控制放大
void Yuntai::ZoomIn() 
{
	m_Serial.Write( &(ZoomInCode),4);
}

//镜头控制缩小
void Yuntai::ZoomOut() 
{
	m_Serial.Write( &(ZoomOutCode),4);
}

//停止
void Yuntai::Stop() 
{
	m_Serial.Write( &(StopCode), 4 );
}


//end of Yuntai.cpp