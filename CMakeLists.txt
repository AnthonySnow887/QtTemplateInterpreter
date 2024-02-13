cmake_minimum_required(VERSION 3.7)

#project(QtTemplateInterpreter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_library(QtTemplateInterpreter STATIC
    QtTemplateInterpreter.h
    QtTemplateInterpreterVersion.h
    QtTIControlBlock/ControlBlocks/QtTIAbstractControlBlock.h
    QtTIControlBlock/ControlBlocks/QtTIControlBlockFor.h
    QtTIControlBlock/ControlBlocks/QtTIControlBlockIf.h
    QtTIControlBlock/ControlBlocks/QtTIControlBlockSet.h
    QtTIControlBlock/ControlBlocks/QtTIControlBlockUnset.h
    QtTIControlBlock/QtTIControlBlockFabric.h
    QtTIDefines/QtTIDefines.h
    QtTIHelperFunction/QtTIAbstractHelperFunction.h
    QtTIHelperFunction/QtTIHelperFunction.h
    QtTIHelperFunction/QtTIHelperFunctionArg.h
    QtTIParser/BracketsExpr/QtTIBracketsNode.h
    QtTIParser/Logic/QtTIParserLogic.h
    QtTIParser/Math/QtTIMathAction.h
    QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h
    QtTIParser/QtTIParser.h
    QtTIParser/QtTIParserArgs.h
    QtTIParser/Math/QtTIParserMath.h
    QtTIParser/BracketsExpr/QtTIParserBracketsExpr.h
    QtTIParser/QtTIParserFunc.h
    QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h
    QtTemplateInterpreter.cpp
    QtTIControlBlock/ControlBlocks/QtTIControlBlockFor.cpp
    QtTIControlBlock/ControlBlocks/QtTIControlBlockIf.cpp
    QtTIControlBlock/ControlBlocks/QtTIControlBlockSet.cpp
    QtTIControlBlock/ControlBlocks/QtTIControlBlockUnset.cpp
    QtTIControlBlock/QtTIControlBlockFabric.cpp
    QtTIParser/Logic/QtTIParserLogic.cpp
    QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.cpp
    QtTIParser/QtTIParser.cpp
    QtTIParser/QtTIParserArgs.cpp
    QtTIParser/Math/QtTIParserMath.cpp
    QtTIParser/BracketsExpr/QtTIParserBracketsExpr.cpp
    QtTIParser/QtTIParserFunc.cpp
    QtTIParser/TernaryOperator/QtTIParserTernaryOperator.cpp)

target_link_libraries(QtTemplateInterpreter PRIVATE
    Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(QtTemplateInterpreter INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})
