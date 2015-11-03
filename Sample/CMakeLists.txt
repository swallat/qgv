# enable all QT-Stuff we need
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

set(qgv_TS
        )

set(qgv_UI
        MainWindow.ui
        )

# all files which may be mocced
set(qgv_CPP
        main.cpp
        MainWindow.cpp
        QGraphicsViewEc.cpp
        )

# include with QT_USE selected library parts
#INCLUDE(${QT_USE_FILE})

include_directories(
        ${libqgv_SOURCE_DIR}
        ${libqgv_BINARY_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# make *.h out of *.ui
qt5_wrap_ui(qgv_UI_H ${qgv_UI})

# moc classes
#QT5_AUTOMOC(${qgv_CPP})

# add translations
#QT5_ADD_TRANSLATION(qgv_TRANS ${qgv_TS})

set(neededStuff
        ${qgv_CPP}
        ${qgv_UI_H}
        ${qgv_TRANS}
        )

if (UNIX)
    add_executable(sample ${neededStuff})
elseif (APPLE)
    # it's with gui, so MACOSX_BUNDLE is needed
    add_executable(sample MACOSX_BUNDLE ${neededStuff})
elseif (WIN32)
    # it's with gui, so WIN32 is needed
    add_executable(sample WIN32 ${neededStuff})
endif ()
set_target_properties(sample PROPERTIES
        OUTPUT_NAME "Sample")

target_link_libraries(sample
        ${QT_LIBRARIES}
        qgvcore
        )

#IF(BINDINGS_QT5)
qt5_use_modules(sample Core Widgets)
set(CMAKE_AUTOMOC ON)
#ENDIF(BINDINGS_QT5)

# The COMPONENT option only defines the internal belonging of the INSTALL'ed thing
install(
        TARGETS sample
        RUNTIME DESTINATION ${bin_DIR}
        COMPONENT sample
)
