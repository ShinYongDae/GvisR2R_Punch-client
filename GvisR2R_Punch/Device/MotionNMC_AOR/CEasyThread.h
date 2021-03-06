#pragma once


class CEasyThread : public CWinThread
{
	DECLARE_DYNCREATE(CEasyThread)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void KillThread();

	void DoLoop();

	virtual int Running();

	BOOL m_bStarted;
	BOOL PrepareThread();
	BOOL m_bStop;
	bool			m_bRun;
	bool			m_bEnd;

	CEasyThread();
	virtual ~CEasyThread();
	DECLARE_MESSAGE_MAP()
};