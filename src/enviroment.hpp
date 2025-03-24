#ifndef ENVIROMENT_HPP
#define ENVIROMENT_HPP

#include <QWidget>
#include <QComboBox>

#include "vault.hpp"
#include "json.hpp"

class Enviroment{
public:
    static Enviroment& GetInstance(){
        static Enviroment instance;
        return instance;
    }

    Enviroment(){
        vaults = Json().LoadVaultJson();
    };

    QVector<Vault>* getVaults() const{
        return &vaults;
    };

    Vault* getVault(int index){
        if (index < 0 || index >= vaults.size()){
            return nullptr;
        }
        return &vaults[index];
    }

    void AddNewVault(const Vault& vault, QComboBox* box){
        if (box){
            box->blockSignals(true);
            box->clear();
            getVaults.push_back(vault);
            for (int i = 0; i < getVaults.size(); i++){
                getVaults[i].id = i;
                box->addItem(getVaults[i].displayName);
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
        }else{
            qDebug() << "Error Failed to add vault";
        }
        Json().SaveVaultJson(getVaults);
    }

    void DetachVault(const int& id, QComboBox* box){
        if (box){
            box->blockSignals(true);
            box->clear();
            getVaults.remove(id);
            for (int i = 0; i < getVaults.size(); i++){
                getVaults[i].id = i;
                box->addItem(getVaults[i].displayName);
            }
            box->setCurrentIndex(-1);
            box->blockSignals(false);
        }else{
            qDebug() << "Error Failed to detach vault";
        }
        Json().SaveVaultJson(getVaults);
    }

private:
    QVector<Vault> vaults;

};





#endif // ENVIROMENT_HPP
