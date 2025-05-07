#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <Windows.h>
#include <chrono>
#include <ostream>
#include <sstream>
#include <iostream>
#include <QThread>

#include <ctime>

namespace Utils{

    inline static void Sleep(const int& time, const int& resolution, const bool& flag){
        clock_t start = clock();
        while(flag && time >= clock() - start){
            QThread::msleep(resolution);
        }
    }
    inline static void Sleep(const int& time){
        QThread::msleep(time);
    }

    static std::string getTime(){
        std::chrono::time_point<std::chrono::system_clock> chrono_now = std::chrono::system_clock::now();
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
