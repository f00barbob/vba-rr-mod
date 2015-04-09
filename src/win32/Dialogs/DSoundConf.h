#if !defined(AFX_DSOUNDCONF_H__8ADB13C1_EC1B_4294_8D66_B4E87D6FC732__INCLUDED_)
#define AFX_DSOUNDCONF_H__8ADB13C1_EC1B_4294_8D66_B4E87D6FC732__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DSoundConf.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// DSoundConf dialog

class DSoundConf : public CDialog
{
	// Construction
public:
	CString initialFolderDir;
	CString browseForDir(CString title);
	
	DSoundConf(CWnd*pParent = NULL);  // standard constructor

	// Dialog Data
	//{{AFX_DATA(DSoundConf)
	enum { IDD = IDD_DSOUNDCONF };
	CListBox m_soundoutput;
	//  CEdit m_pluginPath;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(DSoundConf)
protected:
	virtual void DoDataExchange(CDataExchange*pDX);   // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(DSoundConf)
	virtual BOOL OnInitDialog();
	afx_msg void OnSoundOutput();
	afx_msg void OnSelchangeOutput();
	virtual void OnCancel();
	virtual void OnOK();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeSoundOutput();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRECTORIES_H__7ADB14C1_3C1B_4294_8D66_A4E87D6FC731__INCLUDED_)
