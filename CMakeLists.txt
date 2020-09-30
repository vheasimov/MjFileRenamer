cmake_minimum_required(VERSION 3.8.2)

project(MjFileRenamer)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Make this a GUI application on Windows
if(WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

find_package(mp3lame CONFIG REQUIRED)

find_package(Qt5 REQUIRED NO_MODULE COMPONENTS 
    Widgets 
    Gui 
    Core 
REQUIRED)

add_executable(MjFileRenamer 
    filesnames.cpp
    main.cpp
    MediaEditModel.cpp
    MediaEditView.cpp
    filesnames.ui
    filesnames.qrc
    mainIcon.rc
)

target_link_libraries(MjFileRenamer PRIVATE 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Widgets 
)
