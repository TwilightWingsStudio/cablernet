TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../cablernet_dynamic/release/ -lcablernet_dynamic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../cablernet_dynamic/debug/ -lcablernet_dynamic
else:unix: LIBS += -L$$OUT_PWD/../../cablernet_dynamic/ -lcablernet_dynamic

INCLUDEPATH += $$PWD/../../
DEPENDPATH += $$PWD/../../cablernet_dynamic
