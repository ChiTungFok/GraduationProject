QT       += core gui network xml webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfrinedorgroup.cpp \
    basewindow.cpp \
    chatwindow.cpp \
    client.cpp \
    feedbacklistitem.cpp \
    groupwindow.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    qnchatmessage.cpp \
    registwindow.cpp \
    roomcreate.cpp

HEADERS += \
    addfrinedorgroup.h \
    basewindow.h \
    chatwindow.h \
    client.h \
    feedbacklistitem.h \
    global.h \
    groupwindow.h \
    loginwindow.h \
    mainwindow.h \
    qnchatmessage.h \
    registwindow.h \
    roomcreate.h

FORMS += \
    addfrinedorgroup.ui \
    chatwindow.ui \
    groupwindow.ui \
    loginwindow.ui \
    mainwindow.ui \
    registwindow.ui \
    roomcreate.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

DISTFILES += \
    images/create_room.png


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/release/ -lqxmpp.1.4.0
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/debug/ -lqxmpp.1.4.0
else:unix: LIBS += -L$$PWD/../../../usr/local/lib/ -lqxmpp.1.4.0

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include
