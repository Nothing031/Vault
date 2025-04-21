#ifndef VAULTMANAGER_HPP
#define VAULTMANAGER_HPP


#include <QObject>
#include <QVector>
#include "vault.hpp"

class VaultManager : QObject{
    Q_OBJECT

public:

    VaultManager(){

    }

    ~VaultManager(){
        for (int i = 0; i < vaults.size(); i++){
            delete vaults[i];
        }
    }

    static VaultManager& GetInstance(){
        static VaultManager instance;
        return instance;
    }

    Vault* GetVault(int index){
        if (index < 0 || index >= vaults.size())
            return nullptr;

        return vaults[index];
    }

    void DetachVault(Vault* pVault){
        for (int i = 0; i < vaults.size(); i++){
            if (vaults[i] == pVault){
                vaults.remove(i, 1);
                delete pVault;
            }
        }
        emit OnDetachVault(pVault);
    }

    void AddVault(Vault* pVault){
        vaults.append(pVault);
        emit AddVault(pVault);
    }

private:
    QVector<Vault*> vaults;
};

#endif // VAULTMANAGER_HPP
