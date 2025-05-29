QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/cryptography/CryptoEngine.cpp \
    src/core/cryptography/Error.cpp \
    src/core/fileinfo/FileHeader.cpp \
    src/core/fileinfo/FileInfoLoader.cpp \
    src/core/vault/AES256Settings.cpp \
    src/core/vault/ExcludeChecker.cpp \
    src/core/vault/Vault.cpp \
    src/core/vault/VaultLoader.cpp \
    src/core/vault/VaultManager.cpp \
    src/main.cpp \
    src/widgets/FileInfoTooltipWidget.cpp \
    src/widgets/FileListView.cpp \
    src/widgets/Menu.cpp \
    src/widgets/SettingsWindow.cpp \
    src/widgets/VaultButton.cpp \
    src/widgets/VaultCreateNew.cpp \
    src/widgets/VaultEntryWindow.cpp \
    src/widgets/VaultListWidget.cpp \
    src/widgets/VaultTitle.cpp \
    src/widgets/VaultViewWindow.cpp \
    src/widgets/WrappedLabel.cpp

HEADERS += \
    src/Manifest.hpp \
    src/core/Settings.hpp \
    src/core/Utils.hpp \
    src/core/cryptography/CryptoEngine.hpp \
    src/core/cryptography/Cryptography.hpp \
    src/core/cryptography/Error.hpp \
    src/core/fileinfo/FileHeader.hpp \
    src/core/fileinfo/FileInfo.hpp \
    src/core/fileinfo/FileInfoLoader.hpp \
    src/core/vault/AES256Settings.hpp \
    src/core/vault/ExcludeChecker.hpp \
    src/core/vault/Vault.hpp \
    src/core/vault/VaultLoader.hpp \
    src/core/vault/VaultManager.hpp \
    src/models/FileListModel.hpp \
    src/widgets/FileInfoTooltipWidget.hpp \
    src/widgets/FileListView.hpp \
    src/widgets/Menu.hpp \
    src/widgets/SettingsWindow.hpp \
    src/widgets/VaultButton.hpp \
    src/widgets/VaultCreateNew.hpp \
    src/widgets/VaultEntryWindow.hpp \
    src/widgets/VaultListWidget.hpp \
    src/widgets/VaultTitle.hpp \
    src/widgets/VaultViewWindow.hpp \
    src/widgets/WrappedLabel.hpp

FORMS += \
    src/forms/PasswordInputDialog.ui \
    src/forms/SettingsWindow.ui \
    src/forms/VaultCreateNew.ui \
    src/forms/VaultEntryWindow.ui \
    src/forms/VaultTitle.ui \
    src/forms/VaultViewWindow.ui

win32:CONFIG(release, debug|release): LIBS += -LC:/Library/openssl/3.4.1/lib/VC/x64/MT/ -llibcrypto -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/Library/openssl/3.4.1/lib/VC/x64/MTd/ -llibcrypto -llibssl

INCLUDEPATH += C:/Library/openssl/3.4.1/include
DEPENDPATH += C:/Library/openssl/3.4.1/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    asset/resource.qrc

DISTFILES += \
    .gitignore \
    LICENSE \
    README.md \
    changelog.txt
