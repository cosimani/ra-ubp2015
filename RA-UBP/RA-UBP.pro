#-------------------------------------------------
#
# Proyecto para Realidad Aumentada - Investigacion
#
#-------------------------------------------------

QT += core gui opengl network multimedia widgets serialport sql

TEMPLATE = app

DEFINES += NO_DEBUG_ARUCO
DEFINES += NO_DEBUG_OSI

unix:INCLUDEPATH += "/usr/include/GL/"                          # OpenGL
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libglut.so"             # OpenGL

unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_core.so"      # OpenCV
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_highgui.so"   # OpenCV
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_imgproc.so"   # OpenCV
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_objdetect.so" # OpenCV
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_calib3d.so"
unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_ml.so"
#unix:LIBS += "/usr/lib/x86_64-linux-gnu/libopencv_contrib.so"


unix:LIBS += "/usr/lib/x86_64-linux-gnu/lib3ds.so"              # Modelos 3D

# Para ArUco se puso directamente las fuentes
#unix:LIBS += "/usr/local/lib/libaruco.so"                       # Aruco

SOURCES += main.cpp\
           mainwindow.cpp \
           scene.cpp \
    aruco/ar_omp.cpp \
    aruco/arucofidmarkers.cpp \
    aruco/board.cpp \
    aruco/boarddetector.cpp \
    aruco/cameraparameters.cpp \
    aruco/chromaticmask.cpp \
    aruco/cvdrawingutils.cpp \
    aruco/highlyreliablemarkers.cpp \
    aruco/marker.cpp \
    aruco/markerdetector.cpp \
    aruco/subpixelcorner.cpp \
    serial.cpp \
    adminDB.cpp \
    threedwriter.cpp

HEADERS += mainwindow.h \
           model.h \
           scene.h \
           texture.h \
           video.h \
           webimage.h \
           webmusic.h \
           logo.h \
    aruco/ar_omp.h \
    aruco/aruco.h \
    aruco/arucofidmarkers.h \
    aruco/board.h \
    aruco/boarddetector.h \
    aruco/cameraparameters.h \
    aruco/chromaticmask.h \
    aruco/cvdrawingutils.h \
    aruco/exports.h \
    aruco/highlyreliablemarkers.h \
    aruco/marker.h \
    aruco/markerdetector.h \
    aruco/subpixelcorner.h \
    serial.h \
    adminDB.h \
    threedwriter.h

FORMS   += mainwindow.ui
