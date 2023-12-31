#include "stdafx.h"
#include "MainContext.h"

#include "include/base/cef_logging.h"
#include "include/cef_parser.h"
#include "ClientSwitches.h"
#include <direct.h>
#include <shlobj.h>

#include <algorithm>

namespace Browser
{

namespace
{

// The default URL to load in a browser window.
const char kDefaultUrl[] = "https://www.baidu.com";

MainContext* g_main_context = nullptr;

// Returns the ARGB value for |color|.
cef_color_t ParseColor(const std::string& color)
{
    std::string colorToLower;
    colorToLower.resize(color.size());
    std::transform(color.begin(), color.end(), colorToLower.begin(), ::tolower);

    if (colorToLower == "black")
        return CefColorSetARGB(255, 0, 0, 0);
    else if (colorToLower == "blue")
        return CefColorSetARGB(255, 0, 0, 255);
    else if (colorToLower == "green")
        return CefColorSetARGB(255, 0, 255, 0);
    else if (colorToLower == "red")
        return CefColorSetARGB(255, 255, 0, 0);
    else if (colorToLower == "white")
        return CefColorSetARGB(255, 255, 255, 255);

    // Use the default color.
    return 0;
}
} // namespace

// static
MainContext* MainContext::Inst()
{
    DCHECK(g_main_context);
    return g_main_context;
}

MainContext::MainContext(CefRefPtr<CefCommandLine> command_line,
                         bool terminate_when_all_windows_closed)
    : command_line_(command_line)
    , terminate_when_all_windows_closed_(terminate_when_all_windows_closed)
    , background_color_(CefColorSetARGB(255, 255, 255, 255))
{
    DCHECK(!g_main_context);
    g_main_context = this;

    DCHECK(command_line_.get());

    // Whether windowless (off-screen) rendering will be used.
    use_windowless_rendering_ = command_line_->HasSwitch(Switches::kOffScreenRenderingEnabled);

    if (use_windowless_rendering_ && command_line_->HasSwitch(Switches::kOffScreenFrameRate))
    {
        windowless_frame_rate_ =
            atoi(command_line_->GetSwitchValue(Switches::kOffScreenFrameRate).ToString().c_str());
    }

    // Whether transparent painting is used with windowless rendering.
    const bool use_transparent_painting =
        use_windowless_rendering_ &&
        command_line_->HasSwitch(Switches::kTransparentPaintingEnabled);

#if defined(OS_WIN)
    // Shared texture is only supported on Windows.
    shared_texture_enabled_ =
        use_windowless_rendering_ && command_line_->HasSwitch(Switches::kSharedTextureEnabled);
#endif

    external_begin_frame_enabled_ =
        use_windowless_rendering_ && command_line_->HasSwitch(Switches::kExternalBeginFrameEnabled);

    if (windowless_frame_rate_ <= 0)
    {
// Choose a reasonable default rate based on the OSR mode.
#if defined(OS_WIN)
        windowless_frame_rate_ = shared_texture_enabled_ ? 60 : 30;
#else
        windowless_frame_rate_ = 30;
#endif
    }

    // Enable experimental Chrome runtime. See issue #2969 for details.
    use_chrome_runtime_ = command_line_->HasSwitch(Switches::kEnableChromeRuntime);

    if (use_windowless_rendering_ && use_chrome_runtime_)
    {
        LOG(ERROR) << "Windowless rendering is not supported with the Chrome runtime.";
        use_chrome_runtime_ = false;
    }

    // Whether the Views framework will be used.
    use_views_ = command_line_->HasSwitch(Switches::kUseViews);

    if (use_windowless_rendering_ && use_views_)
    {
        LOG(ERROR) << "Windowless rendering is not supported by the Views framework.";
        use_views_ = false;
    }

    if (command_line_->HasSwitch(Switches::kBackgroundColor))
    {
        // Parse the background color value.
        background_color_ = ParseColor(command_line_->GetSwitchValue(Switches::kBackgroundColor));
    }

    if (background_color_ == 0 && !use_views_)
    {
        // Set an explicit background color.
        background_color_ = CefColorSetARGB(255, 255, 255, 255);
    }

    // |browser_background_color_| should remain 0 to enable transparent painting.
    if (!use_transparent_painting)
    {
        browser_background_color_ = background_color_;
    }
}

MainContext::~MainContext()
{
    g_main_context = nullptr;

    // The context must either not have been initialized,
    // or it must have also been shut down.
    DCHECK(!initialized_ || shutdown_);
}

std::string MainContext::GetConsoleLogPath()
{
    return GetAppWorkingDirectory() + "console.log";
}

std::string MainContext::GetDownloadPath(const std::string& file_name)
{
    TCHAR szFolderPath[MAX_PATH];
    std::string path;

    // Save the file in the user's "My Documents" folder.
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE, NULL, 0, szFolderPath)))
    {
        path = CefString(szFolderPath);
        path += "\\" + file_name;
    }

    return path;
}

