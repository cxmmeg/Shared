#####################################################
###  WARNING:  due to external library dependencies
###            this project file is Linux-specific.
#####################################################

!include("../Build/RemoteCareAgent.pri") {
    error("../Build/RemoteCareAgent.pri not found")
}

extlib.commands = make $$ARCH_SET -C $$AGENTEMBEDDED_PATH
QMAKE_EXTRA_TARGETS += extlib
PRE_TARGETDEPS += extlib

TEMPLATE = app

# config for test
CONFIG += qtestlib \
          warn_on \
          qt \
          thread \
          rtti

QT += core \
      xml \
      xmlpatterns \
      network

HEADERS += ../Include/*.h \

SOURCES += ../Source/AgentController.cpp \
           ../Source/MasterConnector.cpp \
           ../Source/RCConfigurationWrapper.cpp \
           ../Source/REServerConnector.cpp

DESTDIR = bin_$${CONFIG_SUFFIX}

# ################ list used platform libraries #################
PLATFORM_COMPONENTS_DIR = $$PLATFORM_COMPONENTS_PATH
PLATFORM_COMPONENTS = NetworkComponents \
                      NetCommands \
                      Global \
                      DataManager \

# ################ start group
LIBS += -Wl,--start-group

# ################ include platform libraries and set dependencies
for(TheComponent, PLATFORM_COMPONENTS) {
    THELIBPATH = $$PLATFORM_COMPONENTS_DIR/$${TheComponent}/Build/lib_$${CONFIG_SUFFIX}
    PRE_TARGETDEPS += $$THELIBPATH/lib$${TheComponent}.a
    LIBS += $$THELIBPATH/lib$${TheComponent}.a
}

LIBS += ../../Himalaya/Shared/Master/Components/HimalayaDataContainer/Build/lib_$${CONFIG_SUFFIX}/libHimalayaDataContainer.a


LIBS += -Wl,--end-group

# ################ end group

# ################ include Axeda Embedded Agent lib paths:
LIBS += -L$$AGENTEMBEDDED_PATH \
        -L$$AGENTEMBEDDED_PATH/lib/lib \
        -L$$AGENTEMBEDDED_PATH/Libsrc/zlib \
        -L$$AGENTEMBEDDED_PATH/Libsrc/expat/xmlparse
# ################ include Axeda Embedded Agent libs:
LIBS += -lAgentEmbedded \
        -lz \
        -lexpat \
        -lcrypto \
        -lssl
