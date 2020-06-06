#-------------------------------------------------
#
# Project created by QtCreator 2020-06-03T17:31:35
#
#-------------------------------------------------

QT      += widgets

TARGET = VerilogNppPlugin
TEMPLATE = lib

DEFINES += VERILOGNPPPLUGIN_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    header_cmd

SOURCES += \
    plugin_agent.cpp \
    plugin_cmd.cpp \
    plugin_interface.cpp

HEADERS += \
    header_cmd/menu_cmd.h \
    header_cmd/notepad_plus_msgs.h \
    header_cmd/sci_position.h \
    header_cmd/scintilla.h \
    header_cmd/scintilla_editor.h \
    plugin_agent.h \
    plugin_cmd.h \
    plugin_interface.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32: LIBS += -luser32