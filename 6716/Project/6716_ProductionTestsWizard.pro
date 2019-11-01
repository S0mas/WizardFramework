QT += quick sql

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += ../main.cpp

RESOURCES += ../../Resources/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

HEADERS += \
    ../../Common/include/AbstractTest.h \
    ../../Common/include/DataBasseConnector.h \
    ../../Common/include/ProductionTestWizardData.h \
    ../../Common/include/TestListModel.h \
    ../../Common/include/TestSuit.h \
    ../include/ProductionTestWizardData_6716.h \
    ../include/ProductionTests_6716.h

LIBS += -L$$PWD/../../../win32/Debug/ -lvisa32
LIBS += -L$$PWD/../../../win32/Debug/ -lbu3100
LIBS += -L$$PWD/../../../win32/Debug/ -lbu3416
LIBS += -L$$PWD/../../../win32/Debug/ -lbu6716
LIBS += -L$$PWD/../../../win32/Debug/ -lt028

PRE_TARGETDEPS += $$PWD/../../../win32/Debug/visa32.lib
PRE_TARGETDEPS += $$PWD/../../../win32/Debug/bu3100.lib
PRE_TARGETDEPS += $$PWD/../../../win32/Debug/bu3416.lib
PRE_TARGETDEPS += $$PWD/../../../win32/Debug/bu6716.lib
PRE_TARGETDEPS += $$PWD/../../../win32/Debug/t028.lib

INCLUDEPATH += $$PWD/../../../visa/include
INCLUDEPATH += $$PWD/../../../bu3100/include
INCLUDEPATH += $$PWD/../../../bu3416/include
INCLUDEPATH += $$PWD/../../../bu6716/include
INCLUDEPATH += $$PWD/../../../bu5716/t028/include

DEPENDPATH += $$PWD/../../../visa/include
DEPENDPATH += $$PWD/../../../bu3100/include
DEPENDPATH += $$PWD/../../../bu3416/include
DEPENDPATH += $$PWD/../../../bu6716/include
DEPENDPATH += $$PWD/../../../bu5716/t028/include












