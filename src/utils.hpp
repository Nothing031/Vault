#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <Windows.h>
#include <stdexcept>
#include <chrono>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace Utils{

    static std::wstring string_to_wstring(const std::string& string)
    {
        if (string.empty())
        {
            return L"";
        }

        const auto size_needed = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);
        if (size_needed <= 0)
        {
            throw std::runtime_error("MultiByteToWideChar() failed: " + std::to_string(size_needed));
        }

        std::wstring result(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), result.data(), size_needed);
        return result;
    }
    static std::string wstring_to_string(const std::wstring& wide_string)
    {
        if (wide_string.empty())
        {
            return "";
        }

        const auto size_needed = WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), nullptr, 0, nullptr, nullptr);
        if (size_needed <= 0)
        {
            throw std::runtime_error("WideCharToMultiByte() failed: " + std::to_string(size_needed));
        }

        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wide_string.data(), (int)wide_string.size(), result.data(), size_needed, nullptr, nullptr);
        return result;
    }

    static std::string getTime(){        std::chrono::time_point<std::chrono::system_clock> chrono_now = std::chrono::system_clock::now();
        std::time_t now = std::chrono::system_clock::to_time_t(chrono_now);
        std::tm utcTime;
        gmtime_s(&utcTime, &now);
        std::ostringstream oss;

        oss << utcTime.tm_year + 1900
            << "." << utcTime.tm_mon + 1
            << "." << utcTime.tm_mday
            << "." << utcTime.tm_hour
            << "." << utcTime.tm_min
            << "." << utcTime.tm_sec;
        return oss.str();
    }

};



#endif // UTILS_HPP
