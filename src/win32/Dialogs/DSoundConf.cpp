// DSoundConf.cpp : implementation file
//


#include "../stdafx.h"
#include <MMReg.h>
#include <dsound.h>
#include <shlobj.h>
#include <windowsx.h>



#include "../../common/system.h"
#include "../resource.h"
#include "DSoundConf.h"
#include "../Reg.h"
#include "../WinMiscUtil.h"
#include "../WinResUtil.h"

#pragma comment(lib, "dsound.lib")

#define MAX_OUTPUTS 32
/////////////////////////////////////////////////////////////////////////////
// DSoundConf dialog

static int gOutputCnt = 0;                      // Total number of drivers
static LPGUID *gpSelectedOutputGUID;

struct
{
	char     szDescription[128];
	char     szName[128];
	GUID *   GUID;
	_GUID     GUIDcopy;
	//HMONITOR hm;
} Outputs[MAX_OUTPUTS];

extern bool systemSoundCleanInit();


BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
{
	/*LPGUID lpTemp = NULL;

	if (lpGUID != NULL)  //  NULL only for "Primary Sound Driver".
	{
	if ((lpTemp = (LPGUID)malloc(sizeof(GUID))) == NULL)
	{
	return(TRUE);
	}
	memcpy(lpTemp, lpGUID, sizeof(GUID));
	}

	HWND hList = (HWND)lpContext;*/

	printf("lpContext = %s\n", (char *)lpContext);
	printf("Device description = %s\n", lpszDesc);
	printf("Driver name = %s\n", lpszDrvName);
	printf("\n");


	if (lpGUID)
	{
		Outputs[gOutputCnt].GUIDcopy = *lpGUID;
		Outputs[gOutputCnt].GUID = &Outputs[gOutputCnt].GUIDcopy;
	}
	else
	{
		Outputs[gOutputCnt].GUID = NULL;
	}

	Outputs[gOutputCnt].szDescription[127] = '\0';
	Outputs[gOutputCnt].szName[127] = '\0';

	strncpy(Outputs[gOutputCnt].szDescription, lpszDesc, 127);
	strncpy(Outputs[gOutputCnt].szName, lpszDrvName, 127);

	//Outputs[gOutputCnt].hm = hm;

	if (gOutputCnt < MAX_OUTPUTS)
		gOutputCnt++;
	else
		return 0;

	//hList.AddString("fwef");
	//ListBox_AddString((HWND)hList, lpszDesc);
	//ListBox_SetItemData(hList, ListBox_FindString(hList, 0, lpszDesc), lpTemp);

	return TRUE;
}



DSoundConf::DSoundConf(CWnd*pParent /*=NULL*/)
	: CDialog(DSoundConf::IDD, pParent)
{
	//{{AFX_DATA_INIT(DSoundConf)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void DSoundConf::DoDataExchange(CDataExchange*pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DSoundConf)
	DDX_Control(pDX, IDC_SOUND_OUTPUT, m_soundoutput);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DSoundConf, CDialog)
	//{{AFX_MSG_MAP(DSoundConf)
	ON_LBN_SELCHANGE(IDC_SOUND_OUTPUT, OnSelchangeOutput)
	ON_BN_CLICKED(ID_CANCEL, OnCancel)
	ON_BN_CLICKED(ID_OK, OnOK)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DSoundConf message handlers

BOOL DSoundConf::OnInitDialog()
{

	BOOL CALLBACK DSEnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext);

	// this is all you have to do. You don't even need to have a DirectSound device set up
	//
	// To demonstrate lpContext, I am sending over "Moo!". "Moo!" doesn't do anything in this case
	if ((DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, (VOID*)&m_soundoutput)) == DS_OK)
	{
		printf("Operation completed successfully.\n");
	}
	else
	{
		printf("Houston, we have a problem.\n");
	}

	// If ddraw.dll doesn't exist in the search path,
	// then DirectX probably isn't installed, so fail.
	//if (!h)
	//	return -1;

	//gDriverCnt = 0;

	// Note that you must know which version of the
	// function to retrieve (see the following text).
	// For this example, we use the ANSI version.


	CDialog::OnInitDialog();
	printf("Init'd dialog!\n");
	//GetDlgItem(IDC_SOUND_OUTPUT)Items.Add("Foo");
	//m_soundoutput.AddString("FLUB");

	for (int i = 0; i < gOutputCnt; i++)
	{
		OLECHAR szGUID[128];
		
		m_soundoutput.AddString(Outputs[i].szDescription);
		printf("szDescription: %s\n", Outputs[i].szDescription);
		printf("       szName: %s\n", Outputs[i].szName);
		//printf("         GUID: %s\n", Outputs[i].GUID);
		StringFromGUID2(Outputs[i].GUIDcopy, szGUID, 40);
		printf("     GUIDcopy: %s\n", szGUID);
	}

	m_soundoutput.SetCurSel(regQueryDwordValue("SoundOutput", -1));

	CenterWindow();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void DSoundConf::OnSelchangeOutput()
{
	int item = m_soundoutput.GetCurSel();

	GetDlgItem(ID_OK)->EnableWindow(item != -1);
}


void DSoundConf::OnCancel()
{
	EndDialog(FALSE);
}

void DSoundConf::OnOK()
{
	CString buffer;
	if (m_soundoutput.GetCurSel() != LB_ERR)
	{
		regSetDwordValue("SoundOutput", m_soundoutput.GetCurSel());
	}
	else
		regDeleteValue("SoundOutput");

	memset(Outputs, 0, sizeof(Outputs));
	gOutputCnt = 0;
	systemSoundCleanInit();
	EndDialog(TRUE);
}


