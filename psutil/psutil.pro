#-------------------------------------------------
#
# Project created by QtCreator 2019-01-11T14:36:48
#
#-------------------------------------------------

QT       -= gui

TARGET = psutil
TEMPLATE = lib

DEFINES += PSUTIL_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    img_processor.cpp \
    improve_gray_img.cpp \
    improve_img.cpp
HEADERS += \
        psutil.h \
        psutil_global.h \ 



unix {
    target.path = /usr/lib
    INSTALLS += target
}




win32: LIBS += -L$$PWD/../lib/ -llibopencv_world401.dll

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include
