#include "stdafx.h"
#include "BrowserUI.h"

#include <sstream>
#include <string>

#include "BrowserDlg.h"
#include "BrowserDlgManager.h"
#include "MainContext.h"

namespace Browser {
BrowserUI::BrowserUI(HWND hParentWnd)
    : m_hParentWnd(hParentWnd)
{
    SetBkColor(0xFFFFFFFF);
}

LPCTSTR BrowserUI::GetClass() const
{
    return _T("BrowserUI");
}

LPVOID BrowserUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, _T("BrowserUI")) == 0) return static_cast<BrowserUI*>(this);

    return CControlUI::GetInterface(pstrName);
}

void BrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    DuiLib::CDuiRect rcPos = rc;
    if (m_pCEFHandler && !rcPos.IsNull())
    {
        m_pCEFHandler->ShowBrowser(m_nCurBrowserId, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    }
    CControlUI::SetPos(rc, bNeedInvalidate);
}

void BrowserUI::SetCtrl(BrowserWindow* pCtrl)
{
    if (pCtrl)
    {
        m_pCEFHandler = pCtrl;
    }
}

void BrowserUI::CreateBrowser(const CefString& url, CefRefPtr<CefRequestContext> request_context)
{
    if (m_pCEFHandler)
    {
        CefBrowserSettings settings;
        RECT rcPos = GetPos();
        CefRect cef_rect(rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
        m_pCEFHandler->CreateBrowser(m_hParentWnd, url, cef_rect, settings, request_context);
    }
}

void BrowserUI::ShowBrowser(int nBrowserId)
{
    m_nCurBrowserId = nBrowserId;
    if (m_pCEFHandler)
    {
        RECT rcPos = GetPos();
        m_pCEFHandler->ShowBrowser(nBrowserId, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                                   rcPos.bottom - rcPos.top);
    }
}

void BrowserUI::CloseBrowser(int nBrowserId)
{
    if (m_pCEFHandler)
    {
        auto pBrowser = m_pCEFHandler->GetBrowser(nBrowserId);
        if (pBrowser)
        {
            pBrowser->GetHost()->CloseBrowser(false);
        }
    }
}
} // namespace Browser
