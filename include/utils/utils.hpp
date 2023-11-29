#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include <ctime>

#include <array>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>

#include "string_tools.hpp"
#include "file_tools.hpp"
#include "md5.hpp"
#include "url.hpp"

struct InstanceInfo
{
    HWND hWnd = nullptr;
    std::wstring mutex_name;
    std::wstring class_name;
    std::wstring window_name;
};

namespace MBase
{
namespace
{
GUID CreateGuid()
{
    GUID guid;
    CoCreateGuid(&guid);
    return guid;
}

std::string GuidToString(const GUID& guid)
{
    char buf[64] = {0};
    _snprintf_s(buf,
                sizeof(buf),
                "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1,
                guid.Data2,
                guid.Data3,
                guid.Data4[0],
                guid.Data4[1],
                guid.Data4[2],
                guid.Data4[3],
                guid.Data4[4],
                guid.Data4[5],
                guid.Data4[6],
                guid.Data4[7]);
    return std::string(buf);
}
} // namespace
class Utils
{
public:
    static std::wstring GetUUID()
    {
        std::string uuid = GuidToString(CreateGuid());
        if (!uuid.empty())
        {
            return StringTools::A2W(uuid);
        }
        return L"";
    }

    template <class T>
    static std::wstring GetDecimal(T in_Value, int in_DecimalSeparator)
    {
        //! Return a decimal in base of decimal separator
        std::wstringstream ss;
        ss << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right);
        ss << std::setprecision(in_DecimalSeparator) << in_Value;
        return ss.str();
    }

    static std::wstring GetTimeMd5()
    {
        std::time_t ms = std::time(nullptr);
        std::string strTime = std::to_string(ms);
        std::string md5 = Md5::BufferDigest(strTime);
        return StringTools::A2W(md5);
    }

    static bool IsHaveInstance(HWND hWnd,
                               std::wstring mutexName,
                               std::wstring className,
                               std::wstring windowName)
    {
        InstanceInfo i;
        i.hWnd = hWnd;
        i.mutex_name = mutexName;
        i.class_name = className;
        i.window_name = windowName;
        return IsHaveInstance(i);
    }

