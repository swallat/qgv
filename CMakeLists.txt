######################
#
#    Copyright (C) 2014 EngSaS - Engineering Solutions and Services Langenbach. All rights reserved.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
######################

cmake_minimum_required(VERSION 3.2)
project(qgv)

set(TARGET_VERSION_MAJOR 0)
set(TARGET_VERSION_MINOR 1)
set(${PROJECT_NAME}_PATCH_LEVEL 0)
set(${PROJECT_NAME}_SOVERSION ${TARGET_VERSION_MAJOR})
set(${PROJECT_NAME}_VERSION
        "${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}")

# INCLUDE(CTest)

# Extending the Module path with the local module path of desuto
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

option(BUILD_DEV "Build development version of qgv" ON)
option(BUILD_DEBUG "Build in debug mode" ON)
option(BINDINGS_QT4 "Build with Qt4" OFF)
option(BINDINGS_QT5 "Build with Qt5" ON)

set(BUILD_DEBUG_WANTED ${BUILD_DEBUG})
set(BUILD_DEV_WANTED ${BUILD_DEV})
set(BINDINGS_QT4_WANTED ${BINDINGS_QT4})
set(BINDINGS_QT5_WANTED ${BINDINGS_QT5})

if (BUILD_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
else ()
    set(CMAKE_BUILD_TYPE Release)
endif ()

#IF(NOT BINDINGS_QT4 AND NOT BINDINGS_QT5)
#    MESSAGE("Trying to autodetect Qt version")
#    FIND_PACKAGE(Qt4 QUIET)
#    IF(Qt4_FOUND OR QT4_FOUND)
#        SET(BINDINGS_QT4 ON)
#        MESSAGE("Found Qt4 --> Prefer Qt4 over Qt5")
#    ELSE(Qt4_FOUND OR QT4_FOUND)
#        SET(BINDINGS_QT5 ON)
#        MESSAGE("Qt4 not found --> Try to build with Qt5")
#    ENDIF(Qt4_FOUND OR QT4_FOUND)
#ENDIF(NOT BINDINGS_QT4 AND NOT BINDINGS_QT5)
#IF(BINDINGS_QT4)
#        # Qt4 must be included after setting the build type
#        FIND_PACKAGE(Qt4 REQUIRED)
#ENDIF(BINDINGS_QT4)
if (BINDINGS_QT5)
    #        IF(BINDINGS_QT4)
    #                MESSAGE("Qt4 and Qt5 enabled --> Preferring Qt4 and therefore disabling Qt5")
    #                SET(BINDINGS_QT5  OFF)
    #        ELSE(BINDINGS_QT4)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Xml REQUIRED)
    find_package(Qt5XmlPatterns REQUIRED)
    find_package(Qt5Sql REQUIRED)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5LinguistTools REQUIRED)
    # Do not automatically link against qtmain
    cmake_policy(SET CMP0020 OLD)
    #        ENDIF(BINDINGS_QT4)
endif (BINDINGS_QT5)
# enable automatic moc by CMAKE
set(CMAKE_AUTOMOC ON)

