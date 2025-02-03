QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/vault.cpp

HEADERS += \
    src/crypto.hpp \
    src/json.hpp \
    src/mainwindow.h \
    src/utils.hpp \
    src/vault.h

FORMS += \
    src/mainwindow.ui

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


