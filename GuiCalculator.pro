QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calculation.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    calculation.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Output directory
CONFIG(debug, debug|release) {
    compiled = debug
}
CONFIG(release, debug|release) {
    compiled = release
}

# All temporary files are put in the directory $$compiled
DESTDIR = bin/$$compiled
OBJECTS_DIR = $$DESTDIR
MOC_DIR = $$DESTDIR
RCC_DIR = $$DESTDIR
UI_DIR = $$DESTDIR

# Copy styles folder and shiba.gif to build directory
win32 {
    COPY_DEST = $$replace(OUT_PWD, /, \\)
    system("xcopy /E /I styles $$COPY_DEST\\bin\\$$compiled\\styles")
    system("copy shiba.gif $$COPY_DEST\\bin\\$$compiled\\shiba.gif")
}

mac {
    system("cp -r styles $$OUT_PWD/bin/$$compiled/styles")
    system("cp shiba.gif $$OUT_PWD/bin/$$compiled/shiba.gif")
}

RC_ICONS = shiba.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
