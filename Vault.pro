QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    ui/mainwindow.cpp \
    ui/window_crypto.cpp

HEADERS += \
    src/crypto.hpp \
    ui/mainwindow.h \
    src/utils.hpp \
    src/vault.h \
    ui/window_crypto.h

FORMS += \
    ui/mainwindow.ui \
    ui/window_crypto.ui

win32:CONFIG(release, debug|release): LIBS += -LC:/openssl/lib/VC/x64/MT/ -llibcrypto -llibssl
else:win32:CONFIG(debug, debug|release): LIBS += -LC:/openssl/lib/VC/x64/MTd/ -llibcrypto -llibssl

INCLUDEPATH += C:/openssl/include
DEPENDPATH += C:/openssl/include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    asset/resource.qrc


