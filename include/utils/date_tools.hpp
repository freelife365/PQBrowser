#pragma once

#include <iomanip> // put_time

#include <date/date.h>

namespace MBase
{
class DateTools
{
public:
    static std::string GetDateString()
    {
        auto now = std::chrono::system_clock::now();
        auto today = date::floor<date::days>(now);
        auto ymd = date::year_month_day{today};

        std::stringstream ss;
        ss << ymd;

        std::string date = ss.str();
        ss.str(""); //清空缓冲区

        return date;
    }

    static std::string GetCurrentDate()
    {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
        //  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
        return ss.str();
    }

    static std::string GetTimestamp(const std::string dateTime)
    {
        //时间格式 2022-3-8 16:00:00
        std::istringstream in{dateTime};
        date::sys_seconds tp;
        in >> parse("%F %T", tp);

        auto real_tp = tp + std::chrono::hours{-8};

        std::stringstream ss;
        ss << real_tp.time_since_epoch().count();
        std::string timestamp = ss.str();
        ss.str(""); //清空缓冲区

        return timestamp;
    }
};
} // namespace MBase