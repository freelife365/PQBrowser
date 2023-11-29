#pragma once

#include <string>
#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace MBase
{

class UrlTools
{
public:
    static std::string UrlEncode(std::string text)
    {
        std::ostringstream escaped;
        for (auto i = text.begin(); i != text.end(); ++i)
        {
            std::string::value_type c = (*i);
            // Keep alphanumeric and other accepted characters intact
            if (isalnum((unsigned char)c) || (c == '-') || (c == '_') || (c == '.') || (c == '~'))
            {
                escaped << c;
            }
            // else if (c == ' ') {
            //	escaped << '+';
            // }
            else
            {
                // Any other characters are percent-encoded
                escaped << '%';
                escaped << to_hex((unsigned char)c >> 4);
                escaped << to_hex((unsigned char)c);
            }
        }
        return escaped.str();
    }

    static std::string UrlDecode(std::string text)
    {
        std::ostringstream unescaped;
        for (auto i = text.begin(); i != text.end(); ++i)
        {
            std::string::value_type c = (*i);
            if (c == '%')
            {
                if (i[1] && i[2])
                {
                    int ch1 = from_hex(i[1]);
                    int ch2 = from_hex(i[2]);
                    char h = (char)(ch1 << 4 | ch2);
                    unescaped << h;
                    i += 2;
                }
            }
            else if (c == '+')
            {
                unescaped << ' ';
            }
            else
            {
                unescaped << c;
            }
        }
        return unescaped.str();
    }

    bool parse(const std::string& urlStr)
    {
        static const std::regex expression(
            //       proto                 host               port
            "^(\?:([^:/\?#]+)://)\?(\\w+[^/\?#:]*)(\?::(\\d+))\?"
            //       path                  file       parameters
            "(/\?(\?:[^\?#/]*/)*)\?([^\?#]*)\?(\\\?(.*))\?");

        std::cmatch groups;
        if (!std::regex_match(urlStr.c_str(), groups, expression))
        {
            // Invalid url
            return false;
        }

        m_protocol = std::string(groups[1].first, groups[1].second);
        m_host = std::string(groups[2].first, groups[2].second);
        std::string portStr(groups[3].first, groups[3].second);
        m_pathWithoutFile = std::string(groups[4].first, groups[4].second);
        m_file = std::string(groups[5].first, groups[5].second);
        m_parameter = std::string(groups[6].first, groups[6].second);
        m_path = m_pathWithoutFile + m_file;

        if (!portStr.empty())
        {
            m_port = atoi(groups[3].first);
        }
        else
        {
            auto it = defaultPortsMap().find(m_protocol);
            if (it != defaultPortsMap().end())
            {
                m_port = it->second;
            }
            else
            {
                // Invalid port
                return false;
            }
        }

        return true;
    }

    const std::string& protocol() const { return m_protocol; }
    const std::string& host() const { return m_host; }
    const int port() const { return m_port; }
    const std::string& path() const { return m_path; }
    const std::string& pathWithoutFile() const { return m_pathWithoutFile; }
    const std::string& file() const { return m_file; }
    const std::string& parameter() const { return m_parameter; }

    std::string hostPort() const
    {
        std::stringstream ss;
        ss << m_host << ':' << m_port;
        return ss.str();
    }

    std::string getParamValue(const std::string& name) const
    {
        if (m_parameter.empty())
        {
            return std::string();
        }

        std::string params = UrlDecode(m_parameter);
        std::smatch result;
        if (std::regex_search(params.cbegin(), params.cend(), result, std::regex(name + "=(.*?)&")))
        {
            // 匹配具有多个参数的url
            // *? 重复任意次，但尽可能少重复
            return result[1];
        }
        else if (std::regex_search(
                     params.cbegin(), params.cend(), result, std::regex(name + "=(.*)")))
        {
            // 匹配只有一个参数的url
            return result[1];
        }
        return std::string();
    }

private:
    static char from_hex(char ch) { return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10; }

    static char to_hex(char code)
    {
        static char hex[] = "0123456789abcdef";
        return hex[code & 15];
    }

    const std::map<std::string, int> initDefaultPortsMap()
    {
        std::map<std::string, int> defaultPortsMap;
        defaultPortsMap["http"] = 80;
        defaultPortsMap["https"] = 443;
        return defaultPortsMap;
    }

    const std::map<std::string, int>& defaultPortsMap()
    {
        static std::map<std::string, int> defaultPortsMap = initDefaultPortsMap();
        return defaultPortsMap;
    }

private:
    int m_port{0};
    std::string m_host;
    std::string m_path;
    std::string m_file;
    std::string m_protocol;
    std::string m_parameter;
    std::string m_pathWithoutFile;
};

inline std::ostream& operator<<(std::ostream& os, const UrlTools& obj)
{
    os << "Url [Host = " << obj.host() << ", Protocol = " << obj.protocol()
       << ", Port = " << obj.port() << "]";
    return os;
}

} // namespace MBase