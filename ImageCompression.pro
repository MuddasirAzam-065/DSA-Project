QT += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = ImageCompression
TEMPLATE = app

# Source files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    image.cpp \
    tree.cpp \
    queue.cpp \
    matrix.cpp

# Header files
HEADERS += \
    mainwindow.h \
    image.h \
    tree.h \
    queue.h \
    matrix.h

# Forms
FORMS += \
    mainwindow.ui

# Resource files (if you have any)
# RESOURCES += Resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
