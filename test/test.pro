TEMPLATE = app

QT += qml quick
CONFIG += c++11

INCLUDEPATH += ../utils

LIBS += -lws2_32

SOURCES += main.cpp \
    ../utils/svr_epoll.cpp \
    ../utils/prop.cpp \
    ../utils/socket.cpp \
    ../utils/test.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ../utils/singleton.h \
    ../utils/sysinfo.h \
    ../utils/threadpool.h \
    ../utils/svr_epoll.h \
    ../utils/pool.h \
    ../utils/dl.h \
    ../utils/ar.h \
    ../utils/prop.h \
    ../utils/ring.h \
    ../utils/socket.h
