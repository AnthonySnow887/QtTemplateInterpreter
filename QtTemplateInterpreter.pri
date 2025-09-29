INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

QT += core
CONFIG += c++14
equals(QMAKE_COMPILER, "gcc"): QMAKE_CXXFLAGS += -std=c++14
equals(QMAKE_COMPILER, "msvc"): QMAKE_CXXFLAGS += /std:c++14

HEADERS += \
    $$PWD/QtTIDefines/QtTIDefines.h \
    $$PWD/QtTIDefines/QtTIRegExpDefines.h \
    $$PWD/QtTIHelperFunction/QtTIAbstractHelperFunction.h \
    $$PWD/QtTIHelperFunction/QtTIHelperFunction.h \
    $$PWD/QtTIHelperFunction/QtTIHelperFunctionArg.h \
    $$PWD/QtTIParser/Abstract/QtTIAbstractParser.h \
    $$PWD/QtTIParser/Abstract/QtTIAbstractParserArgs.h \
    $$PWD/QtTIParser/Abstract/QtTIAbstractParserBlock.h \
    $$PWD/QtTIParser/Abstract/QtTIAbstractParserFunc.h \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIAbstractControlBlock.h \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockFor.h \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockIf.h \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockSet.h \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockUnset.h \
    $$PWD/QtTIParser/ControlBlockFabric/QtTIControlBlockFabric.h \
    $$PWD/QtTIParser/BracketsExpr/QtTIBracketsNode.h \
    $$PWD/QtTIParser/BracketsExpr/QtTIParserBracketsExpr.h \
    $$PWD/QtTIParser/Logic/QtTIParserLogic.h \
    $$PWD/QtTIParser/Math/QtTIMathAction.h \
    $$PWD/QtTIParser/Math/QtTIParserMath.h \
    $$PWD/QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h \
    $$PWD/QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h \
    $$PWD/QtTIParser/HelperClasses/RegExp.h \
    $$PWD/QtTIParser/QtTIParser.h \
    $$PWD/QtTIParser/QtTIParserArgs.h \
    $$PWD/QtTIParser/QtTIParserFunc.h \
    $$PWD/QtTIParser/QtTIParserBlock.h \
    $$PWD/QtTemplateInterpreter.h \
    $$PWD/QtTemplateInterpreterVersion.h

SOURCES += \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockFor.cpp \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockIf.cpp \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockSet.cpp \
    $$PWD/QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockUnset.cpp \
    $$PWD/QtTIParser/ControlBlockFabric/QtTIControlBlockFabric.cpp \
    $$PWD/QtTIParser/Logic/QtTIParserLogic.cpp \
    $$PWD/QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.cpp \
    $$PWD/QtTIParser/TernaryOperator/QtTIParserTernaryOperator.cpp \
    $$PWD/QtTIParser/Math/QtTIParserMath.cpp \
    $$PWD/QtTIParser/BracketsExpr/QtTIParserBracketsExpr.cpp \
    $$PWD/QtTIParser/QtTIParser.cpp \
    $$PWD/QtTIParser/QtTIParserArgs.cpp \
    $$PWD/QtTIParser/QtTIParserFunc.cpp \
    $$PWD/QtTemplateInterpreter.cpp
