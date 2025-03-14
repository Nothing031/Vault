QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    ui/mainwindow.cpp \
    ui/window_crypto.cpp \
    ui/window_newvault.cpp

HEADERS += \
    src/crypto.hpp \
    src/file_t.hpp \
    src/filelistmodel.hpp \
    src/json.h \
    ui/mainwindow.h \
    src/utils.hpp \
    src/vault.h \
    ui/window_crypto.h \
    ui/window_newvault.h

FORMS += \
    ui/mainwindow.ui \
    ui/window_crypto.ui \
    ui/window_newvault.ui

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


