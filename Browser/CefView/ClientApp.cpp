#include "stdafx.h"

#include "ClientApp.h"

#include "include/cef_command_line.h"

namespace Browser {
namespace {
// These flags must match the Chromium values.
const char kProcessType[] = "type";
const char kRendererProcess[] = "renderer";
#if defined(OS_LINUX)
const char kZygoteProcess[] = "zygote";
#endif

} // namespace

ClientApp::ClientApp() {}

// static
ClientApp::ProcessType ClientApp::GetProcessType(CefRefPtr<CefCommandLine> command_line)
{
    // The command-line flag won't be specified for the browser process.
    if (!command_line->HasSwitch(kProcessType))
    {
        return BrowserProcess;
    }

    const std::string& process_type = command_line->GetSwitchValue(kProcessType);
    if (process_type == kRendererProcess)
    {
        return RendererProcess;
    }
#if defined(OS_LINUX)
    else if (process_type == kZygoteProcess)
    {
        return ZygoteProcess;
    }
        
#endif
    return OtherProcess;
}

bool ClientApp::IsBrowser(CefRefPtr<CefCommandLine> command_line)
{
    return BrowserProcess == GetProcessType(command_line);
}

bool ClientApp::IsRenderer(CefRefPtr<CefCommandLine> command_line)
{
    return RendererProcess == GetProcessType(command_line);
}

} // namespace Browser
