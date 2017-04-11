//********************************************
// �ļ�����yuntai.cpp
// ���ߣ�kevincole
// ���ܣ�Yuntai���ʵ�֣���Ҫ��ʵ�ֶ���̨�Ŀ���
// ����ʱ�䣺2008.5.16
// �޸����ݣ�
// �޸�ʱ�䣺
//********************************************

#define STRICT
#include <tchar.h>
#include <windows.h>
#include "yuntai.h"

//Construct
Yuntai::Yuntai()
{
	//��ʼ������
	InitCOM();
}

//������
int Yuntai::ShowError (LONG lError, LPCTSTR lptszMessage)
{
	// Generate a message text
	TCHAR tszMessage[256];
	wsprintf(tszMessage,_T("%s\n(error code %d)"), lptszMessage, lError);

	// Display message-box and return with an error-code
	::MessageBox(0,tszMessage,_T("Hello world"), MB_ICONSTOP|MB_OK);
	return 1;
}

//��ʼ������
void Yuntai::InitCOM()
{
	LONG    lLastError = ERROR_SUCCESS;

	//���Դ򿪴���(COM1)
	lLastError = m_Serial.Open(_T("COM1"));;
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to open COM-port"));

	// ���ô���(9600,N81) ʹ��Ӳ������
    lLastError = m_Serial.Setup(CSerial::EBaud4800,CSerial::EData8,CSerial::EParNone,CSerial::EStop1);
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to set COM-port setting"));

	// ��������
    lLastError = m_Serial.SetupHandshaking(CSerial::EHandshakeOff);
	if (lLastError != ERROR_SUCCESS)
		ShowError(m_Serial.GetLastError(), _T("Unable to set COM-port handshaking"));

	//ʹ��Ĭ��ָ���ʼ����������;
	DefaultCodeSet();
}

//Ĭ��ָ���ʼ�����������
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
	// �رմ���
	m_Serial.Close();
}

//��Set()��������Ŀ�����꣬����ɶ���̨�Ŀ���
void Yuntai::Set( int x, int y )
{
	XY[0] = x /10;
	XY[1] = x%10;
	XY[2] = y/10;
	XY[3] = y%10;
	m_Serial.Write(&XY,4);
}

//��ת��������
void Yuntai::Up() 
{
	m_Serial.Write( &(UpCode), 4) ;
}

//��ת��������
void Yuntai::Down() 
{
	m_Serial.Write( &(DownCode), 4 );
}

//��ת��������
void Yuntai::Left() 
{
	m_Serial.Write( &(LeftCode), 4);
}

//��ת��������
void Yuntai::Right() 
{
	m_Serial.Write( &(RightCode), 4 );
}

//��ͷ����Զ�۽�
void Yuntai::FarFocus() 
{
	m_Serial.Write ( &(FarFocusCode),4);
}

//��ͷ���ƽ��۽�
void Yuntai::CloseFocus() 
{
	m_Serial.Write( &(CloseFocusCode),4);
}

//��ͷ���ƷŴ�
void Yuntai::ZoomIn() 
{
	m_Serial.Write( &(ZoomInCode),4);
}

//��ͷ������С
void Yuntai::ZoomOut() 
{
	m_Serial.Write( &(ZoomOutCode),4);
}

//ֹͣ
void Yuntai::Stop() 
{
	m_Serial.Write( &(StopCode), 4 );
}


//end of Yuntai.cpp