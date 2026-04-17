cmake_minimum_required(VERSION 3.10)
project(CPU_Scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt GUI build support
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# GUI target
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(QT_WIDGETS_TARGET Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_WIDGETS_TARGET Qt5::Widgets)
endif()

set(GUI_SOURCES
    ${PROJECT_SOURCE_DIR}/GUI/main.cpp
    ${PROJECT_SOURCE_DIR}/GUI/widget.cpp
    ${PROJECT_SOURCE_DIR}/GUI/NonLiveMode.cpp
    ${PROJECT_SOURCE_DIR}/GUI/LiveMode.cpp
    ${PROJECT_SOURCE_DIR}/Process/Process.cpp
    ${PROJECT_SOURCE_DIR}/First-Come-First-Serve/FCFS.cpp
    ${PROJECT_SOURCE_DIR}/Round-Robin/Round_Robin.cpp
    ${PROJECT_SOURCE_DIR}/Shortest-Job-First/Non_Preemptive_SJF.cpp
    ${PROJECT_SOURCE_DIR}/Shortest-Job-First/Preemptive_SJF.cpp
    ${PROJECT_SOURCE_DIR}/Priority-Scheduler/Non_Preemptive_priority.cpp
    ${PROJECT_SOURCE_DIR}/Priority-Scheduler/Preemptive_priority.cpp
    ${PROJECT_SOURCE_DIR}/Live-Scheduling-Manager/SchedulingManager.cpp
)

set(GUI_HEADERS
    ${PROJECT_SOURCE_DIR}/GUI/widget.hpp
    ${PROJECT_SOURCE_DIR}/GUI/NonLiveMode.hpp
    ${PROJECT_SOURCE_DIR}/GUI/LiveMode.hpp
)

set(GUI_FORMS
    ${PROJECT_SOURCE_DIR}/GUI/widget.ui
    ${PROJECT_SOURCE_DIR}/GUI/NonLiveMode.ui
    ${PROJECT_SOURCE_DIR}/GUI/LiveMode.ui
)

add_executable(CPU_Scheduler
    ${GUI_SOURCES}
    ${GUI_HEADERS}
    ${GUI_FORMS}
)

target_include_directories(CPU_Scheduler PRIVATE
    ${PROJECT_SOURCE_DIR}/GUI
    ${PROJECT_SOURCE_DIR}/Process
    ${PROJECT_SOURCE_DIR}/First-Come-First-Serve
    ${PROJECT_SOURCE_DIR}/Round-Robin
    ${PROJECT_SOURCE_DIR}/Shortest-Job-First
    ${PROJECT_SOURCE_DIR}/Priority-Scheduler
    ${PROJECT_SOURCE_DIR}/Live-Scheduling-Manager
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(CPU_Scheduler PRIVATE ${QT_WIDGETS_TARGET})

set_target_properties(CPU_Scheduler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)