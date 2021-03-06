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
	BOOL SendCommand(SOCKET_DATA SocketData, BOOL bWait=FALSE);
	BOOL IsConnected();

	// Communcation

	void GetSysInfo(SOCKET_DATA SockData);
	void GetOpInfo(SOCKET_DATA SockData);
	void GetInfo(SOCKET_DATA SockData);
	void GetTotRatio(SOCKET_DATA SockData);
	void GetStTime(SOCKET_DATA SockData);
	void GetRunTime(SOCKET_DATA SockData);
	void GetEdTime(SOCKET_DATA SockData);
	void GetStripRatio(SOCKET_DATA SockData);
	void GetDef(SOCKET_DATA SockData);
	void Get2DReader(SOCKET_DATA SockData);
	void GetEngInfo(SOCKET_DATA SockData);
	void GetFdInfo(SOCKET_DATA SockData);
	void GetAoiInfo(SOCKET_DATA SockData);
	void GetMkInfo(SOCKET_DATA SockData);
	void GetMkInfoLf(SOCKET_DATA SockData);
	void GetMkInfoRt(SOCKET_DATA SockData);

	BOOL SetSysInfo();
	void SetOpInfo();
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

	// Communcation - Sub Item

	// SetOpInfo()
	void SetOpName();
	void SetDualTest();
	void SetSampleTest();
	void SetSampleShotNum();
	void SetTestMode();
	void SetRecoilerCcw();
	void SetUncoilerCcw();
	void SetAlignMethode();
	void SetDoorRecoiler();
	void SetDoorAoiUp();
	void SetDoorAoiDn();
	void SetDoorMk();
	void SetDoorEngrave();
	void SetDoorUncoiler();
	void SetSaftyMk();
	void SetCleannerAoiUp();
	void SetCleannerAoiDn();
	void SetUltraSonicAoiDn();
	void SetUltraSonicEngrave();
	void SetTotReelLen();
	void SetOnePnlLen();
	void SetTempPause();
	void SetTempStopLen();
	void SetLotCut();
	void SetLotCutLen();
	void SetLotCutPosLen();
	void SetLmtTotYld();
	void SetLmtPatlYld();
	void SetStripOutRatio();
	void SetCustomNeedRatio();
	void SetNumRangeFixDef();
	void SetNumContFixDef();
	void SetUltraSonicStTim();

	// SetInfo()
	void SetModelUpName();
	void SetModelDnName();
	void SetLotUpName();
	void SetLotDnName();
	void SetLayerUpName();
	void SetLayerDnName();
	void SetOrderNum();
	void SetShotNum();
	void SetTotOpRto();
	void SetTotVel();
	void SetPartVel();
	void SetMkDoneLen();
	void SetAoiDnDoneLen();
	void SetAoiUpDoneLen();
	void SetLotSerial();
	void SetMkVerfyLen();

	// SetTotRatio()
	void SetDefNumUp();
	void SetDefRtoUp();
	void SetGoodNumUp();
	void SetGoodRtoUp();
	void SetTestNumUp();
	void SetDefNumDn();
	void SetDefRtoDn();
	void SetGoodNumDn();
	void SetGoodRtoDn();
	void SetTestNumDn();
	void SetDefNumTot();
	void SetDefRtoTot();
	void SetGoodNumTot();
	void SetGoodRtoTot();
	void SetTestNumTot();

	// Set2DReader()
	void Set2DEngLen();
	void Set2DAoiLen();
	void Set2DMkLen();
	void Set2DMoveVel();
	void Set2DMoveAcc();
	void Set2DOneShotLen();

	// SetEngInfo()
	void SetEngLeadPitch();
	void SetEngPushOffLen();
	void SetEngTqVal();
	void SetEngAoiLen();
	void SetEngFdDiffMax();
	void SetEngFdDiffRng();
	void SetEngFdDiffNum();
	void SetEngBuffInitPos();
	void SetEngBuffCurrPos();

	// SetFdInfo()
	void SetFdVel();
	void SetFdAcc();
	void SetOnePnlVel();
	void SetOnePnlAcc();
	void SetFdDiffMax();
	void SetFdDiffRng();
	void SetFdDiffNum();

	// SetAoiInfo()
	void SetAoiLeadPitch();
	void SetAoiPushOffLen();
	void SetAoiTqVal();
	void SetAoiBuffShotNum();
	void SetAoiMkLen();

	// SetMkInfo()
	void SetMkLeadPitch();
	void SetMkPushOffLen();
	void SetMkBuffInitPos();
	void SetMkBuffCurrPos();
	void SetMkNumLf();
	void SetMkNumRt();
	void SetMkMaxNumLf();
	void SetMkMaxNumRt();

	// SetMkInfoLf()
	void SetMkInitPosLf();
	void SetMkInitVelLf();
	void SetMkInitAccLf();
	void SetMkFnlPosLf();
	void SetMkFnlVelLf();
	void SetMkFnlAccLf();
	void SetMkFnlTqLf();
	void SetMkHgtPosX1Lf();
	void SetMkHgtPosY1Lf();
	void SetMkHgtPosX2Lf();
	void SetMkHgtPosY2Lf();
	void SetMkHgtPosX3Lf();
	void SetMkHgtPosY3Lf();
	void SetMkHgtPosX4Lf();
	void SetMkHgtPosY4Lf();

	// SetMkInfoRt()
	void SetMkInitPosRt();
	void SetMkInitVelRt();
	void SetMkInitAccRt();
	void SetMkFnlPosRt();
	void SetMkFnlVelRt();
	void SetMkFnlAccRt();
	void SetMkFnlTqRt();
	void SetMkHgtPosX1Rt();
	void SetMkHgtPosY1Rt();
	void SetMkHgtPosX2Rt();
	void SetMkHgtPosY2Rt();
	void SetMkHgtPosX3Rt();
	void SetMkHgtPosY3Rt();
	void SetMkHgtPosX4Rt();
	void SetMkHgtPosY4Rt();

	// End for SetSysInfo()


protected:
	afx_msg LRESULT wmClientReceived(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT wmClientClosed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateConnection(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

