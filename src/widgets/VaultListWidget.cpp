#include "VaultListWidget.hpp"

#include "src/widgets/VaultButton.hpp"
#include "src/core/vault/VaultManager.hpp"


VaultListWidget::VaultListWidget(QWidget* parent)
    :QListWidget(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);
}

void VaultListWidget::addVaultButton(std::shared_ptr<Vault> vault)
{
    VaultButton* button = new VaultButton(vault, this);
    QListWidgetItem* item = new QListWidgetItem(this);
    item->setSizeHint(button->sizeHint());
    setItemWidget(item, button);

    connect(button, &VaultButton::requestDetachVault, &VaultManager::GetInstance(), &VaultManager::DetachVault);
    connect(button, &VaultButton::requestOpenVault, this, &VaultListWidget::requestOpenVault);
    connect(button, &VaultButton::clicked, this, [this, button]{
        emit requestOpenVault(button->getVault());
    });

    connect(button, &VaultButton::onSizeHintChange, this, [item](QSize size, VaultButton* self){
        Q_UNUSED(self);
        item->setSizeHint(size);
    });
}

void VaultListWidget::removeVaultButton(std::shared_ptr<Vault> vault)
{
    for (int i = 0; i < count(); ++i) {
        if (qobject_cast<VaultButton* >(itemWidget(item(i)))->getVault() == vault) {
            QListWidgetItem *listItem = takeItem(i);
            delete itemWidget(listItem);
            delete listItem;
            break;
        }
    }
}













