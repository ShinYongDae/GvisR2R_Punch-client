#pragma once

#include "afxwin.h"
#include "TcpIpClient.h"
#include "EngraveDef.h"


#define TIM_CONNECT			1
#define TIM_CHECK_CONNECT	10

#define DELAY_CHECK_CONNECT	5000
#define DELAY_RESPONSE		10000

class CEngrave : public CWnd
{
	HWND m_hParentWnd;
	CTcpIpClient* m_pClient;

	CString	m_strPortSvr, m_strAddrSvr;
	CString	m_strAddrCli;
	int m_nServerID;

	BOOL m_bWaitForResponse;
	CString m_strResponse;
	int m_nCmd;
	BOOL m_bCmdF;

	SOCKET_DATA m_SocketData;
	BOOL m_bAutoConnect;
	BOOL m_bTIM_CHECK_CONNECT;
	
	void StartClient(CString sAddrCli, CString sAddrSvr, CString sPortSvr);
	void StopClient();
	BOOL CheckResponse(int nCmd, CString sResponse);
	BOOL WaitResponse();

public:
	CEngrave(CString sAddrCli, CString sAddrSvr, CString sPortSvr, CWnd* pParent = NULL);
	~CEngrave();

	void SetHwnd(HWND hParentWnd);
	BOOL IsRunning();
	int WriteComm(CString sMsg, DWORD dwTimeout = INFINITE);
	BOOL ReadComm(CString &sData);
	SOCKET_DATA GetSocketData();

	// Thread
	CEvent      m_evtThread;
	CWinThread *m_pThread;
	HANDLE		m_hThread;
	BOOL		m_bModify;
	BOOL		m_bAlive;

	void StartThread();
	void StopThread();
	void WaitUntilThreadEnd(HANDLE hThread);
	static UINT RunThread(LPVOID pParam);

	int OnClientReceived(WPARAM wParam, LPARAM lParam);
	void Close();

	// General Function
	BOOL SendCommand(SOCKET_DATA SocketData);
	BOOL IsConnected();

	// Communcation
	BOOL SetSysInfo();
	void SetInfo();
	void SetTotRatio();
	void SetStTime();
	void SetRunTime();
	void SetEdTime();
	void SetStripRatio();
	void SetDef();
	void Set2DReader();
	void SetEngInfo();
	void SetFdInfo();
	void SetAoiInfo();
	void SetMkInfo();
	void SetMkInfoLf();
	void SetMkInfoRt();

protected:
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateConnection(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

