#pragma once

#include <string>
#include <sstream>
#include <fstream>

#include <fmt/format.h>

#include <httplib.h>

#include "string_tools.hpp"
#include "file_tools.hpp"
#include "url.hpp"

namespace MBase
{

struct DownloadParam
{
    std::string url;
    std::string cdn_host;
    std::string save_dir;
    std::string save_path;
};

namespace
{

bool DownLoadFile(const std::string& localPath,
                  const std::string& protol,
                  const std::string& host,
                  const std::string& url,
                  std::string& errMsg)
{
    std::string err("");
    if (!FileTools::CreateDir(localPath.c_str(), err))
    {
        errMsg = fmt::format("CreateDir Failed: {}", err);
        return false;
    }

    std::error_code ec;
    std::string strLocalFileTemp = localPath + ".tmp";
    if (fs::exists(fs::path(strLocalFileTemp)))
    {
        fs::remove(fs::path(strLocalFileTemp), ec);
        if (ec)
        {
            errMsg = fmt::format("Remove tmpFile failed: {}", ec.message());
            return false;
        }
    }

    std::fstream file;
    file.open(strLocalFileTemp, std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
    if (!file.good())
    {
        errMsg = fmt::format("Open tmpFile failed£∫ {}", strLocalFileTemp);
        return false;
    }

    bool bHttpsUrl = (protol == "https");
    std::string body_str("");

    if (bHttpsUrl)
    {
        std::string s = host;

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
        httplib::SSLClient cli(s);
#else
        httplib::Client cli(s);
#endif

        auto res = cli.Get(url.c_str(), [&](const char* data, size_t data_length) {
            if (file.is_open())
            {
                file.write(data, data_length);
                file.flush();
            }
            return true;
        });
    }

    file.close();

    fs::rename(strLocalFileTemp, localPath, ec);
    if (!ec)
    {
        return true;
    }

    errMsg = fmt::format("rename fileTemp to localPath failed: {}", ec.message());
    return false;
}

} // namespace

class HttpTools
{
public:
    static bool DownLoadDocxFile(const DownloadParam& download_param, std::string& errMsg)
    {
        //Ω‚ŒˆUrl
        UrlTools url;
        if (!url.parse(download_param.url))
        {
            errMsg = "url is not correct";
            return false;
        }

        if (download_param.save_path.empty())
        {
            errMsg = "save_path is empty";
            return false;
        }

        if (!FileTools::CheckFileExist(download_param.save_path))
        {
            std::string host =
                download_param.cdn_host.empty() ? url.host() : download_param.cdn_host;
            return DownLoadFile(download_param.save_path, url.protocol(), host, url.path(), errMsg);
        }

        return true;
    }
};
} // namespace MBase