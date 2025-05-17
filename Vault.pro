QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/core/Crypto.cpp \
    src/core/Vault.cpp \
    src/main.cpp \
    src/widgets/FrontendMainWindow.cpp \
    src/widgets/FrontendNewVault.cpp \
    src/widgets/FrontendVault.cpp \
    src/widgets/VaultSettingsWindow.cpp

HEADERS += \
    src/Manifest.hpp \
    src/core/Crypto.hpp \
    src/core/Error.hpp \
    src/core/FileInfo.hpp \
    src/core/FileMetadataLoader.hpp \
    src/core/Settings.hpp \
    src/core/Utils.hpp \
    src/core/Vault.hpp \
    src/core/VaultManager.hpp \
    src/models/FileListModel.hpp \
    src/widgets/FrontendMainWindow.hpp \
    src/widgets/FrontendNewVault.hpp \
    src/widgets/FrontendVault.hpp \
    src/widgets/VaultSettingsWindow.hpp

FORMS += \
    src/forms/FrontendMainWindow.ui \
    src/forms/FrontendNewVault.ui \
    src/forms/FrontendVault.ui \
    src/forms/VaultSettingsWindow.ui

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
