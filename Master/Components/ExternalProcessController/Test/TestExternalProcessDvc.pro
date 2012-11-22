!include("ExternalProcessTest.pri") {
    error("ExternalProcessTest.pri not found")
}

QT += xml \
      xmlpatterns \
      network

TARGET = utTestExternalProcessDvc

HEADERS += TestExternalProcessDvc.h
           #DerivedController.h

SOURCES += TestExternalProcessDvc.cpp
           #DerivedController.cpp

UseLibs(Global EventHandler DataLogging StateMachines NetworkComponents ExternalProcessController Threads)
