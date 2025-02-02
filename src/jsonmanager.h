#ifndef JSONMANAGER_H
#define JSONMANAGER_H
#include <filesystem>
#include <fstream>
#include <string>
#include <condition_variable>
#include <vector>


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


class JsonManager{
    static bool ReadJson(const std::string& path, json& out){
        if (fs::exists(path)){
            ifstream f(path);
            out = json::parse(out);
            f.close();
            return true;
        }else{
            return false;
        }
    }

    static void WriteJson(const std::string& path, const json& data){


    }
    static bool JsonToStruct(const json& data, bool& out){


    }
    static bool StructToJson(const bool& data, json& out){


    }






};






#endif // JSONMANAGER_H
