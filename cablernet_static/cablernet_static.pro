TEMPLATE = lib
CONFIG += staticlib
CONFIG -= app_bundle
CONFIG -= qt

include(../shared.pri)

# MinGW
win32-g++ {
    LIBS += -lws2_32
}
