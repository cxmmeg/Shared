!include("DataManager.pri") {
    error("DataManager.pri not found")
}

TARGET = TestStubDataManager

HEADERS += ../Helper/Include/*.h \
 ../Containers/ContainerBase/Include/*.h \
 ../Containers/DeviceConfiguration/Include/*.h \
 ../Containers/ExportConfiguration/Include/*.h \
 ../Containers/ExportConfiguration/Commands/Include/*.h \
 ../Containers/Racks/Include/*.h \
 ../Containers/Racks/Commands/Include/*.h \
 ../Containers/SWVersions/Include/*.h \
 ../Containers/UserSettings/Include/*.h \
 ../Containers/UserSettings/Commands/Include/*.h \
 ../CommandInterface/Include/*.h \
 ../Include/*.h
 
SOURCES += ../Helper/Source/*.cpp \
 ../Containers/ContainerBase/Source/*.cpp \
 ../Containers/DeviceConfiguration/Source/*.cpp \
 ../Containers/ExportConfiguration/Source/*.cpp \
 ../Containers/ExportConfiguration/Commands/Source/*.cpp \
 ../Containers/Racks/Source/*.cpp \
 ../Containers/Racks/Commands/Source/*.cpp \
 ../Containers/SWVersions/Source/*.cpp \
 ../Containers/UserSettings/Source/*.cpp \
 ../Containers/UserSettings/Commands/Source/*.cpp \
 ../CommandInterface/Source/*.cpp \
 ../Source/*.cpp

INCLUDEPATH += ../../../../../Colorado/ColoradoMain/Gui/Test/Components/
