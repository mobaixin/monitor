QT       += core gui sql serialport
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += D:\Documents\QTProjects\monitor\src\camera\include

#lib
LIBS += D:\Documents\QTProjects\monitor\src\camera\lib\MVCAMSDK.lib
#LIBS += D:\Documents\QTProjects\monitor\src\camera\lib\MVCAMSDK_X64.lib


LIBS += "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\legacy_stdio_definitions.lib"
LIBS += D:\Documents\QTProjects\monitor\src\serialport\lib\Lucero.lib
#LIBS += $$PWD\src\serialport\lib\Lucero.lib


#INCLUDEPATH +=D:\Documents\opencv3.4.0\OpenCV-MinGW-Build-OpenCV-3.4.5\include \
#              D:\Documents\opencv3.4.0\OpenCV-MinGW-Build-OpenCV-3.4.5\include\opencv2

#LIBS +=D:\Documents\opencv3.4.0\OpenCV-MinGW-Build-OpenCV-3.4.5\x86\mingw\bin\libopencv_*.dll

INCLUDEPATH += D:\Documents\opencv3.4.14\opencv\sources\build\install\include \
               D:\Documents\opencv3.4.14\opencv\sources\build\install\include\opencv2

LIBS += D:\Documents\opencv3.4.14\opencv\sources\build\install\x86\vc14\lib\opencv_*.lib

SOURCES += \
    main.cpp \
    src/camera/camera.cpp \
    src/camera/capturethread.cpp \
    src/camera/detect.cpp \
    src/database/mydatabase.cpp \
    src/serialport/myserialport.cpp \
    src/view/bottombar/bottombar.cpp    \
    src/view/common/myselectframe.cpp \
    src/view/common/mysettings.cpp \
    src/view/common/myslider.cpp \
    src/view/imgarea/imgarea.cpp        \
    src/view/imgarea/mygraphicsitem.cpp \
    src/view/imgarea/mygraphicsscene.cpp\
    src/view/imgarea/mypointitem.cpp    \
    src/view/mainwindow.cpp             \
    src/view/sidebar/sidebar.cpp \
    src/view/titlebar/cameraipset.cpp \
    src/view/titlebar/camerapara.cpp \
    src/view/titlebar/iosetting.cpp \
    src/view/titlebar/ngrecord.cpp \
    src/view/titlebar/syssetting.cpp \
    src/view/titlebar/titlebar.cpp \
    src/view/titlebar/optrecord.cpp

HEADERS += \
    src/camera/camera.h \
    src/camera/capturethread.h \
    src/camera/resource.h \
    src/database/mydatabase.h \
    src/serialport/myserialport.h \
    src/serialport/usb2gpio.h \
    src/serialport/usbdevice.h \
    src/view/common/myselectframe.h \
    src/view/common/mysettings.h \
    src/view/common/myslider.h \
    src/view/imgarea/mygraphicsitem.h   \
    src/view/imgarea/mygraphicsscene.h  \
    src/view/imgarea/mypointitem.h  \
    src/view/mainwindow.h           \
    src/view/titlebar/cameraipset.h \
    src/view/titlebar/camerapara.h \
    src/view/titlebar/iosetting.h \
    src/view/titlebar/ngrecord.h \
    src/view/titlebar/syssetting.h \
    src/view/titlebar/titlebar.h    \
    src/view/bottombar/bottombar.h  \
    src/view/sidebar/sidebar.h      \
    src/view/imgarea/imgarea.h      \
    src/model/model.h               \
    src/controller/controller.h \
    src/view/titlebar/optrecord.h

TRANSLATIONS += \
    monitor_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    data/img/page01.png \
    data/ver1.0_monitor.db \
    src/camera/lib/MVCAMSDK.lib \
    src/camera/lib/MVCAMSDK_X64.lib \
    src/serialport/lib/Lucero.lib

RESOURCES += \
    data/database/database.qrc \
    data/img/img.qrc

DISTFILES += \
    lib/MVCAMSDK.lib \
    lib/MVCAMSDK_X64.lib
