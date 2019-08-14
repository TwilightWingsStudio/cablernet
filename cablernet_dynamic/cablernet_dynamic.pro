TEMPLATE = lib
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += CBLRNET_DYNAMIC_LIB
DEFINES += CBLRNET_EXPORTS

include(../shared.pri)

# MinGW
win32-g++ {
    LIBS += -lws2_32
}
