#ifndef __BROWSER_MANAGER_H__
#define __BROWSER_MANAGER_H__
#pragma once

#include <set>
#include "include/cef_command_line.h"
#include "BrowserDlg.h"
#include "TempWindow.h"

namespace Browser
{
// Used to create/manage RootWindow instances. The methods of this class can be
// called from any browser process thread unless otherwise indicated.
class BrowserDlgManager : public BrowserDlg::Delegate
{
public:
    // If |terminate_when_all_windows_closed| is true quit the main message loop
    // after all windows have closed.
    explicit BrowserDlgManager(bool terminate_when_all_windows_closed);

    CefRefPtr<BrowserDlg> CreateBrowserDlg(HWND hParent,
                                           bool with_controls,
                                           const CefRect& bounds,
                                           const CefString& url);

    CefRefPtr<BrowserDlg> CreateBrowserDlgAsPopup(bool with_controls,
                                                  const CefPopupFeatures& popupFeatures,
                                                  CefWindowInfo& windowInfo,
                                                  CefRefPtr<CefClient>& client,
                                                  CefBrowserSettings& settings);

    // Returns the BrowserDlg associated with the specified browser ID.
    // Must be called on the main thread.
    CefRefPtr<BrowserDlg> GetWindowForBrowser(int browser_id);

    // Close all existing windows.
    // If |force| is true onunload handlers will not be executed.
    void CloseAllWindows(bool force);

private:
    // Allow deletion via scoped_ptr only.
    friend struct std::default_delete<BrowserDlgManager>;

    ~BrowserDlgManager();

    void OnBrowserDlgCreated(CefRefPtr<BrowserDlg> pDlg);

    // BrowserDlg::Delegate methods.
    CefRefPtr<CefRequestContext> GetRequestContext() override;
    void OnBrowserDlgDestroyed(BrowserDlg* pDlg) override;

    const bool terminate_when_all_windows_closed_;
    bool request_context_per_browser_;
    bool request_context_shared_cache_;

    // Existing root windows. Only accessed on the main thread.
    typedef std::set<CefRefPtr<BrowserDlg>> BrowserDlgSet;
    BrowserDlgSet m_BrowserDlgSet;

    // Singleton window used as the temporary parent for popup browsers.
    TempWindow m_TempWindow;

    CefRefPtr<CefRequestContext> m_SharedRequestContext;

    DISALLOW_COPY_AND_ASSIGN(BrowserDlgManager);
};
} // namespace Browser

#endif
