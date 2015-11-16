TEMPLATE = app
TARGET =
DEPENDPATH  +=  src
INCLUDEPATH +=  src
OBJECTS_DIR = build

# Input
SOURCES += main.cpp
HEADERS += Debug.h

SOURCES += LoopTimer.cpp
HEADERS += LoopTimer.h

SOURCES += Regul.cpp
HEADERS += Regul.h
SOURCES += PID_v1.cpp
HEADERS += PID_v1.h

SOURCES += Sensor.cpp
HEADERS += Sensor.h

# The submodules
include($$PWD/mod/qpiOneWire/mod.pri)

SOURCES += BigPWM.cpp
HEADERS += BigPWM.h
SOURCES += Output.cpp
HEADERS += Output.h