if (NOT KDE)
    if (NOT APPLE)
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    endif (NOT APPLE)
    if (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQT_NO_EXCEPTIONS -Zc:wchar_t-")
    else (MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -DQT_NO_EXCEPTIONS -fno-check-new -fno-common")
    endif (MSVC)
    if (NOT WIN32 AND NOT APPLE)
        #		SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--fatal-warnings -lc -Wl,--disable-auto-import")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--fatal-warnings -lc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ansi")
    endif (NOT WIN32 AND NOT APPLE)
endif (NOT KDE)
if (BUILD_DEBUG AND NOT MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif (BUILD_DEBUG AND NOT MSVC)
add_definitions(${QT_DEFINITIONS})

# directory to install binaries
set(bin_DIR ${CMAKE_INSTALL_PREFIX}/bin)
if (WIN32)
    set(bin_DIR .)
endif (WIN32)

if (NOT WIN32)
    set(LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/" CACHE STRING "Where to install the library files to")
endif (NOT WIN32)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
if (UNIX)
    #  SET(designer_DIR "/usr/lib/qt4/plugins/designer")
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
    set(include_DIR ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
    set(find_cmake_DIR ${CMAKE_ROOT}/Modules/)
else (UNIX)
    set(LIB_INSTALL_DIR ${bin_DIR})
    set(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
    set(include_DIR ${bin_DIR}/include/${PROJECT_NAME})
    set(find_cmake_DIR ${CMAKE_ROOT}/Modules/)
endif (UNIX)

if (WIN32)
    if (BUILD_DEBUG)
        set(qt_LIBS ${qt_LIBS}/Debug)
    else (BUILD_DEBUG)
        set(qt_LIBS ${qt_LIBS}/Release)
    endif (BUILD_DEBUG)
endif ()

#FIND_PACKAGE(Qt4 REQUIRED)
find_package(Qt5Widgets)
#ADD_DEFINITIONS(${QT_DEFINITIONS})
find_package(Graphviz REQUIRED)

######################
#
# include source tree
#
######################
set(libqgv_SOURCE_DIR ${CMAKE_SOURCE_DIR}/QGVCore)
set(libqgv_BINARY_DIR ${CMAKE_BINARY_DIR}/QGVCore)
add_subdirectory(QGVCore)
add_subdirectory(Sample)

######################
#
# printing choosen configuration
#
######################
message("######################################################")
message("")
message("CONFIGURATION")
message("")
message("CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
message("CMAKE_CXX_FLAGS:           ${CMAKE_CXX_FLAGS}")
message("CMAKE_INSTALL_PREFIX:      ${CMAKE_INSTALL_PREFIX}")
message("")
message("OPTIONS:")
message("  Name                               Wanted     Used")
# MESSAGE("  BUILD_PACKAGE:                     ${BUILD_PACKAGE_WANTED}	${BUILD_PACKAGE}")
message("  BUILD_DEBUG:                       ${BUILD_DEBUG_WANTED}		${BUILD_DEBUG}")
# MESSAGE("  BUILD_TESTING:                     ${BUILD_TESTING_WANTED}		${BUILD_TESTING}")
message("  BUILD_DEV:                         ${BUILD_DEV}	${BUILD_DEV}")
message("  BINDINGS_QT4:                      ${BINDINGS_QT4_WANTED}		${BINDINGS_QT4}")
message("  BINDINGS_QT5:                      ${BINDINGS_QT5_WANTED}		${BINDINGS_QT5}")
# MESSAGE("  L10N_ALL:                          ${L10N_ALL_WANTED}		${L10N_ALL}")
# MESSAGE("    L10N_DE:                         ${L10N_DE_WANTED}		${L10N_DE}")
# MESSAGE("    L10N_EN:                         ${L10N_EN_WANTED}		${L10N_EN}")
# MESSAGE("    L10N_FR:                         ${L10N_FR_WANTED}		${L10N_FR}")
message("")
message("######################################################")

######################
#
# Add uninstall target
#
######################
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")


######################
#
# CPACK
#
######################
set(CPACK_GENERATOR "NSIS")
set(CPACK_PACKAGE_NAME "QGV")
set(CPACK_PACKAGE_VENDOR "")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt interface for Graphviz")
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${TARGET_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${TARGET_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_PATCH_LEVEL})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "qgv")

# add all components which should be installable via the package
if (WIN32)
    set(external_DLL
            qtlibs
            graphvizlibs
            )
endif (WIN32)

set(CPACK_COMPONENTS_ALL
        library
        ${external_DLL}
        headerfiles
        sample
        )

# License file
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)

# Pre selected installation profiles
set(CPACK_ALL_INSTALL_TYPES Full Minimal)

# groups for components
set(CPACK_COMPONENT_GROUP_3RDPARTYLIBS_DISPLAY_NAME "3rd Party Libraries")
set(CPACK_COMPONENT_GROUP_3RDPARTYLIBS_DESCRIPTION "Libraries which are needed, but not belonging to this project. They must not be installed, if they are already installed at path")

# the scheme of the variable names are: CPACK_COMPNONENT_%COMPONENTNAMEINUPPERCASE%_DISPLAY_NAME
# where %COMPONENTNAMEINUPPERCASE% must be replaced by the upper cased name of the component
# see below for example
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "QGV")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION
        "The qgv library")
# SET(CPACK_COMPONENT_LIBRARY_GROUP "Base")
set(CPACK_COMPONENT_LIBRARY_INSTALL_TYPES Full Minimal)

if (BUILD_DEV)
    set(CPACK_COMPONENT_HEADERFILES_DISPLAY_NAME "Header files")
    set(CPACK_COMPONENT_HEADERFILES_DESCRIPTION
            "Header files of qgv library")
    set(CPACK_COMPONENT_HEADERFILES_DEPENDS library)
    set(CPACK_COMPONENT_HEADERFILES_INSTALL_TYPES Full)
endif (BUILD_DEV)

set(CPACK_COMPONENT_QTLIBS_DISPLAY_NAME "QT")
set(CPACK_COMPONENT_QTLIBS_DESCRIPTION
        "Qt libraries")
set(CPACK_COMPONENT_QTLIBS_GROUP "3rdPartyLibs")
set(CPACK_COMPONENT_QTLIBS_INSTALL_TYPES Full)

set(CPACK_COMPONENT_GRAPHVIZLIBS_DISPLAY_NAME "Graphviz")
set(CPACK_COMPONENT_GRAPHVIZLIBS_DESCRIPTION
        "Graphviz libraries")
set(CPACK_COMPONENT_GRAPHVIZLIBS_GROUP "3rdPartyLibs")
set(CPACK_COMPONENT_GRAPHVIZLIBS_INSTALL_TYPES Full)

set(CPACK_COMPONENT_SAMPLE_DISPLAY_NAME "Sample")
set(CPACK_COMPONENT_SAMPLE_DESCRIPTION
        "Example application")
set(CPACK_COMPONENT_SAMPLE_DEPENDS library)
set(CPACK_COMPONENT_SAMPLE_INSTALL_TYPES Full)

# used by NSIS to create startmenu links to the executables
# this entrys are only useful if a bin folder is used, otherwise use the empty EXECUTABLES
# and fill out the NSIS_MENU_LINKS
set(CPACK_PACKAGE_EXECUTABLES ${CPACK_PACKAGE_EXECUTABLES} "Sample" "Sample")

# This must always be last!
include(CPack)
