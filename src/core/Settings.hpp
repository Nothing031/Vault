#pragma once

#include <QWidget>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <thread>

class Settings{
public:
    static Settings& GetInstance(){
        static Settings instance;
        return instance;
    }

    int GetThreadCount() const{
        return threadCount;
    }
    void SetThreadCount(int val){
        threadCount = val;
    }


private:
    Settings(){


    }



    int threadCount = std::thread::hardware_concurrency();
};




