#include "stdafx.h"
#include "ClientAppRenderer.h"
#include "include/base/cef_logging.h"
#include "PerfTest.h"

namespace Browser {
ClientAppRenderer::ClientAppRenderer()
{
    CreateDelegates(m_delegates);
}

//////////////////////////////////////////////////////////////////////////////////////////
// CefRenderProcessHandler methods.
void ClientAppRenderer::OnWebKitInitialized()
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnWebKitInitialized(this);
}

void ClientAppRenderer::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extra_info)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnBrowserCreated(this, browser, extra_info);
}

void ClientAppRenderer::OnBrowserDestroyed(CefRefPtr<CefBrowser> browser)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnBrowserDestroyed(this, browser);
}

CefRefPtr<CefLoadHandler> ClientAppRenderer::GetLoadHandler()
{
    CefRefPtr<CefLoadHandler> load_handler;
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end() && !load_handler.get(); ++it) load_handler = (*it)->GetLoadHandler(this);

    return load_handler;
}

void ClientAppRenderer::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                         CefRefPtr<CefV8Context> context)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnContextCreated(this, browser, frame, context);
}

void ClientAppRenderer::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                          CefRefPtr<CefV8Context> context)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnContextReleased(this, browser, frame, context);
}

void ClientAppRenderer::OnUncaughtException(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                            CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Exception> exception,
                                            CefRefPtr<CefV8StackTrace> stackTrace)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it)
    {
        (*it)->OnUncaughtException(this, browser, frame, context, exception, stackTrace);
    }
}

void ClientAppRenderer::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                             CefRefPtr<CefDOMNode> node)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it) (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool ClientAppRenderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                                 CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    DCHECK_EQ(source_process, PID_BROWSER);

    bool handled = false;

    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end() && !handled; ++it)
    {
        handled = (*it)->OnProcessMessageReceived(this, browser, frame, source_process, message);
    }

    return handled;
}

// static
void ClientAppRenderer::CreateDelegates(DelegateSet& delegates)
{
    Renderer::CreateDelegates(delegates);
    PerfTest::CreateDelegates(delegates);
}
} // namespace Browser
