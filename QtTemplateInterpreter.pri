INCLUDEPATH += $$PWD/..
DEPENDPATH += $$PWD/..

QT += core
CONFIG += c++14
equals(QMAKE_COMPILER, "gcc"): QMAKE_CXXFLAGS += -std=c++14
equals(QMAKE_COMPILER, "msvc"): QMAKE_CXXFLAGS += /std:c++14

HEADERS += \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIAbstractControlBlock.h \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockFor.h \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockIf.h \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockSet.h \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockUnset.h \
    $$PWD/QtTIControlBlock/QtTIControlBlockFabric.h \
    $$PWD/QtTIDefines/QtTIDefines.h \
    $$PWD/QtTIHelperFunction/QtTIAbstractHelperFunction.h \
    $$PWD/QtTIHelperFunction/QtTIHelperFunction.h \
    $$PWD/QtTIHelperFunction/QtTIHelperFunctionArg.h \
    $$PWD/QtTIParser/BracketsExpr/QtTIBracketsNode.h \
    $$PWD/QtTIParser/Logic/QtTIParserLogic.h \
    $$PWD/QtTIParser/Math/QtTIMathAction.h \
    $$PWD/QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h \
    $$PWD/QtTIParser/QtTIParser.h \
    $$PWD/QtTIParser/QtTIParserArgs.h \
    $$PWD/QtTIParser/Math/QtTIParserMath.h \
    $$PWD/QtTIParser/BracketsExpr/QtTIParserBracketsExpr.h \
    $$PWD/QtTIParser/QtTIParserFunc.h \
    $$PWD/QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h \
    $$PWD/QtTIParser/HelperClasses/RegExp.h \
    $$PWD/QtTemplateInterpreter.h \
    $$PWD/QtTemplateInterpreterVersion.h

SOURCES += \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockFor.cpp \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockIf.cpp \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockSet.cpp \
    $$PWD/QtTIControlBlock/ControlBlocks/QtTIControlBlockUnset.cpp \
    $$PWD/QtTIControlBlock/QtTIControlBlockFabric.cpp \
    $$PWD/QtTIParser/Logic/QtTIParserLogic.cpp \
    $$PWD/QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.cpp \
    $$PWD/QtTIParser/QtTIParser.cpp \
    $$PWD/QtTIParser/QtTIParserArgs.cpp \
    $$PWD/QtTIParser/Math/QtTIParserMath.cpp \
    $$PWD/QtTIParser/BracketsExpr/QtTIParserBracketsExpr.cpp \
    $$PWD/QtTIParser/QtTIParserFunc.cpp \
    $$PWD/QtTIParser/TernaryOperator/QtTIParserTernaryOperator.cpp \
    $$PWD/QtTemplateInterpreter.cpp