    static bool IsHaveInstance(const InstanceInfo& info)
    {
        // 单实例检测
        bool bExist = false;
        HWND hWnd = nullptr;

        do
        {
            if (info.hWnd && IsWindow(info.hWnd))
            {
                bExist = true;
                hWnd = info.hWnd;
                break;
            }

            HANDLE hMutex = ::CreateMutexW(NULL, TRUE, info.mutex_name.c_str());
            if (hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
            {
                bExist = true;
                hWnd = FindWindowW(info.class_name.c_str(), info.window_name.c_str());
                ::ReleaseMutex(hMutex);
                break;
            }

        } while (0);

        if (bExist && hWnd)
        {
            ShowWindow(hWnd, SW_RESTORE);
            BringWindowToTop(hWnd);
            SetForegroundWindow(hWnd);
        }

        return bExist;
    }

    static void StartExe(const std::wstring& exePath, const std::wstring& cmdLine = L"")
    {
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        GetStartupInfo(&si);
        si.wShowWindow = SW_SHOW;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES | STARTF_RUNFULLSCREEN;

        // Start the child process.
        BOOL bRet = CreateProcess(exePath.c_str(),
                                  (LPWSTR)cmdLine.c_str(),
                                  NULL,  // Process handle not inheritable
                                  NULL,  // Thread handle not inheritable
                                  FALSE, // Set handle inheritance to FALSE
                                  0,     // No creation flags
                                  NULL,  // Use parent's environment block
                                  NULL,  // Use parent's starting directory
                                  &si,   // Pointer to STARTUPINFO structure
                                  &pi    // Pointer to PROCESS_INFORMATION structure
        );

        // Wait until child process exits.
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    static void KillProcessByName(const std::wstring& name)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
        PROCESSENTRY32W pEntry{};
        pEntry.dwSize = sizeof(pEntry);
        BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
        while (hRes)
        {
            std::wstring exe_file{pEntry.szExeFile};
            std::transform(exe_file.begin(), exe_file.end(), exe_file.begin(), ::tolower);
            if (exe_file == name)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
                if (hProcess != NULL && pEntry.th32ProcessID != GetCurrentProcessId())
                {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
            hRes = Process32NextW(hSnapShot, &pEntry);
        }
        CloseHandle(hSnapShot);
    }

    static std::wstring VectorToString(const std::vector<std::wstring>& V,
                                       const std::wstring& separator)
    {
        std::wstring result;
        for (const auto& v : V)
        {
            result += v;
            if (v != V.back())
            {
                result += separator;
            }
        }
        return result;
    }

    static std::wstring GetLogPath(const std::wstring& logName)
    {
        TCHAR szModuleFileName[MAX_PATH] = {0};
        TCHAR drive[_MAX_DRIVE] = {0};
        TCHAR dir[_MAX_DIR] = {0};
        TCHAR fname[_MAX_FNAME] = {0};
        TCHAR ext[_MAX_EXT] = {0};

        if (NULL == GetModuleFileName(NULL, szModuleFileName, MAX_PATH))
            return {};

        //分割该路径，得到盘符，目录，文件名，后缀名
        _tsplitpath_s(
            szModuleFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

        std::wstring logDir{dir};
        logDir += L"logs\\";

        TCHAR szLogPath[_MAX_PATH] = {0};
        _tmakepath_s(szLogPath, _MAX_PATH, drive, logDir.c_str(), logName.c_str(), L".log");

        return std::wstring{szLogPath};
    }

    static bool InitResource(const UILIB_RESTYPE& resType,
                             const std::wstring& uiResPath,
                             const std::wstring& uiZipName = L"")
    {
        // 资源类型
        CPaintManagerUI::SetResourceType(resType);

        // 加载资源
        switch (resType)
        {
            case UILIB_FILE:
            {
                CPaintManagerUI::SetResourcePath(uiResPath.c_str());
                break;
            }
            case UILIB_RESOURCE:
            {
                CPaintManagerUI::SetResourcePath(uiResPath.c_str());
                break;
            }
            case UILIB_ZIP:
            {
                CPaintManagerUI::SetResourcePath(uiResPath.c_str());
                CPaintManagerUI::SetResourceZip(uiZipName.c_str(), true); //xxx.zip
                break;
            }
            case UILIB_ZIPRESOURCE:
            {
                HRSRC hResource = ::FindResource(
                    CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
                if (hResource)
                {
                    HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
                    if (hGlobal)
                    {
                        DWORD dwSize =
                            ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
                        if (dwSize > 0)
                        {
                            CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal),
                                                            dwSize);
                        }
                        ::FreeResource(hGlobal);
                    }
                }
                break;
            }
            default: break;
        }
        return true;
    }

    static std::wstring GetResourceDefaultCover(const std::wstring& category)
    {
        if (category == L"picture")
        {
            return L"image\\defcover\\def-image.png";
        }
        else if (category == L"audio")
        {
            return L"image\\defcover\\def-audio.png";
        }
        else if (category == L"video")
        {
            return L"image\\defcover\\def-video.png";
        }
        else if (category == L"animation")
        {
            return L"image\\defcover\\def-widget.png";
        }
        else if (category == L"courseware")
        {
            return L"image\\defcover\\def-course.png";
        }
        else if (category == L"document")
        {
            return L"image\\defcover\\def-design.png";
        }
        else if (category == L"pdf")
        {
            return L"image\\defcover\\def-pdf.png";
        }
        else if (category == L"paper")
        {
            return L"image\\defcover\\def-paper.png";
        }
        else if (category == L"rich_text")
        {
            return L"image\\defcover\\def-others.png";
        }
        else if (category == L"404")
        {
            return L"image\\defcover\\404.png";
        }
        else if (category == L"picture_small")
        {
            return L"image\\defcover\\default-picture-small.png";
        }

        return L"";
    }

    static bool IsValidSuffix(const std::wstring& wstrSuffix)
    {
        auto target = StringTools::ToLower(wstrSuffix);
        const static std::array<std::wstring, 11> vSuffixs{L"ppt",
                                                           L"pptx",
                                                           L"doc",
                                                           L"docx",
                                                           L"mp3",
                                                           L"mp4",
                                                           L"jpg",
                                                           L"jpeg",
                                                           L"png",
                                                           L"swf",
                                                           L"pdf"};

        auto got = std::find_if(vSuffixs.begin(), vSuffixs.end(), [&](const std::wstring& suffix) {
            return suffix == target;
        });

        return (got != std::end(vSuffixs));
    }

    static bool LoadUIResource()
    {
        CPaintManagerUI::SetResourceType(UILIB_FILE);

        //获取程序所在的上一级目录
        std::wstring uiPath = FileTools::GetAppDirectory();
        auto pos = uiPath.find_last_of(L"\\/");
        if (pos != std::wstring::npos)
        {
            uiPath = uiPath.substr(0, pos);
        }

        if (FileTools::FilePathIsExist(uiPath.c_str(), true))
        {
            uiPath = std::format(L"{}\\uires\\", uiPath);
            CPaintManagerUI::SetResourcePath(uiPath.c_str());
            return true;
        }
        return false;
    }
};
} // namespace MBase