# CPU Scheduling

## Types of Schedulers
1. FCFS
2. SJF (Preemptive and Non Preemptive)
3. Priority (Preemptive and Non Preemptive) (the smaller the priority number the higher the priority)
4. Round Robin

## Input: 
- 

## Operation:
- A live scheduler is run with each 1 unit of time mapped to 1 second
- Remaining burst time table is updated as time progresses.
- An option to run the currently existing processes only without live scheduling must be available.

## Outputs:
- Timeline showing the order and time taken by each process (Gantt Chart) drawn live.
- Average waiting time and average turnaround time
- Remaining burst time updated table live


## How to run
**1. Set-Location**
```
 "../CPU_Scheduling_Project"  
```
**2. Build the GUI**
```
cmake --build build-gui --target CPU_Scheduler_GUI
```
**3. Lauch it**
```
CPU_Scheduler_GUI.exe
```