#-------------------------------------------------
#
# Project created by QtCreator 2018-02-09T10:11:53
#
#-------------------------------------------------

QT       += core opengl gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LBC_shaders
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += external/triangle/triangle.c \
        main.cpp \
        glwidget.cpp \
        DataSetup.cpp \
        LBCSolver.cpp \
        LBCUI.cpp \
        OpenGL.cpp \
        LBCUI_3D.cpp

HEADERS += external/triangle/triangle.h \
        glwidget.h \
        DataSetup.h \
        ui_glwidget.h \
        LBCSolver.h \
        LBCUI.h \
        OpenGL.h \
        common.h \
        shaders.h \
        LBCUI_3D.h

FORMS += glwidget.ui

DEFINES += QT_DEPRECATED_WARNINGS \
            TRILIBRARY \
            ANSI_DECLARATORS      # trinagulate() function and others

INCLUDEPATH *= $${_PRO_FILE_PWD_}/external/eigen
INCLUDEPATH *= /usr/local/include/eigen3/
INCLUDEPATH *= /usr/include/eigen3/

DESTDIR = $${OUT_PWD}

LIBS +=   -lgomp #  -lCGAL           # omp_get_wtime

QMAKE_CXXFLAGS+= -fopenmp

QMAKE_CFLAGS_DEBUG += -fopenmp          # for debug
# QMAKE_CFLAGS_RELEASE += -fopenmp      # for release

QMAKE_POST_LINK = copy $$shell_path($${_PRO_FILE_PWD_}/colorbar_texture.png) $$shell_path($${OUT_PWD}) /Y
