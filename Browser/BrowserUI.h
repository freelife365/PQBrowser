#ifndef __BROWSERUI_H__
#define __BROWSERUI_H__
#pragma once

#include "ClientHandler.h"
#include "DuiLib/UIlib.h"
using namespace DuiLib;

class BrowserDlg;

namespace Browser {
class BrowserWindow;
class BrowserUI : public DuiLib::CControlUI
{
    DECLARE_DUICONTROL(BrowserUI)
public:
    BrowserUI(HWND hParentWnd);

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void SetCtrl(BrowserWindow* pCtrl);
    void CreateBrowser(const CefString& url, CefRefPtr<CefRequestContext> request_context);
    void ShowBrowser(int nBrowserId = 0);
    void CloseBrowser(int nBrowserId = 0);

protected:
    HWND m_hParentWnd;
    BrowserWindow* m_pCEFHandler = nullptr;
    int m_nCurBrowserId = 0;

    DISALLOW_COPY_AND_ASSIGN(BrowserUI);
};
} // namespace Browser

#endif
