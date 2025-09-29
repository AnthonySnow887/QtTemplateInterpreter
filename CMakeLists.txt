cmake_minimum_required(VERSION 3.7)

#project(QtTemplateInterpreter VERSION 1.2.0 LANGUAGES CXX)

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
    QtTIDefines/QtTIDefines.h
    QtTIDefines/QtTIRegExpDefines.h
    QtTIHelperFunction/QtTIAbstractHelperFunction.h
    QtTIHelperFunction/QtTIHelperFunction.h
    QtTIHelperFunction/QtTIHelperFunctionArg.h
    QtTIParser/Abstract/QtTIAbstractParser.h
    QtTIParser/Abstract/QtTIAbstractParserArgs.h
    QtTIParser/Abstract/QtTIAbstractParserBlock.h
    QtTIParser/Abstract/QtTIAbstractParserFunc.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIAbstractControlBlock.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockFor.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockIf.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockSet.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockUnset.h
    QtTIParser/ControlBlockFabric/QtTIControlBlockFabric.h
    QtTIParser/BracketsExpr/QtTIBracketsNode.h
    QtTIParser/BracketsExpr/QtTIParserBracketsExpr.h
    QtTIParser/Logic/QtTIParserLogic.h
    QtTIParser/Math/QtTIMathAction.h
    QtTIParser/Math/QtTIParserMath.h
    QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.h
    QtTIParser/TernaryOperator/QtTIParserTernaryOperator.h
    QtTIParser/HelperClasses/RegExp.h
    QtTIParser/QtTIParser.h
    QtTIParser/QtTIParserArgs.h
    QtTIParser/QtTIParserFunc.h
    QtTIParser/QtTIParserBlock.h
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockFor.cpp
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockIf.cpp
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockSet.cpp
    QtTIParser/ControlBlockFabric/ControlBlocks/QtTIControlBlockUnset.cpp
    QtTIParser/ControlBlockFabric/QtTIControlBlockFabric.cpp
    QtTIParser/Logic/QtTIParserLogic.cpp
    QtTIParser/NullCoalescingOperator/QtTIParserNullCoalescingOperator.cpp
    QtTIParser/TernaryOperator/QtTIParserTernaryOperator.cpp
    QtTIParser/Math/QtTIParserMath.cpp
    QtTIParser/BracketsExpr/QtTIParserBracketsExpr.cpp
    QtTIParser/QtTIParser.cpp
    QtTIParser/QtTIParserArgs.cpp
    QtTIParser/QtTIParserFunc.cpp
    QtTemplateInterpreter.cpp)

target_link_libraries(QtTemplateInterpreter PRIVATE
    Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(QtTemplateInterpreter INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR})
