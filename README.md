# CPU Scheduling

## Input: 
Type of scheduler + no of Processes ready to run currently + required 
information about each process according to the scheduler type.

## Operation:
**1. Static Mode:**
- Calculates and displays the full scheduling results (Gantt chart, turnaround times, and waiting times) instantly for a pre-defined batch of processes without a timer.

**2. Dynamic Mode:**
- Simulates the CPU scheduling process in real-time.
- Allows users to pause, resume, and dynamically inject new processes during execution.
- Remaining burst time table is updated as time progresses.

## Project Structure
```
├── Process
|       └── Process.hpp/.cpp files
|
├── First-Come-First-Serve/   
|       └── First-Come-First-Serve.cpp/.hpp files
|
├── Priority-Scheduler/
|       ├── Preemptive_Priority.cpp/.hpp files
|       └── Non_Preemptive_Priority.cpp/hpp files
|
├── Shortest-Job-First/
|       ├── Preemptive_SJF.cpp/.hpp files
|       └── Non_Preemptive_SJF.cpp/hpp files
|
├── Round-Robin
|       └── Round-Robin.cpp/.hpp files
|
├── Live-Schduling-Manager
|       └── SchdulingManager.cpp/.hpp files  #For handling dynamic process adding
|
├──  GUI/
|       ├── LiveMode.cpp/.hpp/.ui files
|       ├── NonLiveMode.cpp/.hpp/.ui files      # Static Process 
|       └── Widget.cpp/.hpp/.ui files           # Main window
|
├── .gitignore
└── CMakeLists.txt     # Building the project
```
## How to run
### Requirements
- C++ 17
- CMake 3.10 
- Qt 5

### Getting Started
**1. Set-Location**
```
 "../CPU_Scheduling_Project"  
```
**2. Build the project**
```
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
**3. Execute it**
```
.\CPU_Scheduler.exe
```