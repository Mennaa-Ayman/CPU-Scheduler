# CPU Scheduling

## Operation:
**1. Static Mode:**
- Calculates and displays the full scheduling results (Gantt chart, turnaround times, and waiting times) instantly for a pre-defined batch of processes without a timer.

**2. Dynamic Mode:**
- Simulates the CPU scheduling process in real-time.
- Allows users to pause, resume, and dynamically inject new processes during execution.
- Remaining burst time table is updated as time progresses.

## Project Structure
```
├── Process/
|       └── Process.hpp/.cpp 
├── First-Come-First-Serve/   
|       └── First-Come-First-Serve.cpp/.hpp 
├── Priority-Scheduler/
|       ├── Preemptive_Priority.cpp/.hpp 
|       └── Non_Preemptive_Priority.cpp/hpp 
├── Shortest-Job-First/
|       ├── Preemptive_SJF.cpp/.hpp 
|       └── Non_Preemptive_SJF.cpp/hpp 
├── Round-Robin/
|       └── Round-Robin.cpp/.hpp 
├── Live-Schduling-Manager/           # For handling adding processes dynamically
|       └── SchdulingManager.cpp/.hpp      
├──  GUI/
|       ├── LiveMode.cpp/.hpp/.ui 
|       ├── NonLiveMode.cpp/.hpp/.ui   
|       └── Widget.cpp/.hpp/.ui           
└── CMakeLists.txt    
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