QT       += core gui sql
QT       += core gui widgets printsupport
QT       += charts
QT       += network
QT +=core gui printsupport
QT += core gui serialport
QT += core gui sql charts printsupport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    clientwindow.cpp \
    connection.cpp \
    employe.cpp \
    main.cpp \
    mainwindow.cpp \
    messagedialog.cpp \
    partenaire.cpp \
    produit.cpp \
    s.cpp \
    smartbin.cpp \
    updateclientdialog.cpp

HEADERS += \
    clientwindow.h \
    connection.h \
    employe.h \
    mainwindow.h \
    messagedialog.h \
    partenaire.h \
    produit.h \
    s.h \
    smartbin.h \
    updateclientdialog.h

FORMS += \
    clientwindow.ui \
    mainwindow.ui \
    updateclientdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc \
    img1.qrc \
    img2.qrc \
    img3.qrc \
    img4.qrc \
    img6.qrc \
    mm.qrc

DISTFILES +=
