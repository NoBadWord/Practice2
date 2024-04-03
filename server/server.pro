QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

LIBS += -lrabbitmq
LIBS += -lprotobuf
LIBS += -lgtest

INCLUDEPATH += ../LoggingCategories
INCLUDEPATH += ../protobuf

SOURCES += \
        main.cpp \
        ../LoggingCategories/LoggingCategories.cpp \
        ../protobuf/proto_file.pb.cpp \
        server.cpp

HEADERS += \
        ../LoggingCategories/LoggingCategories.h \
        ../protobuf/proto_file.pb.h \
        server.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
