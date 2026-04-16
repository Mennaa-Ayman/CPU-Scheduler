cmake_minimum_required(VERSION 3.10)
project(CPU_Scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt setup
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Include directories
include_directories(CPU_Scheduler PRIVATE
    ${PROJECT_SOURCE_DIR}/Round-Robin
    ${PROJECT_SOURCE_DIR}/Process
    ${PROJECT_SOURCE_DIR}/First-Come-First-Serve
    ${PROJECT_SOURCE_DIR}/Priority-Scheduler
    ${PROJECT_SOURCE_DIR}/Shortest-Job-First
)

# Source files
file(GLOB PROJECT_SOURCES
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/Round-Robin/*.cpp"
    "${PROJECT_SOURCE_DIR}/Process/*.cpp"
    "${PROJECT_SOURCE_DIR}/First-Come-First-Serve/*.cpp"
    "${PROJECT_SOURCE_DIR}/Priority-Scheduler/*.cpp"
    "${PROJECT_SOURCE_DIR}/Shortest-Job-First/*.cpp"
    "${PROJECT_SOURCE_DIR}/GUI/*.cpp"
)

# Create executable
add_executable(CPU_Scheduler WIN32 ${PROJECT_SOURCES}

)

target_link_libraries(CPU_Scheduler Qt6::Core Qt6::Widgets)

# Set output directory to project root
set_target_properties(CPU_Scheduler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)