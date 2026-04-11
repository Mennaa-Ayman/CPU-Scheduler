cmake_minimum_required(VERSION 3.10)
project(CPU_Scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(CPU_Scheduler PRIVATE
    ${PROJECT_SOURCE_DIR}/Round-Robin
    ${PROJECT_SOURCE_DIR}/Process
    # ${PROJECT_SOURCE_DIR}/First-Come-First-Serve
    # ${PROJECT_SOURCE_DIR}/Priority-Scheduler
    # ${PROJECT_SOURCE_DIR}/Shortest-Job-First
)

# Source files
file(GLOB PROJECT_SOURCES
    "${PROJECT_SOURCE_DIR}/*.cpp"
    "${PROJECT_SOURCE_DIR}/Round-Robin/*.cpp"
    "${PROJECT_SOURCE_DIR}/Process/*.cpp"
    # "${PROJECT_SOURCE_DIR}/First-Come-First-Serve/*.cpp"
    # "${PROJECT_SOURCE_DIR}/Priority-Scheduler/*.cpp"
    # "${PROJECT_SOURCE_DIR}/Shortest-Job-First/*.cpp"

)

# Create executable
add_executable(CPU_Scheduler ${PROJECT_SOURCES})



# Set output directory to project root
set_target_properties(CPU_Scheduler PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)