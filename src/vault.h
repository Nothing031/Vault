#ifndef JSONMANAGER_H
#define JSONMANAGER_H
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <QMessageBox>


#include <QDebug>
#include "utils.hpp"
#include <src/json.hpp>


using namespace std;
using namespace nlohmann;
namespace fs = std::filesystem;



typedef struct __FILE_STRUCT__{
    fs::path path;
    QString relativePath;
    bool encrypted;
} FILE_STRUCT;

typedef struct __VAULT_STRUCT__{
    wstring directory;
    string password;
} VAULT_STRUCT;

typedef struct __MAINHUB_STRUCT__{
    vector<VAULT_STRUCT> vaults;
} MAINHUB;

class VAULTS{
private:
    static MAINHUB hub;

public:
    static bool json_to_struct(const json& data, MAINHUB& out){
        try{
            out = {};
            // read from json
            for (const auto& jVault : data.at("vaults")){
                VAULT_STRUCT vault;
                vault.directory = Utils::string_to_wstring(jVault.at("directory"));
                vault.password = jVault.at("password");
                out.vaults.push_back(vault);
            }

            return true;
        }catch(...){
            out = {};
            return false;
        }
    }
    static bool struct_to_json(const MAINHUB& hub, json& out){
        try{
            out = {};
            // read from hub
            out["vaults"] = json::array();
            for (const auto& hVault : hub.vaults){
                json jVault;
                jVault["directory"] = Utils::wstring_to_string(hVault.directory);
                jVault["password"] = hVault.password;
                out["vaults"].push_back(jVault);
            }
            return true;
        } catch(...){
            out = {};
            return false;
        }
    }

    static void load(){
        // read json
        fs::path jsonPath = fs::current_path().append("data.json");
        fs::path backupDirPath = fs::current_path().append("backup");
        json jData;

        // if json not exists
        if (!fs::exists(jsonPath)){
            qDebug() << "json not found";
            // create json
            hub = {};
            save();

            // end
            return;
        }

        // try parse
        ifstream f(jsonPath);
        try{
            jData = json::parse(f);
            f.close();
        }catch(...){
            f.close();
            // backup
            if (!fs::exists(fs::current_path().append("backup"))){
                fs::create_directory(fs::current_path().append("backup"));
            }
            std::string newFileName = Utils::getTime().append("_backup.json");
            fs::path newFilePath = backupDirPath.append(newFileName);
            fs::rename(jsonPath, newFilePath);
            // create json
            hub = {};
            save();
            // error dialog
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "failed to load json");
            return;
        }

        // json to struct
        if (json_to_struct(jData, hub)){
            return;
        }else{
            // backup
            if (!fs::exists(fs::current_path().append("backup"))){
                fs::create_directory(fs::current_path().append("backup"));
            }
            std::string newFileName = Utils::getTime().append("_backup.json");
            fs::path newFilePath = backupDirPath.append(newFileName);
            fs::rename(jsonPath, newFilePath);
            // create json
            hub = {};
            save();

            // error dialog
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "failed to load json");
            return;
        }
    }

    static void save(){
        fs::path jsonPath = fs::current_path().append("data.json");
        json jData;
        // struct to json
        struct_to_json(hub, jData);
        // save
        ofstream of(jsonPath, ios::trunc);
        of << jData.dump(2);
        of.close();
    }

    static inline const vector<VAULT_STRUCT>& getVaults(){
        return hub.vaults;
    }

    static void addOrReplaceVault(const VAULT_STRUCT& target){
        for (auto& hVault : hub.vaults){
            if (hVault.directory == target.directory){
                hVault = target;
                return;
            }
        }
        // if not found
        hub.vaults.push_back(target);
    }
};




#endif // JSONMANAGER_H