std::string MainContext::GetAppWorkingDirectory()
{
    char szWorkingDir[MAX_PATH + 1];
    if (_getcwd(szWorkingDir, MAX_PATH) == NULL)
    {
        szWorkingDir[0] = 0;
    }
    else
    {
        // Add trailing path separator.
        size_t len = strlen(szWorkingDir);
        szWorkingDir[len] = '\\';
        szWorkingDir[len + 1] = 0;
    }
    return szWorkingDir;
}

std::wstring MainContext::GetMainURL()
{
    return main_url_;
}

cef_color_t MainContext::GetBackgroundColor()
{
    return background_color_;
}

bool MainContext::UseChromeRuntime()
{
    return use_chrome_runtime_;
}

bool MainContext::UseViews()
{
    return use_views_;
}

bool MainContext::UseWindowlessRendering()
{
    return use_windowless_rendering_;
}

bool MainContext::TouchEventsEnabled()
{
    return command_line_->GetSwitchValue("touch-events") == "enabled";
}

bool MainContext::UseDefaultPopup()
{
    return !use_windowless_rendering_ && command_line_->HasSwitch(Switches::kUseDefaultPopup);
}

void MainContext::PopulateSettings(CefSettings* settings)
{
    settings->multi_threaded_message_loop =
        command_line_->HasSwitch(Switches::kMultiThreadedMessageLoop);

    CefString(&settings->cache_path) = command_line_->GetSwitchValue(Switches::kCachePath);

    if (command_line_->HasSwitch(Switches::kOffScreenRenderingEnabled))
        settings->windowless_rendering_enabled = true;

    if (browser_background_color_ != 0)
        settings->background_color = browser_background_color_;
}

void MainContext::PopulateBrowserSettings(CefBrowserSettings* settings)
{
    if (command_line_->HasSwitch(Switches::kOffScreenFrameRate))
    {
        settings->windowless_frame_rate =
            atoi(command_line_->GetSwitchValue(Switches::kOffScreenFrameRate).ToString().c_str());
    }
}

BrowserDlgManager* MainContext::GetBrowserDlgManager()
{
    DCHECK(InValidState());
    return root_window_manager_.get();
}

bool MainContext::Initialize(const CefMainArgs& args,
                             const CefSettings& settings,
                             CefRefPtr<CefApp> application,
                             void* windows_sandbox_info)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!initialized_);
    DCHECK(!shutdown_);

    if (!CefInitialize(args, settings, application, windows_sandbox_info))
        return false;

    // Need to create the RootWindowManager after calling CefInitialize because
    // TempWindowX11 uses cef_get_xdisplay().
    root_window_manager_.reset(new BrowserDlgManager(terminate_when_all_windows_closed_));

    // Set the main URL.
    if (command_line_->HasSwitch(Switches::kUrl))
        main_url_ = command_line_->GetSwitchValue(Switches::kUrl);

    initialized_ = true;

    return true;
}

void MainContext::Shutdown()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(initialized_);
    DCHECK(!shutdown_);

    root_window_manager_.reset();

    CefShutdown();

    shutdown_ = true;
}
} // namespace Browser