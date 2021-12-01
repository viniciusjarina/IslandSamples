
// MFCApplication1.h : main header file for the MFCApplication1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFCApplication1App:
// See MFCApplication1.cpp for the implementation of this class
//

class CMFCApplication1App : public CWinApp
{
public:
	CMFCApplication1App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication1App theApp;
