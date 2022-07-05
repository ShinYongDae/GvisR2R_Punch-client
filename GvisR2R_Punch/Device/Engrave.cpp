#include "stdafx.h"
#include "Engrave.h"

#include "EngraveDef.h"
#include "../Process/GlobalDefine.h"
#include "../GvisR2R_PunchDoc.h"
#include "../GvisR2R_PunchView.h"

extern CGvisR2R_PunchDoc* pDoc;
extern CGvisR2R_PunchView* pView;

BEGIN_MESSAGE_MAP(CEngrave, CWnd)
	//{{AFX_MSG_MAP(CEngrave)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_MESSAGE(WM_UPDATE_CONNECTION, OnUpdateConnection)
	ON_WM_TIMER()
	ON_MESSAGE(WM_CLIENT_RECEIVED, wmClientReceived)
	ON_MESSAGE(WM_CLIENT_CLOSED, wmClientClosed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CEngrave::CEngrave(CString sAddrCli, CString sAddrSvr, CString sPortSvr, CWnd* pParent /*=NULL*/)
{
	m_hParentWnd = pParent->GetSafeHwnd();

	m_pClient = NULL;
	m_nServerID = ID_ENGRAVE;
	m_bWaitForResponse = FALSE;
	m_strResponse = _T("");
	m_nCmd = _None_;
	m_bCmdF = FALSE;
	m_bAutoConnect = TRUE;

	RECT rt = { 0,0,0,0 };
	Create(NULL, _T("None"), WS_CHILD, rt, FromHandle(m_hParentWnd), (UINT)this);

	StartClient(sAddrCli, sAddrSvr, sPortSvr);
	Sleep(10);

	m_pThread = NULL;
	m_bTIM_CHECK_CONNECT = FALSE;
}


CEngrave::~CEngrave()
{
	m_bTIM_CHECK_CONNECT = FALSE;

	if (m_pClient)
	{
		m_bAutoConnect = FALSE;
		StopClient();
	}
}


///////////////////////////////////////////////////////////////////////////////
// OnUpdateConnection
// This message is sent by server manager to indicate connection status
LRESULT CEngrave::OnUpdateConnection(WPARAM wParam, LPARAM lParam)
{
	UINT uEvent = (UINT)wParam;
	CEngrave* pClient = reinterpret_cast<CEngrave*>(lParam);

	if (pClient != NULL)
	{
		// Server socket is now connected, we need to pick a new one
		if (uEvent == EVT_CONSUCCESS)
		{
			//pGlobalDoc->m_bConnect = TRUE;
			m_bWaitForResponse = FALSE;
			m_bTIM_CHECK_CONNECT = TRUE;
			SetTimer(TIM_CHECK_CONNECT, DELAY_CHECK_CONNECT, NULL);
		}
		else if (uEvent == EVT_CONFAILURE || uEvent == EVT_CONDROP)
		{
			// Disconnect socket
			StopClient();
			// 자동접속시도
			if (uEvent == EVT_CONDROP && m_bAutoConnect)
				SetTimer(TIM_CONNECT, 9000, NULL);
		}
	}

	return 1L;
}


void CEngrave::SetHwnd(HWND hParentWnd)
{
	m_hParentWnd = hParentWnd;
}

void CEngrave::StartClient(CString sAddrCli, CString sAddrSvr, CString sPortSvr)
{
	if (!m_pClient)
	{
		m_strAddrCli = sAddrCli;
		m_strAddrSvr = sAddrSvr;
		m_strPortSvr = sPortSvr;

		m_pClient = new CTcpIpClient(this);
		m_pClient->Init(sAddrCli, sAddrSvr, _tstoi(sPortSvr));
		m_pClient->SetServer(m_nServerID);
		m_pClient->Start();
	}
}

void CEngrave::StopClient()
{
	if (m_pClient)
	{
		//if (!m_pClient->Stop()) // Called Destroy Function.
		//{
		//	Sleep(30);
		//	delete m_pClient;
		//}
		m_pClient->Stop();
		Sleep(10);
		m_pClient->StopThread();
		Sleep(10);
		delete m_pClient;
		m_pClient = NULL;
		Sleep(10);
	}
}

LRESULT CEngrave::wmClientClosed(WPARAM wParam, LPARAM lParam)
{
	int nServerID = (int)wParam;

	switch (nServerID)
	{
	case ID_ENGRAVE:
		if (m_pClient)
			m_pClient = NULL;
		break;
	}

	return (LRESULT)1;
}


SOCKET_DATA CEngrave::GetSocketData()
{
	return m_SocketData;
}

LRESULT CEngrave::wmClientReceived(WPARAM wParam, LPARAM lParam)
{
	int nServerID = (int)wParam;

	if (nServerID == ID_ENGRAVE)
		m_SocketData = m_pClient->GetSocketData();

	return (LRESULT)OnClientReceived(wParam, lParam);
}

int CEngrave::OnClientReceived(WPARAM wParam, LPARAM lParam)
{
	int nServerID = (int)wParam;
	//CString sReceived = (LPCTSTR)lParam;

	//if (sReceived == _T("Fail to connect."))
	//	return 1;

	if (m_hParentWnd)
		::PostMessage(m_hParentWnd, WM_CLIENT_RECEIVED, (WPARAM)ID_ENGRAVE, (LPARAM)&m_SocketData); // "OrderNum-ShotNum" (9bytes'-'3bytes)
	m_bWaitForResponse = FALSE;
/*
	switch (nServerID)
	{
	case ID_ENGRAVE:
		if (m_hParentWnd)
			::PostMessage(m_hParentWnd, WM_CLIENT_RECEIVED, (WPARAM)ID_ENGRAVE, (LPARAM)&m_SocketData); // "OrderNum-ShotNum" (9bytes'-'3bytes)
		m_bWaitForResponse = FALSE;
		//if (sReceived.GetLength() > 0)
		//{
		//	if (CheckResponse(m_nCmd, sReceived))
		//	{
		//		;
		//	}
		//	else
		//	{
		//		pView->MsgBox(_T("Error-CheckResponse"));
		//		//AfxMessageBox(_T("Error-CheckResponse"));
		//	}
		//}
		break;
	}
*/
	return 1;
}

BOOL CEngrave::CheckResponse(int nCmd, CString sResponse)
{
	int nPos;
	CString sParsing;

	switch (nCmd)
	{
	//case SrTriggerInputOn:
	//	if (nPos = sResponse.Find(_T("ERROR")) > 0)
	//	{
	//		pView->MsgBox(_T("Error-Mdx response"));
	//		//AfxMessageBox(_T("Error-Mdx response"));
	//		m_strResponse = sResponse;
	//		m_bWaitForResponse = FALSE;
	//	}
	//	else
	//	{
	//		m_strResponse = sResponse;
	//		m_bWaitForResponse = FALSE;
	//		if (m_hParentWnd)
	//			::PostMessage(m_hParentWnd, WM_CLIENT_RECEIVED_SR, (WPARAM)SrTriggerInputOn, (LPARAM)&m_strResponse); // "OrderNum-ShotNum" (9bytes'-'3bytes)
	//	}
	//	return TRUE;
		;
	}

	return FALSE;
}


BOOL CEngrave::IsRunning()
{
	return (m_bWaitForResponse);
}

BOOL CEngrave::ReadComm(CString &sData)
{
	if (!m_bWaitForResponse)
	{
		sData = m_strResponse;
		return TRUE;
	}

	return FALSE;
}

int CEngrave::WriteComm(CString sMsg, DWORD dwTimeout)
{
	return m_pClient->WriteComm(sMsg);
}


// Thread
void CEngrave::StartThread() // Worker Thread 구동관련 Step8
{
	if (m_pThread == NULL)
	{
		m_bModify = TRUE;
		m_evtThread.ResetEvent();
		m_pThread = AfxBeginThread(RunThread, this);
		if (m_pThread)
			m_hThread = m_pThread->m_hThread;
	}
}

void CEngrave::StopThread() // Worker Thread 구동관련 Step9
{
	if (m_pThread != NULL)
	{
		m_evtThread.SetEvent();
		WaitUntilThreadEnd(m_hThread);
	}
	m_pThread = NULL;
	m_bModify = FALSE;
}

void CEngrave::WaitUntilThreadEnd(HANDLE hThread) // Worker Thread 구동관련 Step6
{
	TRACE("WaitUntilThreadEnd(0x%08x:RunThread) Entering\n", hThread);
	MSG message;
	const DWORD dwTimeOut = 500000;
	DWORD dwStartTick = GetTickCount();
	DWORD dwExitCode;
	while (GetExitCodeThread(hThread, &dwExitCode) && dwExitCode == STILL_ACTIVE && m_bAlive) {
		// Time Out Check
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			//pView->MsgBox("WaitUntilThreadEnd() Time Out!!!");
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
	}
	TRACE("WaitUntilThreadEnd(0x%08x:DispMsgThread) Exit\n", hThread);
}

UINT CEngrave::RunThread(LPVOID pParam)
{
	CEngrave* pMk = (CEngrave*)pParam;
	DWORD dwMilliseconds = 10;
	pMk->m_bAlive = TRUE;

	MSG message;
	CString sMsg;

	while (WAIT_OBJECT_0 != ::WaitForSingleObject(pMk->m_evtThread, dwMilliseconds))
	{
		Sleep(100);
	}

	pMk->m_bAlive = FALSE;
	return 0;
}

void CEngrave::Close()
{
	StopThread();
	Sleep(10);
}


// General Function

BOOL CEngrave::SendCommand(SOCKET_DATA SocketData, BOOL bWait)
{
	BOOL bRtn = TRUE;
	SocketData.nTxPC = _Punch;	// Client
	SocketData.nRxPC = _Engrave; // Server
	m_pClient->WriteCommData(SocketData, INFINITE);

	if (bWait)
	{
		m_bWaitForResponse = TRUE;
		bRtn = WaitResponse();
	}

	return bRtn;
}

BOOL CEngrave::WaitResponse()
{
	MSG message;
	DWORD dwStartTick = GetTickCount();

	while (IsRunning())
	{
		if (GetTickCount() - dwStartTick > DELAY_RESPONSE)
		{
			// Disconnect socket
			//AfxMessageBox(_T("WaitResponse() Time Out!!!"));
			m_pClient->SetConnectedStatus(FALSE);
			return FALSE;
		}

		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}

	return TRUE;
}

BOOL CEngrave::IsConnected()
{
	if (!m_pClient)
		return FALSE;

	return m_pClient->IsConnected();
}

void CEngrave::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	SOCKET_DATA SocketData;

	switch (nIDEvent)
	{
	case TIM_CONNECT:
		KillTimer(TIM_CONNECT);
		SetTimer(TIM_CONNECT + 1, 1000, NULL);
		break;
	case TIM_CONNECT + 1:
		KillTimer(TIM_CONNECT + 1);
		StartClient(m_strAddrCli, m_strAddrSvr, m_strPortSvr);
		SetTimer(TIM_CONNECT + 2, 1000, NULL);
		break;
	case TIM_CONNECT + 2:
		KillTimer(TIM_CONNECT + 2);
		if(!IsConnected())
			SetTimer(TIM_CONNECT + 1, 1000, NULL);
		break;
	case TIM_CHECK_CONNECT:
		KillTimer(TIM_CHECK_CONNECT);
		if (m_pClient && m_bTIM_CHECK_CONNECT)
		{
			if(!IsRunning())
			{
				SocketData.nCmdCode = _GetSig;
				SocketData.nMsgID = _Connect;
				SendCommand(SocketData);
			}
			if (m_bTIM_CHECK_CONNECT)
				SetTimer(TIM_CHECK_CONNECT, DELAY_CHECK_CONNECT, NULL);
		}
		break;
	}
	CWnd::OnTimer(nIDEvent);
}

