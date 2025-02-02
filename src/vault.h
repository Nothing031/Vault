#ifndef JSONMANAGER_H
#define JSONMANAGER_H
#include <filesystem>
#include <fstream>
#include <string>
#include <condition_variable>
#include <vector>
#include <QMessageBox>


#include <QDebug>
#include "utils.hpp"
#include <src/json.hpp>


using namespace std;
using namespace nlohmann;
namespace fs = std::filesystem;


typedef struct __VAULT_STRUCT__{
    wstring directory;
} VAULT_STRUCT;


class VAULTS{
private:
    vector<VAULT_STRUCT> vaults;
public:
    static VAULTS& GetInstance(){
        static VAULTS instance;
        return instance;
    }

    void load(){
        fs::path jsonPath = fs::current_path().append("data.json");
        fs::path backupDirPath = fs::current_path().append("backup");
        json jData;
        vector<VAULT_STRUCT> vaultStructVec;
        // load json
        if (fs::exists(jsonPath)){
            ifstream f(jsonPath);
            try{
                jData = json::parse(f);
                f.close();
            }catch(...){
                f.close();
                // backup current json
                if (!fs::exists(fs::current_path().append("backup"))){
                    fs::create_directory(fs::current_path().append("backup"));
                }
                std::string newFileName = Utils::getTime().append("_backup.json");
                fs::path newFilePath = backupDirPath.append(newFileName);
                fs::rename(jsonPath, newFilePath);

                // create new
                save();
                return;
            }
        }else{
            qDebug() << "json not found";
            save();
            return;
        }

        // json to struct
        try{
            VAULT_STRUCT vaultStruct;
            for (const auto& vault : jData.at("vaults")){
                vaultStruct.directory = Utils::string_to_wstring(vault.at("directory"));
                vaultStructVec.push_back(vaultStruct);
            }
            vaults = vaultStructVec;
        }
        catch(const json::exception& e){
            // backup
            qDebug() << e.what();
            if (!fs::exists(fs::current_path().append("backup"))){
                fs::create_directory(fs::current_path().append("backup"));
            }
            std::string newFileName = Utils::getTime().append("_backup.json");
            fs::path newFilePath = backupDirPath.append(newFileName);
            fs::rename(jsonPath, newFilePath);

            // create new
            vaults = {}; // reset
            save();

            QMessageBox msgBox;
            msgBox.critical(0, "Error", "failed to load json");
        }
    }

    void save(){
        fs::path jsonPath = fs::current_path().append("data.json");
        json jData;
        // struct to json
        try{
            jData["vaults"] = json::array();
            json vaultJson;
            for (const auto& vault : vaults){
                vaultJson["directory"] = Utils::wstring_to_string(vault.directory);
                jData["vaults"].push_back(vaultJson);
            }
        }
        catch(const json::exception& e){
            qDebug() << e.what();
            QMessageBox msgBox;
            msgBox.critical(0, "Error", "failed to save json");
            return;
        }

        // save json
        ofstream of(jsonPath, ios::trunc);
        of << jData.dump(2);
        of.close();
    }

    const vector<VAULT_STRUCT>& getVaults(){
        return vaults;
    }

    void setVault(const VAULT_STRUCT& vault){
        for (int i = 0; i < vaults.size(); i++){
            if(vaults[i].directory == vault.directory){
                vaults[i] = vault;
                return;
            }
        }
        // if not found
        addVault(vault);
    }

    void addVault(const VAULT_STRUCT& vault){
        vaults.push_back(vault);
    }

};




#endif // JSONMANAGER_H