BOOL CEngrave::SetSysInfo()
{
	if (!IsRunning())
	{
		SetInfo();
		SetTotRatio();
		SetStTime();
		SetRunTime();
		SetEdTime();
		SetStripRatio();
		SetDef();
		Set2DReader();
		SetEngInfo();
		SetFdInfo();
		SetAoiInfo();
		SetMkInfo();
		SetMkInfoLf();
		SetMkInfoRt();

		return TRUE;
	}

	return FALSE;
}

void CEngrave::SetInfo()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString sVal;
	char cData[BUFFER_DATA_SIZE];

	SocketData.nMsgID = _OpName;
	StringToChar(pDoc->WorkingInfo.LastJob.sSelUserName, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _ModelUpName;
	StringToChar(pDoc->WorkingInfo.LastJob.sModelUp, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _ModelDnName;
	StringToChar(pDoc->WorkingInfo.LastJob.sModelDn, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _LotUpName;
	StringToChar(pDoc->WorkingInfo.LastJob.sLotUp, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _LotDnName;
	StringToChar(pDoc->WorkingInfo.LastJob.sLotDn, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _LayerUpName;
	StringToChar(pDoc->WorkingInfo.LastJob.sLayerUp, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _LayerDnName;
	StringToChar(pDoc->WorkingInfo.LastJob.sLayerDn, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	// _OrderNum
	//_ShotNum

	SocketData.nMsgID = _TotReelLen;
	StringToChar(pDoc->WorkingInfo.LastJob.sReelTotLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	// _TotOpRto
	// _LotOpRto
	// _TotVel

	SocketData.nMsgID = _PartVel;
	StringToChar(pDoc->WorkingInfo.LastJob.sPartialSpd, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _TempStopLen;
	StringToChar(pDoc->WorkingInfo.LastJob.sTempPauseLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _LotCutLen;
	StringToChar(pDoc->WorkingInfo.LastJob.sLotCutPosLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	// _MkDoneLen
	// _AoiDnDoneLen
	// _AoiUpDoneLen

	SocketData.nMsgID = _LotSerial;
	StringToChar(pDoc->WorkingInfo.LastJob.sLotSerial, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkVerfyLen;
	StringToChar(pDoc->WorkingInfo.LastJob.sVerifyLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetTotRatio()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str;
	int nGood = 0, nBad = 0, nTot = 0, nVal;
	double dRatio = 0.0;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;
	// < 전체 수율 >

	// 상면
	pDoc->m_pReelMapUp->GetPcsNum(nGood, nBad);
	nTot = nGood + nBad;

	SocketData.nMsgID = _DefNumUp;
	SocketData.nData1 = nBad;
	SendCommand(SocketData);

	if (nTot > 0)
		dRatio = ((double)nBad / (double)nTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _DefRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	SocketData.nMsgID = _GoodNumUp;
	SocketData.nData1 = nGood;
	SendCommand(SocketData);

	if (nTot > 0)
		dRatio = ((double)nGood / (double)nTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _GoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	SocketData.nMsgID = _TestNumUp;
	SocketData.nData1 = nTot;
	SendCommand(SocketData);

	if (bDualTest)
	{
		// 하면
		pDoc->m_pReelMapDn->GetPcsNum(nGood, nBad);
		nTot = nGood + nBad;

		SocketData.nMsgID = _DefNumDn;
		SocketData.nData1 = nBad;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)nBad / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _DefRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		SocketData.nMsgID = _GoodNumDn;
		SocketData.nData1 = nGood;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _GoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		SocketData.nMsgID = _TestNumDn;
		SocketData.nData1 = nTot;
		SendCommand(SocketData);

		// 전체
		pDoc->m_pReelMapAllDn->GetPcsNum(nGood, nBad);
		nTot = nGood + nBad;

		SocketData.nMsgID = _DefNumTot;
		SocketData.nData1 = nTot;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)nBad / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _DefRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		SocketData.nMsgID = _GoodNumTot;
		SocketData.nData1 = nGood;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)nGood / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _GoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		SocketData.nMsgID = _TestNumTot;
		SocketData.nData1 = nTot;
		SendCommand(SocketData);
	}
}

void CEngrave::SetStTime()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str, sPrev;
	int nYear, nMonth, nDay, nHour, nMin, nSec;

	nYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	if (!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		str = _T("");
	else
		str.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);

	char cData[BUFFER_DATA_SIZE];
	SocketData.nMsgID = _LotStTime;
	StringToChar(str, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetRunTime()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str, sPrev;
	int nDiff;
	int nHour, nMin, nSec;
	int nStYear, nStMonth, nStDay, nStHour, nStMin, nStSec;
	int nCurYear, nCurMonth, nCurDay, nCurHour, nCurMin, nCurSec;
	int nEdYear, nEdMonth, nEdDay, nEdHour, nEdMin, nEdSec;

	nStYear = pDoc->WorkingInfo.Lot.StTime.nYear;
	nStMonth = pDoc->WorkingInfo.Lot.StTime.nMonth;
	nStDay = pDoc->WorkingInfo.Lot.StTime.nDay;
	nStHour = pDoc->WorkingInfo.Lot.StTime.nHour;
	nStMin = pDoc->WorkingInfo.Lot.StTime.nMin;
	nStSec = pDoc->WorkingInfo.Lot.StTime.nSec;

	nCurYear = pDoc->WorkingInfo.Lot.CurTime.nYear;
	nCurMonth = pDoc->WorkingInfo.Lot.CurTime.nMonth;
	nCurDay = pDoc->WorkingInfo.Lot.CurTime.nDay;
	nCurHour = pDoc->WorkingInfo.Lot.CurTime.nHour;
	nCurMin = pDoc->WorkingInfo.Lot.CurTime.nMin;
	nCurSec = pDoc->WorkingInfo.Lot.CurTime.nSec;

	nEdYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nEdMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nEdDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nEdHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nEdMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nEdSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	if (!nStYear && !nStMonth && !nStDay && !nStHour && !nStMin && !nStSec)
	{
		str = _T("");
	}
	else if (!nEdYear && !nEdMonth && !nEdDay && !nEdHour && !nEdMin && !nEdSec)
	{
		nDiff = (GetTickCount() - pView->m_dwLotSt) / 1000;
		nHour = int(nDiff / 3600);
		nMin = int((nDiff - 3600 * nHour) / 60);
		nSec = nDiff % 60;
		str.Format(_T("%02d:%02d:%02d"), nHour, nMin, nSec);
	}
	else
	{
		if (pView->m_dwLotEd > 0)
		{
			nDiff = (pView->m_dwLotEd - pView->m_dwLotSt) / 1000;
			nHour = int(nDiff / 3600);
			nMin = int((nDiff - 3600 * nHour) / 60);
			nSec = nDiff % 60;
			str.Format(_T("%02d:%02d:%02d"), nHour, nMin, nSec);
		}
		else
		{
			nHour = nEdHour - nStHour;
			if (nHour < 0)
				nHour += 24;

			nMin = nEdMin - nStMin;
			if (nMin < 0)
				nMin += 60;

			nSec = nEdSec - nStSec;
			if (nSec < 0)
				nSec += 60;

			str.Format(_T("%02d:%02d:%02d"), nHour, nMin, nSec);
		}
	}

	char cData[BUFFER_DATA_SIZE];
	SocketData.nMsgID = _LotRunTime; 
	StringToChar(str, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetEdTime()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str, sPrev;
	int nYear, nMonth, nDay, nHour, nMin, nSec;

	nYear = pDoc->WorkingInfo.Lot.EdTime.nYear;
	nMonth = pDoc->WorkingInfo.Lot.EdTime.nMonth;
	nDay = pDoc->WorkingInfo.Lot.EdTime.nDay;
	nHour = pDoc->WorkingInfo.Lot.EdTime.nHour;
	nMin = pDoc->WorkingInfo.Lot.EdTime.nMin;
	nSec = pDoc->WorkingInfo.Lot.EdTime.nSec;

	if (!nYear && !nMonth && !nDay && !nHour && !nMin && !nSec)
		str = _T("");
	else
		str.Format(_T("%04d-%02d-%02d, %02d:%02d:%02d"), nYear, nMonth, nDay, nHour, nMin, nSec);

	char cData[BUFFER_DATA_SIZE];
	SocketData.nMsgID = _LotEdTime; 
	StringToChar(str, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetStripRatio()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str;
	int nGood = 0, nBad = 0, nTot = 0, nStTot = 0, nSum = 0, nVal[2][4];
	int nMer[4];
	double dRatio = 0.0;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	for (int i = 0; i < 2; i++)
	{
		for (int k = 0; k < 4; k++)
			nVal[i][k] = 0;
	}

	// < 스트립 별 수율 >
	pDoc->m_pReelMapUp->GetPcsNum(nGood, nBad);
	nTot = nGood + nBad;
	nStTot = nTot / 4;

	// 상면
	nSum = 0;
	nVal[0][0] = pDoc->m_pReelMapUp->GetDefStrip(0);
	nSum += nVal[0][0];
	if (nTot > 0)
		dRatio = ((double)(nStTot - nVal[0][0]) / (double)nStTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _1LnGoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	nVal[0][1] = pDoc->m_pReelMapUp->GetDefStrip(1);
	nSum += nVal[0][1];
	if (nTot > 0)
		dRatio = ((double)(nStTot - nVal[0][1]) / (double)nStTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _2LnGoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	nVal[0][2] = pDoc->m_pReelMapUp->GetDefStrip(2);
	nSum += nVal[0][2];
	if (nTot > 0)
		dRatio = ((double)(nStTot - nVal[0][2]) / (double)nStTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _3LnGoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	nVal[0][3] = pDoc->m_pReelMapUp->GetDefStrip(3);
	nSum += nVal[0][3];
	if (nTot > 0)
		dRatio = ((double)(nStTot - nVal[0][3]) / (double)nStTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _4LnGoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);

	if (nTot > 0)
		dRatio = ((double)(nTot - nSum) / (double)nTot) * 100.0;
	else
		dRatio = 0.0;

	SocketData.nMsgID = _AllLnGoodRtoUp;
	SocketData.fData1 = dRatio;
	SendCommand(SocketData);


	if (bDualTest)
	{
		// 하면
		nSum = 0;
		nVal[1][0] = pDoc->m_pReelMapDn->GetDefStrip(0);
		nSum += nVal[1][0];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nVal[1][0]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _1LnGoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nVal[1][1] = pDoc->m_pReelMapDn->GetDefStrip(1);
		nSum += nVal[1][1];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nVal[1][1]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _2LnGoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nVal[1][2] = pDoc->m_pReelMapDn->GetDefStrip(2);
		nSum += nVal[1][2];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nVal[1][2]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _3LnGoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nVal[1][3] = pDoc->m_pReelMapDn->GetDefStrip(3);
		nSum += nVal[1][3];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nVal[1][3]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _4LnGoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)(nTot - nSum) / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _AllLnGoodRtoDn;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		// 상면 + 하면
		nSum = 0;
		nMer[0] = pDoc->m_pReelMapAllUp->GetDefStrip(0);
		nSum += nMer[0];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nMer[0]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _1LnGoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nMer[1] = pDoc->m_pReelMapAllUp->GetDefStrip(1);
		nSum += nMer[1];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nMer[1]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _2LnGoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nMer[2] = pDoc->m_pReelMapAllUp->GetDefStrip(2);
		nSum += nMer[2];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nMer[2]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _3LnGoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		nMer[3] = pDoc->m_pReelMapAllUp->GetDefStrip(3);
		nSum += nMer[3];
		if (nTot > 0)
			dRatio = ((double)(nStTot - nMer[3]) / (double)nStTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _4LnGoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);

		if (nTot > 0)
			dRatio = ((double)(nTot - nSum) / (double)nTot) * 100.0;
		else
			dRatio = 0.0;

		SocketData.nMsgID = _AllLnGoodRtoTot;
		SocketData.fData1 = dRatio;
		SendCommand(SocketData);
	}
}

void CEngrave::SetDef()
{
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	CString str;
	int nNum = 0;
	BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;

	CReelMap* pReelMap;

	if (bDualTest)
	{
		switch (pView->m_nSelRmap)
		{
		case RMAP_UP:
			pReelMap = pDoc->m_pReelMapUp;
			break;
		case RMAP_DN:
			pReelMap = pDoc->m_pReelMapDn;
			break;
		case RMAP_ALLUP:
			pReelMap = pDoc->m_pReelMapAllUp;
			break;
		case RMAP_ALLDN:
			pReelMap = pDoc->m_pReelMapAllDn;
			break;
		}
	}
	else
		pReelMap = pDoc->m_pReelMapUp;

	nNum = pReelMap->GetDefNum(DEF_OPEN); // IDC_STC_DEF_OPEN
	SocketData.nMsgID = _DefNumOpen;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_SHORT); // IDC_STC_DEF_SHORT
	SocketData.nMsgID = _DefNumShort;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_USHORT); // IDC_STC_DEF_U_SHORT
	SocketData.nMsgID = _DefNumUshort;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_SPACE); // IDC_STC_DEF_SPACE
	SocketData.nMsgID = _DefNumLnW;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_EXTRA); // IDC_STC_DEF_EXTRA
	SocketData.nMsgID = _DefExtr;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_PROTRUSION); // IDC_STC_DEF_PROT
	SocketData.nMsgID = _DefNumProt;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_PINHOLE); // IDC_STC_DEF_P_HOLE
	SocketData.nMsgID = _DefNumPhole;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_PAD); // IDC_STC_DEF_PAD
	SocketData.nMsgID = _DefNumPad;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_HOLE_OPEN); // IDC_STC_DEF_H_OPEN
	SocketData.nMsgID = _DefNumHopen;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_HOLE_MISS); // IDC_STC_DEF_H_MISS
	SocketData.nMsgID = _DefNumHmiss;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_HOLE_POSITION); // IDC_STC_DEF_H_POS
	SocketData.nMsgID = _DefNumHpos;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_HOLE_DEFECT); // IDC_STC_DEF_H_DEF
	SocketData.nMsgID = _DefNumHdef;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_NICK); // IDC_STC_DEF_NICK
	SocketData.nMsgID = _DefNumNick;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_POI); // IDC_STC_DEF_POI
	SocketData.nMsgID = _DefNumPoi;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_VH_OPEN); // IDC_STC_DEF_VH_OPEN
	SocketData.nMsgID = _DefNumVhOpen;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_VH_MISS); // IDC_STC_DEF_VH_MISS
	SocketData.nMsgID = _DefNumVhMiss;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_VH_POSITION); // IDC_STC_DEF_VH_POS
	SocketData.nMsgID = _DefNumVhPos;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_VH_DEF); // IDC_STC_DEF_VH_DEF
	SocketData.nMsgID = _DefNumVhd;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_LIGHT); // IDC_STC_DEF_LIGHT
	SocketData.nMsgID = _DefNumLight;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_EDGE_NICK);
	SocketData.nMsgID = _DefNumEnick;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_EDGE_PROT);
	SocketData.nMsgID = _DefNumEprot;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_EDGE_SPACE);
	SocketData.nMsgID = _DefNumEspace;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_USER_DEFINE_1);
	SocketData.nMsgID = _DefNumUdd1;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_NARROW);
	SocketData.nMsgID = _DefNumNrw;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);

	nNum = pReelMap->GetDefNum(DEF_WIDE);
	SocketData.nMsgID = _DefNumWide;
	SocketData.nData1 = nNum;
	SendCommand(SocketData);


	//nNum = pReelMap->GetDefNum(DEF_SHORT) + pReelMap->GetDefNum(DEF_USHORT); // IDC_STC_DEF_SHORT_TOT
	//str.Format(_T("%d"), nNum);
	//myStcData[76].SetText(str);

	//nNum = pReelMap->GetDefNum(DEF_SPACE) + pReelMap->GetDefNum(DEF_EXTRA) + pReelMap->GetDefNum(DEF_PROTRUSION); // IDC_STC_DEF_SPACE_TOT
	//str.Format(_T("%d"), nNum);
	//myStcData[77].SetText(str);

	//nNum = pReelMap->GetDefNum(DEF_PINHOLE) + pReelMap->GetDefNum(DEF_PAD); // IDC_STC_DEF_P_HOLE_TOT
	//str.Format(_T("%d"), nNum);
	//myStcData[78].SetText(str);

	//nNum = pReelMap->GetDefNum(DEF_HOLE_MISS) + pReelMap->GetDefNum(DEF_HOLE_POSITION) + pReelMap->GetDefNum(DEF_HOLE_DEFECT); // IDC_STC_DEF_H_MISS_TOT
	//str.Format(_T("%d"), nNum);
	//myStcData[79].SetText(str);

	//nNum = pReelMap->GetDefNum(DEF_VH_MISS) + pReelMap->GetDefNum(DEF_VH_POSITION) + pReelMap->GetDefNum(DEF_VH_DEF); // IDC_STC_DEF_VH_MISS_TOT
	//str.Format(_T("%d"), nNum);
	//myStcData[80].SetText(str);
	
}

void CEngrave::Set2DReader()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;
	//CString str;
	//int nGood = 0, nBad = 0, nTot = 0, nStTot = 0, nSum = 0, nVal[2][4];
	//int nMer[4];
	//double dRatio = 0.0;
	//BOOL bDualTest = pDoc->WorkingInfo.LastJob.bDualTest;


	SocketData.nMsgID = _2DEngLen; 
	StringToChar(pDoc->WorkingInfo.Motion.s2DEngLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _2DAoiLen; StringToChar(pDoc->WorkingInfo.Motion.s2DAoiLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _2DMkLen; StringToChar(pDoc->WorkingInfo.Motion.s2DMkLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _2DMoveVel; StringToChar(pDoc->WorkingInfo.Motion.s2DMoveVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _2DMoveAcc; StringToChar(pDoc->WorkingInfo.Motion.s2DMoveAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _2DOneShotLen; StringToChar(pDoc->WorkingInfo.Motion.s2DOneShotRemainLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetEngInfo()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;

	SocketData.nMsgID = _EngLeadPitch; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngraveFdLead, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngPushOffLen; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngraveFdVacOff, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngTqVal; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngraveTq, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngAoiLen; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngAoiLen, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngFdDiffMax; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngFdDiffMax, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngFdDiffRng; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngFdDiffRng, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngFdDiffNum; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngFdDiffNum, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngBuffInitPos; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngBuffInitPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _EngBuffCurrPos; 
	StringToChar(pDoc->WorkingInfo.Motion.sEngBuffCurrPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetFdInfo()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;


	SocketData.nMsgID = _FdVel; 
	StringToChar(pDoc->WorkingInfo.Motion.sMkFdVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _FdAcc; 
	StringToChar(pDoc->WorkingInfo.Motion.sMkFdAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _OnePnlLen;
	sprintf(SocketData.strData, "%.3f", pDoc->GetOnePnlLen());
	SendCommand(SocketData);

	SocketData.nMsgID = _OnePnlVal;
	sprintf(SocketData.strData, "%.3f", pDoc->GetOnePnlVel());
	SendCommand(SocketData);

	SocketData.nMsgID = _OnePnlAcc;
	sprintf(SocketData.strData, "%.3f", pDoc->GetOnePnlAcc());
	SendCommand(SocketData);

	SocketData.nMsgID = _FdDiffMax;
	sprintf(SocketData.strData, "%.3f", pDoc->GetFdErrLmt());
	SendCommand(SocketData);

	SocketData.nMsgID = _FdDiffRng;
	sprintf(SocketData.strData, "%.3f", pDoc->GetFdErrRng());
	SendCommand(SocketData);

	SocketData.nMsgID = _FdDiffNum;
	sprintf(SocketData.strData, "%.3f", pDoc->GetFdErrNum());
	SendCommand(SocketData);
}

void CEngrave::SetAoiInfo()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;

	SocketData.nMsgID = _AoiLeadPitch; StringToChar(pDoc->WorkingInfo.Motion.sAoiFdLead, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _AoiPushOffLen; StringToChar(pDoc->WorkingInfo.Motion.sAoiFdVacOff, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _AoiTqVal; StringToChar(pDoc->WorkingInfo.Motion.sAoiTq, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _AoiBuffShotNum; StringToChar(pDoc->WorkingInfo.Motion.sFdAoiAoiDistShot, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _AoiMkLen; StringToChar(pDoc->WorkingInfo.Motion.sFdMkAoiInitDist, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);
}

void CEngrave::SetMkInfo()
{
	char cData[BUFFER_DATA_SIZE];
	CString str;
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;


	SocketData.nMsgID = _MkLeadPitch; 
	StringToChar(pDoc->WorkingInfo.Motion.sMkFdLead, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkPushOffLen; 
	StringToChar(pDoc->WorkingInfo.Motion.sMkFdVacOff, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkPushOffLen; 
	StringToChar(pDoc->WorkingInfo.Motion.sMkTq, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkBuffInitPos; 
	StringToChar(pDoc->WorkingInfo.Motion.sStBufPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkBuffCurrPos;
	double dBufEnc = (double)pDoc->m_pMpeData[0][1] / 1000.0;	// 마킹부 버퍼 엔코더 값(단위 mm * 1000)
	sprintf(SocketData.strData, "%.1f", dBufEnc);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkNumLf;
	sprintf(SocketData.strData, "%d", pDoc->GetMkCntL());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkNumRt;
	sprintf(SocketData.strData, "%d", pDoc->GetMkCntR());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkMaxNumLf;
	sprintf(SocketData.strData, "%d", pDoc->GetMkLimitL());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkMaxNumRt;
	sprintf(SocketData.strData, "%d", pDoc->GetMkLimitR());
	SendCommand(SocketData);
}

void CEngrave::SetMkInfoLf()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;

	SocketData.nMsgID = _MkInitPosLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sWaitPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkInitVelLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sWaitVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkInitAccLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sWaitAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlPosLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sMarkingPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlVelLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sMarkingVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlAccLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sMarkingAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlTqLf; 
	StringToChar(pDoc->WorkingInfo.Marking[0].sMarkingToq, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX1Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX1_1());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY1Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY1_1());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX2Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX1_2());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY2Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY1_2());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX3Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX1_3());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY3Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY1_3());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX4Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX1_4());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY4Lf;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY1_4());
	SendCommand(SocketData);
}

void CEngrave::SetMkInfoRt()
{
	char cData[BUFFER_DATA_SIZE];
	SOCKET_DATA SocketData;
	SocketData.nCmdCode = _SetData;

	SocketData.nMsgID = _MkInitPosRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sWaitPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkInitVelRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sWaitVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkInitAccRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sWaitAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlPosRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sMarkingPos, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlVelRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sMarkingVel, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlAccRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sMarkingAcc, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkFnlTqRt; 
	StringToChar(pDoc->WorkingInfo.Marking[1].sMarkingToq, cData);
	sprintf(SocketData.strData, "%s", cData);
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX1Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX2_1());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY1Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY2_1());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX2Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX2_2());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY2Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY2_2());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX3Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX2_3());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY3Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY2_3());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosX4Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosX2_4());
	SendCommand(SocketData);

	SocketData.nMsgID = _MkHgtPosY4Rt;
	sprintf(SocketData.strData, "%.3f", pDoc->GetPosY2_4());
	SendCommand(SocketData);
}
